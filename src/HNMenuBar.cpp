#include "HNMenuBar.h"
#include "HNMenu.h"

QT_BEGIN_NAMESPACE

HNMenuBar::HNMenuBar(HNData *data) : QPlatformMenuBar(), m_data(data)
{
    m_object = HNTopbar::Make();

    if (m_object)
        m_object->userData = this;
}

static MenuClone *findMenuClone(HNMenu *menu, HNMenuBar *bar)
{
    for (MenuClone *clone : menu->clones)
        if (clone->bar == bar)
            return clone;

    return nullptr;
}

HNMenuBar::~HNMenuBar()
{
    for (MenuClone *child : children)
    {
        HNMenu *menu = static_cast<HNMenu*>(child->object->userData);
        menu->clones.removeOne(child);
        child->object.reset(); // destroys the Heaven menu
        delete child;
    }

    m_object.reset(); // destroys the Heaven topbar
    m_data->commit();
}

void HNMenuBar::insertMenu(QPlatformMenu *menu, QPlatformMenu *before)
{
    HNMenu *m = static_cast<HNMenu*>(menu);
    HNMenu *b = static_cast<HNMenu*>(before);

    MenuClone *clone = findMenuClone(m, this);

    if (clone)
    {
        children.removeOne(clone);
    }
    else
    {
        clone = m->createClone(object());
        clone->bar = this;
    }

    if (before)
    {
        MenuClone *beforeClone = findMenuClone(b, this);
        children.insert(children.indexOf(beforeClone), clone);
        clone->object->insertBefore(beforeClone->object.get());
    }
    else
    {
        children.append(clone);
        clone->object->setParent(object());
    }

    m_data->commit();
}

void HNMenuBar::removeMenu(QPlatformMenu *menu)
{
    HNMenu *m = static_cast<HNMenu*>(menu);
    MenuClone *clone = findMenuClone(m, this);

    if (!clone)
        return;

    children.removeOne(clone);
    m->clones.removeOne(clone);
    clone->object.reset();
    delete clone;

    m_data->commit();
}

void HNMenuBar::syncMenu(QPlatformMenu *)
{
}

void HNMenuBar::handleReparent(QWindow *newParentWindow)
{
    if (m_window)
        disconnect(m_window, &QWindow::activeChanged, this, &HNMenuBar::windowActivatedChanged);

    m_window = newParentWindow;

    if (m_window)
    {
        connect(m_window, &QWindow::activeChanged, this, &HNMenuBar::windowActivatedChanged);

        if (m_window->isActive())
            windowActivatedChanged();
    }
}

QPlatformMenu *HNMenuBar::menuForTag(quintptr tag) const
{
    for (MenuClone *child : children)
    {
        HNMenu *menu = static_cast<HNMenu*>(child->object->userData);
        if (menu->tag() == tag)
            return menu;
    }

    return nullptr;
}

QPlatformMenu *HNMenuBar::createMenu() const
{
    return new HNMenu(m_data);
}

void HNMenuBar::windowActivatedChanged()
{
    if (m_window && m_window->isActive() && m_object)
    {
        m_data->heaven->setActiveTopbar(m_object.get());
        m_data->commit();
    }
}

QT_END_NAMESPACE
