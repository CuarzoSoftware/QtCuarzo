#include "HNMenu.h"
#include "HNMenuItem.h"
#include "PlatformTheme.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

HNMenu::HNMenu(HNData *data) : QPlatformMenu(), m_data(data)
{
    qDebug() << "HNMenu::createdMenu" << this;
    m_object = hn_menu_create(heaven(), this);
    m_ref.object = this;
}

HNMenu::~HNMenu()
{
    qDebug() << "Destroy menu" << this;

    if(action)
        action->menu = nullptr;

    hn_object_destroy(object());
}

void HNMenu::insertMenuItem(QPlatformMenuItem *menuItem, QPlatformMenuItem *before)
{
    qDebug() << "HNMenu::insertMenuItem" << menuItem << before << this;

    if(menuItem)
    {
        HNMenuItem *item = (HNMenuItem*)menuItem;
        HNMenuItem *beforeItem = (HNMenuItem*)before;

        switch(hn_object_get_type(item->object()))
        {
            case HN_OBJECT_TYPE_MENU:
            {
                if(before)
                    hn_menu_add_to_menu((hn_menu*)item->object(), (hn_menu*)object(), beforeItem->object());
                else
                    hn_menu_add_to_menu((hn_menu*)item->object(), (hn_menu*)object(), NULL);
            }break;
            case HN_OBJECT_TYPE_ACTION:
            {
                if(before)
                    hn_action_add_to_menu((hn_action*)item->object(), (hn_menu*)object(), beforeItem->object());
                else
                    hn_action_add_to_menu((hn_action*)item->object(), (hn_menu*)object(), NULL);
            }break;
            case HN_OBJECT_TYPE_TOGGLE:
            {
                if(before)
                    hn_toggle_add_to_menu((hn_toggle*)item->object(), (hn_menu*)object(), beforeItem->object());
                else
                    hn_toggle_add_to_menu((hn_toggle*)item->object(), (hn_menu*)object(), NULL);
            }break;
            case HN_OBJECT_TYPE_SEPARATOR:
            {
                if(before)
                    hn_separator_add_to_menu((hn_separator*)item->object(), (hn_menu*)object(), beforeItem->object());
                else
                    hn_separator_add_to_menu((hn_separator*)item->object(), (hn_menu*)object(), NULL);
            }break;
        }
    }
}

void HNMenu::removeMenuItem(QPlatformMenuItem *menuItem)
{
    qDebug() << "HNMenu::removeMenuItem" << menuItem << this;

    if(menuItem)
    {
        HNMenuItem *item = (HNMenuItem*)menuItem;
        hn_object_remove_fom_parent(item->object());
    }
}

void HNMenu::syncMenuItem(QPlatformMenuItem *menuItem)
{
    qDebug() << "HNMenu::syncMenuItem" << menuItem << this;
}

void HNMenu::syncSeparatorsCollapsible(bool enable)
{
    qDebug() << "HNMenu::syncSeparatorsCollapsible" << enable << this;
}

void HNMenu::setText(const QString &text)
{
    qDebug() << "HNMenu::setText" << text << this;
    QString a = text;
    a = a.remove("&");
    hn_menu_set_label((hn_menu*)object(), a.toUtf8());
}

void HNMenu::setIcon(const QIcon &icon)
{
    qDebug() << "HNMenu::setIcon" << icon << this;

    u_int32_t width = 0, height = 0;
    hn_pixel *pixels = qIconToHNPixels(icon, &width, &height);

    hn_menu_set_icon((hn_menu*)object(), pixels, width, height);

    if(pixels)
        delete []pixels;
}

void HNMenu::setEnabled(bool enabled)
{
    qDebug() << "HNMenu::setEnabed" << enabled << this;

    hn_menu_set_enabled((hn_menu*)object(), (hn_bool)enabled);
}

void HNMenu::setVisible(bool visible)
{
    qDebug() << "HNMenu::setVisible" << visible << this;
}

QPlatformMenuItem *HNMenu::menuItemAt(int position) const
{
    qDebug() << "HNMenu::menuItemAt" << position << this;

    int i = 0;

    hn_node *node = hn_object_get_children(m_object)->begin;

    while(node)
    {
        HNMenuItem *menuItem = (HNMenuItem*)hn_object_get_user_data(node->data);

        if(i == position)
            return menuItem;

        node = node->next;
        i++;
    }

    return nullptr;
}

QPlatformMenuItem *HNMenu::menuItemForTag(quintptr tag) const
{
    qDebug() << "HNMenu::menuItemForTag" << tag << this;

    hn_node *node = hn_object_get_children(m_object)->begin;

    while(node)
    {

        if(hn_object_get_type((hn_object*)node->data) == HN_OBJECT_TYPE_MENU)
        {
            /*
            HNMenu *menu = (HNMenu*)hn_object_get_user_data((hn_object*)node->data);

            qDebug() << menu;

            if(menu->tag() == tag)
                return (QPlatformMenuItem*)menu;
                */
        }
        else
        {
            HNMenuItem *menuItem = (HNMenuItem*)hn_object_get_user_data((hn_object*)node->data);

            qDebug() << menuItem;

            if(menuItem->tag() == tag)
                return menuItem;
        }


        node = node->next;
    }

    return nullptr;
}

QPlatformMenuItem *HNMenu::createMenuItem() const
{
    qDebug() << "HNMenu::menuItem" << this;

    HNMenuItem *menuItem = new HNMenuItem(m_data);

    hn_action_add_to_menu((hn_action*)menuItem->object(), (hn_menu*)object(), NULL);

    return menuItem;
}

QPlatformMenu *HNMenu::createSubMenu() const
{
    qDebug() << "HNMenu::createSubMenu" << this;

    HNMenu *subMenu = new HNMenu(m_data);

    hn_menu_add_to_menu((hn_menu*)subMenu->object(), (hn_menu*)object(), NULL);

    return subMenu;
}

void HNMenu::setTag(quintptr tag)
{
    qDebug() << "HNMenu::setTag" << this;
    m_ref.tag = tag;
}

quintptr HNMenu::tag() const
{
    return m_ref.tag;
}

hn_client *HNMenu::heaven() const
{
    return m_data->heaven;
}

hn_object *HNMenu::object() const
{
    return m_object;
}

HNObjectRef &HNMenu::ref()
{
    return m_ref;
}

QT_END_NAMESPACE
