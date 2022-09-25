#include "HNMenu.h"
#include "HNMenuItem.h"
#include "PlatformTheme.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

HNMenu::HNMenu(HNData *data) : QPlatformMenu(), m_data(data)
{

}

ItemClone *findItemClone(HNMenuItem *item, MenuClone *menu)
{
    for(ItemClone *clone : qAsConst(item->clones))
    {
        if(clone->menu == menu)
            return clone;
    }

    return nullptr;
}

HNMenu::~HNMenu()
{
    if(action)
    {
        action->menu = nullptr;
    }

    while(!clones.empty())
    {
        MenuClone *mClone = clones.last();

        for(HNMenuItem *child : qAsConst(children))
        {
            ItemClone *iClone = findItemClone(child, mClone);

            if(iClone)
            {
                child->clones.removeOne(iClone);
                hn_object_destroy(iClone->object);
                delete iClone;
            }
        }

        hn_object_destroy(mClone->object);
        clones.removeLast();
        delete mClone;
    }
}

void HNMenu::insertMenuItem(QPlatformMenuItem *menuItem, QPlatformMenuItem *before)
{
    HNMenuItem *i = (HNMenuItem*)menuItem;
    HNMenuItem *b = (HNMenuItem*)before;

    children.removeOne(i);

    if(b)
        children.insert(children.indexOf(b),i);
    else
        children.append(i);


    for(MenuClone *mClone : qAsConst(clones))
    {

        if(loopedMenu(this, hn_object_get_parent(mClone->object)))
            continue;

        ItemClone *iClone = findItemClone(i, mClone);

        /* TODO CHECK IF ITEM HAS SUBMENUS */

        if(!iClone)
        {
            iClone = i->createClone();
            iClone->menu = mClone;

            if(i->menu)
            {
                iClone->subMenu = i->menu->createClone(mClone->object);
            }
        }

        hn_object *bObject = nullptr;

        if(b)
            bObject = findItemClone(b, mClone)->object;

        if(i->menu)
        {
            hn_menu_add_to_menu((hn_menu*)iClone->subMenu->object, (hn_menu*)mClone->object, bObject);
            continue;
        }

        switch(hn_object_get_type(iClone->object))
        {
            case HN_OBJECT_TYPE_ACTION:
            {
                hn_action_add_to_menu((hn_action*)iClone->object, (hn_menu*)mClone->object, bObject);
            }break;
            case HN_OBJECT_TYPE_TOGGLE:
            {
                hn_toggle_add_to_menu((hn_toggle*)iClone->object, (hn_menu*)mClone->object, bObject);
            }break;
            case HN_OBJECT_TYPE_SEPARATOR:
            {
                hn_separator_add_to_menu((hn_separator*)iClone->object, (hn_menu*)mClone->object, bObject);
            }break;
        }
    }
}

void HNMenu::removeMenuItem(QPlatformMenuItem *menuItem)
{
    HNMenuItem *item = (HNMenuItem*)menuItem;

    children.removeOne(item);

    for(MenuClone *mClone : qAsConst(clones))
    {
        /* TODO CHECK IF ITEM HAS SUBMENUS */
        ItemClone *iClone = findItemClone(item,mClone);
        item->clones.removeOne(iClone);
        hn_object_destroy(iClone->object);
        delete iClone;
    }
}

void HNMenu::syncMenuItem(QPlatformMenuItem *menuItem)
{
}

void HNMenu::syncSeparatorsCollapsible(bool enable)
{
}

void HNMenu::setText(const QString &text)
{
    m_text = text;
    m_text = m_text.remove("&").trimmed();

    if(m_text.isEmpty())
        return;

    for(MenuClone *clone : qAsConst(clones))
    {
        hn_menu_set_label(clone->object, m_text.toUtf8());
    }
}

void HNMenu::setIcon(const QIcon &){}

