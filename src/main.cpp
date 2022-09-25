#include <qpa/qplatformthemeplugin.h>
#include <QGuiApplication>
#include "PlatformTheme.h"
#include <QDebug>
#include <QSocketNotifier>
#include <HNMenu.h>
#include <HNMenuItem.h>

QT_BEGIN_NAMESPACE

static void disconnected_from_server(hn_client *)
{
}

static void object_destroy(hn_object *)
{

}

static void server_action_invoke(hn_action *)
{

}

static void server_toggle_set_checked(hn_toggle *, hn_bool)
{

}

static void server_option_set_active(hn_option *, hn_select *)
{

}

static void server_send_custom_event(hn_client *, void *, u_int32_t size)
{

}

hn_client_events_interface heavenClientInterface
{
    &disconnected_from_server,
    &object_destroy,
    &server_action_invoke,
    &server_toggle_set_checked,
    &server_option_set_active,
    &server_send_custom_event
};

class PlatformThemePlugin : public QPlatformThemePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QPlatformThemeFactoryInterface_iid FILE "cuarzo-platformtheme.json")

public:
    PlatformThemePlugin(QObject *parent = nullptr) : QPlatformThemePlugin(parent)
    {
    }

    QPlatformTheme *create(const QString &key, const QStringList &paramList) override
    {
        Q_UNUSED(key)
        Q_UNUSED(paramList)

        QString appName = "Qt Application";

        if(QGuiApplication::applicationName().length() != 0)
            appName = QGuiApplication::applicationName();

        heaven = hn_client_create(NULL, appName.toUtf8(), NULL, &heavenClientInterface);

        if(heaven)
        {
            connect(QGuiApplication::instance(), &QCoreApplication::applicationNameChanged, this, &PlatformThemePlugin::appNameChanged);

            QSocketNotifier *sn = new QSocketNotifier(hn_client_get_fd(heaven), QSocketNotifier::Type::Read, this);
            sn->setEnabled(true);
            connect(sn, &QSocketNotifier::activated, this, &PlatformThemePlugin::heavenEvent);

            return new PlatformTheme(heaven);
        }


        return nullptr;
    }
private:
    hn_client *heaven = nullptr;

private slots:
    void appNameChanged()
    {
        hn_client_set_app_name(heaven, QGuiApplication::applicationName().toUtf8());
    }

    void heavenEvent()
    {
        hn_client_dispatch_events(heaven, 0);
    }
};

bool loopedMenu(HNMenu *menu, hn_object *object)
{
    if(!object)
    {
        return false;
    }

    if(hn_object_get_user_data(object) == menu)
    {
        return true;
    }

    hn_object *parent = hn_object_get_parent(object);

    if(parent)
    {
        if(hn_object_get_user_data(parent) == menu)
        {
            return true;
        }
        return loopedMenu(menu, parent);
    }
    else
    {
        return false;
    }
}

void destroyObjectAndChildren(hn_object *object)
{
    const hn_array *children = hn_object_get_children(object);

    while(!hn_array_empty((hn_array*)children))
    {
        destroyObjectAndChildren(children->begin->data);

        hn_object_destroy(children->begin->data);
    }

    if(hn_object_get_type(object) == HN_OBJECT_TYPE_MENU)
    {
        HNMenu *m = (HNMenu*)hn_object_get_user_data(object);

        for(MenuClone *mClone : qAsConst(m->clones))
        {
            if(mClone->object == object)
            {
                m->clones.removeOne(mClone);
                delete mClone;
                break;
            }
        }
    }
    else
    {
        HNMenuItem *i = (HNMenuItem*)hn_object_get_user_data(object);

        for(ItemClone *iClone : qAsConst(i->clones))
        {
            if(iClone->object == object)
            {
                i->clones.removeOne(iClone);
                delete iClone;
                break;
            }
        }
    }
    hn_object_destroy(object);
}

QT_END_NAMESPACE

#include "main.moc"
