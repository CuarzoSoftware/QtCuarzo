#include "HNMenuBar.h"
#include "HNMenu.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

HNMenuBar::HNMenuBar(HNData *data) : QPlatformMenuBar(), m_data(data)
{
    m_object = hn_top_bar_create(m_data->heaven, this);
}

MenuClone *findMenuClone(HNMenu *menu, HNMenuBar *bar)
{
    for(MenuClone *clone : qAsConst(menu->clones))
    {
        if(clone->bar == bar)
            return clone;
    }

    return nullptr;
}

HNMenuBar::~HNMenuBar()
{
    for(MenuClone *child : qAsConst(children))
    {
        HNMenu *menu = (HNMenu*)hn_object_get_user_data(child->object);
        menu->clones.removeOne(child);
        hn_object_destroy(child->object);
        delete child;
    }

    hn_object_destroy(object());
}

void HNMenuBar::insertMenu(QPlatformMenu *menu, QPlatformMenu *before)
{
    HNMenu *m = (HNMenu*)menu;
    HNMenu *b = (HNMenu*)before;

    MenuClone *clone = findMenuClone(m, this);

    if(clone)
    {
        children.removeOne(clone);
    }
    else
    {
        clone = m->createClone(object());
        clone->bar = this;
    }

    if(before)
    {
        MenuClone *beforeClone = findMenuClone(b,this);

        children.insert(children.indexOf(beforeClone), clone);

        hn_menu_add_to_top_bar(clone->object, (hn_top_bar*)object(), beforeClone->object);
    }
    else
    {
        children.append(clone);
        hn_menu_add_to_top_bar(clone->object, (hn_top_bar*)object(), nullptr);
    }

}

void HNMenuBar::removeMenu(QPlatformMenu *menu)
{
    HNMenu *m = (HNMenu*)menu;
    MenuClone *clone = findMenuClone(m, this);
    children.removeOne(clone);
    m->clones.removeOne(clone);
    hn_object_destroy(clone->object);
    delete clone;
}

void HNMenuBar::syncMenu(QPlatformMenu *menuItem)
{
 }

void HNMenuBar::handleReparent(QWindow *newParentWindow)
{

    if(m_window)
        disconnect(m_window, &QWindow::activeChanged, this, &HNMenuBar::windowActivatedChanged);

    m_window = newParentWindow;

    if(m_window)
    {
        connect(m_window, &QWindow::activeChanged, this, &HNMenuBar::windowActivatedChanged);
        if(m_window->isActive())
            hn_top_bar_set_active((hn_top_bar*)m_object);
    }
}

QPlatformMenu *HNMenuBar::menuForTag(quintptr tag) const
{
    for(MenuClone *child : children)
    {
        HNMenu *menu = (HNMenu*)hn_object_get_user_data(child->object);
        if(menu->tag() == tag)
            return menu;
    }

    return nullptr;
}

QPlatformMenu *HNMenuBar::createMenu() const
{
    HNMenu *menu = new HNMenu(m_data);
    return menu;
}

hn_object *HNMenuBar::object() const
{
    return m_object;
}

void HNMenuBar::windowActivatedChanged()
{
    if(m_window->isActive())
    {
        hn_top_bar_set_active((hn_top_bar*)m_object);
    }
}


QT_END_NAMESPACE