void HNMenu::setEnabled(bool enabled)
{
    m_enabled = enabled;

    if(action)
    {
        for(ItemClone *clone : qAsConst(action->clones))
        {
            switch(hn_object_get_type(clone->object))
            {
                case HN_OBJECT_TYPE_ACTION:
                {
                    hn_action_set_enabled((hn_action*)clone->object, (hn_bool)enabled);
                }break;
                case HN_OBJECT_TYPE_TOGGLE:
                {
                    hn_toggle_set_enabled((hn_toggle*)clone->object, (hn_bool)enabled);
                }break;
            }
        }
    }

    for(MenuClone *clone : qAsConst(clones))
    {
        hn_menu_set_enabled(clone->object, (hn_bool)enabled);
    }

}

void HNMenu::setVisible(bool visible)
{
    m_visible = visible;
}

QPlatformMenuItem *HNMenu::menuItemAt(int position) const
{
    if(position >= 0 && position < children.count())
    {
        return (QPlatformMenuItem*)children.at(position);
    }

    return nullptr;
}

QPlatformMenuItem *HNMenu::menuItemForTag(quintptr tag) const
{
    for(HNMenuItem *item : children)
    {
        if(item->tag() == tag)
            return item;
    }
    return nullptr;
}

QPlatformMenuItem *HNMenu::createMenuItem() const
{
    HNMenuItem *menuItem = new HNMenuItem(m_data);
    return menuItem;
}

QPlatformMenu *HNMenu::createSubMenu() const
{
    HNMenu *subMenu = new HNMenu(m_data);
    return subMenu;
}

void HNMenu::setTag(quintptr tag)
{
    m_tag = tag;
}

quintptr HNMenu::tag() const
{
    return m_tag;
}

MenuClone *HNMenu::createClone(hn_object *parent)
{
    MenuClone *clone = new MenuClone;
    clone->object = hn_menu_create(heaven(), this);

    if(action)
    {
        hn_menu_set_label(clone->object, action->m_text.toUtf8());
        hn_menu_set_enabled(clone->object, action->m_enabled);
    }
    else
    {
        hn_menu_set_label(clone->object, m_text.toUtf8());
        hn_menu_set_enabled(clone->object, m_enabled);
    }


    clones.append(clone);

    if(!loopedMenu(this, parent))
    {
        for(HNMenuItem *child : qAsConst(children))
        {
            MenuClone *mClone = nullptr;

            if(child->menu)
            {
                mClone = child->menu->createClone(clone->object);
                mClone->menu = this;
                hn_menu_add_to_menu((hn_menu*)mClone->object, (hn_menu*)clone->object, NULL);

                if(child->m_text.isEmpty())
                    hn_menu_set_label((hn_menu*)mClone->object, child->menu->m_text.toUtf8());
                else
                    hn_menu_set_label((hn_menu*)mClone->object, child->m_text.toUtf8());

                hn_menu_set_icon((hn_menu*)mClone->object, child->pixels, child->w, child->h);
            }


            ItemClone *iClone = child->createClone();
            iClone->menu = clone;
            iClone->subMenu = mClone;

            if(child->menu)
                continue;

            switch(hn_object_get_type(iClone->object))
            {
                case HN_OBJECT_TYPE_ACTION:
                {
                    hn_action_add_to_menu((hn_action*)iClone->object,(hn_menu*)clone->object, NULL);
                }break;
                case HN_OBJECT_TYPE_SEPARATOR:
                {
                    hn_separator_add_to_menu((hn_separator*)iClone->object,(hn_menu*)clone->object, NULL);
                }break;
                case HN_OBJECT_TYPE_TOGGLE:
                {
                    hn_toggle_add_to_menu((hn_toggle*)iClone->object,(hn_menu*)clone->object, NULL);
                }break;
            }
        }
    }

    return clone;
}

hn_client *HNMenu::heaven() const
{
    return m_data->heaven;
}


QT_END_NAMESPACE
