#include "Plugin.h"
#include "PlatformTheme.h"
#include "HNMenu.h"
#include "HNMenuItem.h"

#include <QGuiApplication>
#include <QCoreApplication>
#include <QSocketNotifier>
#include <QByteArray>
#include <QTimer>

#include <qpa/qplatformnativeinterface.h>

#include <CZ/Core/CZCore.h>

#include <wayland-client.h>
#include "lvr-private-handle-client.h"

#include <cstring>

QT_BEGIN_NAMESPACE

// ---------------------------------------------------------------------------
// Heaven object-tree helpers
// ---------------------------------------------------------------------------

HNObject *hnParent(HNObject *object) noexcept
{
    auto *wp = dynamic_cast<HNWithParent*>(object);
    return wp ? wp->parent() : nullptr;
}

HNObject *hnNextSibling(HNObject *object) noexcept
{
    auto *wp = dynamic_cast<HNWithParent*>(object);
    if (!wp || !wp->parent())
        return nullptr;

    auto *wc = dynamic_cast<HNWithChildren*>(wp->parent());
    if (!wc)
        return nullptr;

    const auto &list = wc->children();
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        if (*it == wp)
        {
            auto next = std::next(it);
            return next == list.end() ? nullptr : dynamic_cast<HNObject*>(*next);
        }
    }

    return nullptr;
}

std::string hnIconName(const QIcon &icon) noexcept
{
    return icon.name().toStdString();
}

bool loopedMenu(HNMenu *menu, HNObject *object)
{
    if (!object)
        return false;

    if (object->userData == menu)
        return true;

    return loopedMenu(menu, hnParent(object));
}

// Returns the owning clone's shared_ptr for a Heaven object, or nullptr.
static std::shared_ptr<HNObject> hnCloneSharedPtr(HNObject *object)
{
    if (!object || !object->userData)
        return nullptr;

    if (object->type() == HNObject::Menu)
    {
        HNMenu *m = static_cast<HNMenu*>(object->userData);
        for (MenuClone *c : m->clones)
            if (c->object.get() == object)
                return c->object;
    }
    else
    {
        HNMenuItem *i = static_cast<HNMenuItem*>(object->userData);
        for (ItemClone *c : i->clones)
            if (c->object.get() == object)
                return c->object;
    }

    return nullptr;
}

void destroyObjectAndChildren(std::shared_ptr<HNObject> object)
{
    if (!object)
        return;

    // Recursively destroy children first.
    if (auto *wc = dynamic_cast<HNWithChildren*>(object.get()))
    {
        while (!wc->children().empty())
        {
            HNObject *child = dynamic_cast<HNObject*>(wc->children().back());

            if (std::shared_ptr<HNObject> childPtr = hnCloneSharedPtr(child))
                destroyObjectAndChildren(childPtr);
            else if (auto *cwp = dynamic_cast<HNWithParent*>(child))
                cwp->setParent(nullptr); // safety: detach unknown child
        }
    }

    // Unlink the object's clone from its Qt owner and release it.
    if (object->type() == HNObject::Menu)
    {
        if (HNMenu *m = static_cast<HNMenu*>(object->userData))
            for (MenuClone *c : m->clones)
                if (c->object.get() == object.get())
                {
                    m->clones.removeOne(c);
                    hnUnlinkSubMenu(c);
                    c->object.reset();
                    delete c;
                    break;
                }
    }
    else
    {
        if (HNMenuItem *i = static_cast<HNMenuItem*>(object->userData))
            for (ItemClone *c : i->clones)
                if (c->object.get() == object.get())
                {
                    i->clones.removeOne(c);
                    hnUnlinkSubMenu(c);
                    c->object.reset();
                    delete c;
                    break;
                }
    }
}

// ---------------------------------------------------------------------------
// lvr_private_handle: bind on Qt's wl_display and forward the handle to Heaven
// ---------------------------------------------------------------------------

namespace {

struct PrivateHandleState
{
    std::shared_ptr<HNClient> heaven;
    lvr_private_handle_manager *manager = nullptr;
};

void privateHandle(void *data, lvr_private_handle_manager *manager, const char *handle)
{
    auto *st = static_cast<PrivateHandleState*>(data);
    if (st->heaven)
    {
        st->heaven->setPrivateHandle(handle);
        st->heaven->commit();
    }
    lvr_private_handle_manager_destroy(manager);
    st->manager = nullptr;
}

const lvr_private_handle_manager_listener privateHandleListener {
    .handle = &privateHandle
};

void registryGlobal(void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t version)
{
    Q_UNUSED(version)
    auto *st = static_cast<PrivateHandleState*>(data);

    if (!st->manager && strcmp(interface, lvr_private_handle_manager_interface.name) == 0)
    {
        st->manager = static_cast<lvr_private_handle_manager*>(
            wl_registry_bind(registry, name, &lvr_private_handle_manager_interface, 1));
        lvr_private_handle_manager_add_listener(st->manager, &privateHandleListener, st);
    }
}

void registryGlobalRemove(void *, wl_registry *, uint32_t) {}

const wl_registry_listener registryListener {
    .global = &registryGlobal,
    .global_remove = &registryGlobalRemove
};

// Retrieves Qt's internal wl_display through the platform native interface.
wl_display *qtWaylandDisplay()
{
    QPlatformNativeInterface *ni = QGuiApplication::platformNativeInterface();
    if (!ni)
        return nullptr;

    wl_display *dpy = static_cast<wl_display*>(ni->nativeResourceForIntegration(QByteArrayLiteral("wl_display")));
    if (!dpy)
        dpy = static_cast<wl_display*>(ni->nativeResourceForIntegration(QByteArrayLiteral("display")));
    return dpy;
}

// Binds lvr_private_handle_manager on Qt's display. Events are delivered asynchronously by Qt's own
// Wayland dispatch, so this never blocks nor interferes with Qt's read cycle (no roundtrip here).
//
// The platform theme plugin is created before QtWayland has connected its wl_display, so this is
// scheduled onto the event loop and retried a few times until the display becomes available.
void setupPrivateHandle(std::shared_ptr<HNClient> heaven, int attemptsLeft = 50)
{
    wl_display *dpy = qtWaylandDisplay();

    if (!dpy)
    {
        if (attemptsLeft > 0)
            QTimer::singleShot(50, qApp, [heaven, attemptsLeft]{
                setupPrivateHandle(heaven, attemptsLeft - 1);
            });
        return;
    }

    auto *st = new PrivateHandleState{ heaven, nullptr }; // lives for the process
    wl_registry *registry = wl_display_get_registry(dpy);
    wl_registry_add_listener(registry, &registryListener, st);
    wl_display_flush(dpy);
}

} // namespace

// ---------------------------------------------------------------------------
// Platform theme plugin
// ---------------------------------------------------------------------------

PlatformThemePlugin::PlatformThemePlugin(QObject *parent) : QPlatformThemePlugin(parent)
{
}

QPlatformTheme *PlatformThemePlugin::create(const QString &key, const QStringList &paramList)
{
    Q_UNUSED(key)
    Q_UNUSED(paramList)

    m_core = CZ::CZCore::GetOrMake();
    m_heaven = HNClient::GetOrMake();

    if (!m_core || !m_heaven)
        return nullptr;

    QString appName = QStringLiteral("Qt Application");
    if (!QGuiApplication::applicationName().isEmpty())
        appName = QGuiApplication::applicationName();

    m_heaven->setName(appName.toStdString());
    m_heaven->commit();

    connect(QGuiApplication::instance(), &QCoreApplication::applicationNameChanged,
            this, &PlatformThemePlugin::appNameChanged);

    // Default application-menu actions coming from the bar. About/Settings are routed to the app's
    // own role-tagged actions (if any); Quit falls back to a plain application quit.
    m_heaven->onAbout.subscribe(&m_heavenListener, []{
        if (auto *item = HNMenuItem::itemForRole(QPlatformMenuItem::AboutRole))
            emit item->activated();
    });
    m_heaven->onSettings.subscribe(&m_heavenListener, []{
        if (auto *item = HNMenuItem::itemForRole(QPlatformMenuItem::PreferencesRole))
            emit item->activated();
    });
    m_heaven->onQuit.subscribe(&m_heavenListener, []{
        if (auto *item = HNMenuItem::itemForRole(QPlatformMenuItem::QuitRole))
            emit item->activated();
        else
            QCoreApplication::quit();
    });

    // Drive the Heaven (D-Bus) event loop from Qt's event loop.
    //
    // The QSocketNotifier must be created once the application's event dispatcher is up and running.
    // create() runs during platform-theme initialization, before QCoreApplication::exec(), so a
    // notifier created here never registers with the event loop: outgoing traffic still gets flushed
    // opportunistically, but incoming D-Bus events (the bar appearing, click deliveries) are never
    // delivered. Defer it to the first event-loop iteration, exactly like setupPrivateHandle below.
    QTimer::singleShot(0, this, [this]{
        QSocketNotifier *sn = new QSocketNotifier(m_core->fd(), QSocketNotifier::Read, this);
        connect(sn, &QSocketNotifier::activated, this, &PlatformThemePlugin::heavenEvent);
        m_core->dispatch(0);
    });
    m_core->dispatch(0);

    // Obtain the compositor-assigned private handle (Wayland) and forward it to Heaven. Deferred to
    // the event loop because QtWayland has not connected its wl_display yet at plugin-creation time.
    QTimer::singleShot(0, this, [heaven = m_heaven]{ setupPrivateHandle(heaven); });

    return new PlatformTheme(m_heaven);
}

void PlatformThemePlugin::appNameChanged()
{
    if (!m_heaven)
        return;

    m_heaven->setName(QGuiApplication::applicationName().toStdString());
    m_heaven->commit();
}

void PlatformThemePlugin::heavenEvent()
{
    if (m_core)
        m_core->dispatch(0);
}

QT_END_NAMESPACE
