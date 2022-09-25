#include "HNMenuItem.h"
#include "HNMenu.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

HNMenuItem::HNMenuItem(HNData *data) : QPlatformMenuItem() , m_data(data)
{
}

HNMenuItem::~HNMenuItem()
{
    if(menu)
    {
        menu->action = nullptr;
    }

    while(!clones.empty())
    {
        ItemClone *iClone = clones.last();
        hn_object_destroy(iClone->object);
        clones.removeLast();
        delete iClone;
    }
}

void HNMenuItem::setText(const QString &text)
{
    m_text = text;
    m_text = m_text.remove("&");

    for(ItemClone *iClone : qAsConst(clones))
    {
        if(iClone->subMenu)
        {
            hn_menu_set_label((hn_menu*)iClone->subMenu->object, m_text.toUtf8());
            continue;
        }

        switch(hn_object_get_type(iClone->object))
        {
            case HN_OBJECT_TYPE_ACTION:
            {
                hn_action_set_label((hn_action*)iClone->object, m_text.toUtf8());
            }break;
            case HN_OBJECT_TYPE_TOGGLE:
            {
                hn_toggle_set_label((hn_toggle*)iClone->object, m_text.toUtf8());
            }break;
            case HN_OBJECT_TYPE_SEPARATOR:
            {
                hn_separator_set_label((hn_separator*)iClone->object, m_text.toUtf8());
            }break;
        }
    }
}

void HNMenuItem::setIcon(const QIcon &icon)
{
    hn_pixel *pixels = qIconToHNPixels(icon, &w, &h);

    if(this->pixels)
        delete []this->pixels;

    this->pixels = pixels;

    for(ItemClone *iClone : qAsConst(clones))
    {
        if(menu)
        {
            hn_menu_set_icon((hn_menu*)iClone->menu->object, pixels, w, h);
        }

        if(hn_object_get_type(iClone->object) == HN_OBJECT_TYPE_ACTION)
        {
            hn_action_set_icon((hn_action*)iClone->object, pixels, w, h);
        }
    }
}


void HNMenuItem::setMenu(QPlatformMenu *mm)
{
    HNMenu *m = (HNMenu*)mm;

    for(ItemClone *iClone : qAsConst(clones))
    {
        if(m)
        {
            hn_object *parent = hn_object_get_parent(iClone->object);
            const hn_node *node;
            hn_object *before = NULL;

            if(parent)
            {
                node = hn_object_get_parent_link(iClone->object);

                if(node && node->next)
                    before = node->next->data;
            }

            hn_object_remove_fom_parent(iClone->object);

            iClone->subMenu = m->createClone(parent);

            if(parent)
                hn_menu_add_to_menu((hn_menu*)iClone->subMenu->object, (hn_menu*)parent, before);

            hn_menu_set_label((hn_menu*)iClone->subMenu->object, m_text.toUtf8());
            hn_menu_set_icon((hn_menu*)iClone->subMenu->object, pixels, w, h);
        }
        else
        {
            if(this->menu)
            {
                hn_object *parent = hn_object_get_parent(iClone->subMenu->object);
                const hn_node *node;
                hn_object *before = NULL;

                if(parent)
                {
                    node = hn_object_get_parent_link(iClone->subMenu->object);

                    if(node && node->next)
                        before = node->next->data;
                }

                // Restore menu
                destroyObjectAndChildren(iClone->subMenu->object);

                switch(hn_object_get_type(iClone->object))
                {
                    case HN_OBJECT_TYPE_ACTION:
                        hn_action_add_to_menu((hn_action*)iClone->object,(hn_menu*)parent,before);
                        break;
                    case HN_OBJECT_TYPE_SEPARATOR:
                        hn_separator_add_to_menu((hn_separator*)iClone->object,(hn_menu*)parent,before);
                        break;
                    case HN_OBJECT_TYPE_TOGGLE:
                        hn_toggle_add_to_menu((hn_toggle*)iClone->object,(hn_menu*)parent,before);
                        break;
                }

            }
        }
    }

    if(this->menu)
        this->menu->action = nullptr;

    this->menu = (HNMenu*)m;

    if(this->menu)
        this->menu->action = this;

}

void HNMenuItem::setVisible(bool isVisible)
{
    m_visible = isVisible;
}

void HNMenuItem::setIsSeparator(bool isSeparator)
{

    m_isSeparator = isSeparator;

    for(ItemClone *iClone : qAsConst(clones))
    {
        if(isSeparator)
        {
            if(hn_object_get_type(iClone->object) == HN_OBJECT_TYPE_SEPARATOR)
                continue;

            hn_object *parent = hn_object_get_parent(iClone->object);
            const hn_node *node;
            hn_object *before = NULL;

            if(parent)
            {
                node = hn_object_get_parent_link(iClone->object);

                if(node && node->next)
                    before = node->next->data;
            }

            hn_object_destroy(iClone->object);
            iClone->object = hn_separator_create(m_data->heaven, this);
            hn_separator_set_label((hn_separator*)iClone->object, m_text.toUtf8());

            if(parent)
                hn_separator_add_to_menu((hn_separator*)iClone->object, (hn_menu*)parent, before);
        }
        else
        {
            if(hn_object_get_type(iClone->object) != HN_OBJECT_TYPE_SEPARATOR)
                continue;

            hn_object *parent = hn_object_get_parent(iClone->object);
            const hn_node *node;
            hn_object *before = NULL;

            if(parent)
            {
                node = hn_object_get_parent_link(iClone->object);

                if(node && node->next)
                    before = node->next->data;
            }

            hn_object_destroy(iClone->object);
            iClone->object = hn_action_create(m_data->heaven, this);

            hn_action_set_label((hn_action*)iClone->object, m_text.toUtf8());
            hn_action_set_enabled((hn_action*)iClone->object, (hn_bool)m_enabled);
            hn_action_set_icon((hn_action*)iClone->object, pixels, w, h);

            if(parent)
                hn_action_add_to_menu((hn_action*)iClone->object, (hn_menu*)parent, before);
        }
    }

}

void HNMenuItem::setRole(MenuRole role) {}

#if QT_CONFIG(shortcut)
void HNMenuItem::setShortcut(const QKeySequence& shortcut)
{
    m_shortcut = shortcut.toString();
    m_shortcut = m_shortcut.remove(" ");

    for(ItemClone *iClone : qAsConst(clones))
    {
        switch(hn_object_get_type(iClone->object))
        {
            case HN_OBJECT_TYPE_ACTION:
            {
                if(m_shortcut.isEmpty())
                    hn_action_set_shortcuts((hn_action*)iClone->object, NULL);
                else
                    hn_action_set_shortcuts((hn_action*)iClone->object, m_shortcut.toUtf8());
            }break;
            case HN_OBJECT_TYPE_TOGGLE:
            {
                if(m_shortcut.isEmpty())
                    hn_toggle_set_shortcuts((hn_toggle*)iClone->object, NULL);
                else
                    hn_toggle_set_shortcuts((hn_toggle*)iClone->object, m_shortcut.toUtf8());
            }break;
        }
    }
}
#endif

void HNMenuItem::setCheckable(bool checkable)
{

    m_isCheckable = checkable;


    for(ItemClone *iClone : qAsConst(clones))
    {
        if(checkable)
        {
            if(hn_object_get_type(iClone->object) == HN_OBJECT_TYPE_TOGGLE)
                return;

            hn_object *parent = hn_object_get_parent(iClone->object);
            const hn_node *node;
            hn_object *before = NULL;

            if(parent)
            {
                node = hn_object_get_parent_link(iClone->object);

                if(node && node->next)
                    before = node->next->data;
            }

            hn_object_destroy(iClone->object);

            iClone->object = hn_toggle_create(m_data->heaven, this);

            hn_toggle_set_checked((hn_toggle*)iClone->object, (hn_bool)m_checked);
            hn_toggle_set_label((hn_toggle*)iClone->object, m_text.toUtf8());
            hn_toggle_set_enabled((hn_toggle*)iClone->object, (hn_bool)m_enabled);

            if(parent)
                hn_toggle_add_to_menu((hn_toggle*)iClone->object, (hn_menu*)parent, before);
        }
        else
        {
            if(hn_object_get_type(iClone->object) != HN_OBJECT_TYPE_TOGGLE)
                return;

            hn_object *parent = hn_object_get_parent(iClone->object);
            const hn_node *node;
            hn_object *before = NULL;

            if(parent)
            {
                node = hn_object_get_parent_link(iClone->object);

                if(node && node->next)
                    before = node->next->data;
            }

            hn_object_destroy(iClone->object);
            iClone->object = hn_action_create(m_data->heaven, this);

            hn_action_set_label((hn_action*)iClone->object, m_text.toUtf8());
            hn_action_set_enabled((hn_action*)iClone->object, (hn_bool)m_enabled);
            hn_action_set_icon((hn_action*)iClone->object, pixels, w, h);

            if(parent)
                hn_action_add_to_menu((hn_action*)iClone->object, (hn_menu*)parent, before);
        }
    }
}

void HNMenuItem::setChecked(bool isChecked)
{

    m_checked = isChecked;

    for(ItemClone *iClone : qAsConst(clones))
    {
        if(hn_object_get_type(iClone->object) == HN_OBJECT_TYPE_TOGGLE)
            hn_toggle_set_checked((hn_toggle*)iClone->object, (hn_bool)isChecked);
    }
}

void HNMenuItem::setEnabled(bool enabled)
{
    m_enabled = enabled;

    if(menu)
    {
        menu->setEnabled(enabled);
    }

    for(ItemClone *iClone : qAsConst(clones))
    {

        switch(hn_object_get_type(iClone->object))
        {
            case HN_OBJECT_TYPE_ACTION:
            {
                hn_action_set_enabled((hn_action*)iClone->object, (hn_bool)enabled);
            }break;
            case HN_OBJECT_TYPE_TOGGLE:
            {
                hn_toggle_set_enabled((hn_toggle*)iClone->object, (hn_bool)enabled);
            }break;
        }
    }

}

void HNMenuItem::setIconSize(int size) {}

void HNMenuItem::setTag(quintptr tag)
{
    m_tag = tag;
}

quintptr HNMenuItem::tag() const
{
    return m_tag;
}

ItemClone *HNMenuItem::createClone()
{
    ItemClone *clone = new ItemClone;

    if(m_isSeparator)
    {
        clone->object = hn_separator_create(heaven(), this);
        hn_separator_set_label((hn_separator*)clone->object, m_text.toUtf8());
    }
    else if(m_isCheckable)
    {
        clone->object = hn_toggle_create(heaven(), this);
        hn_toggle_set_label((hn_toggle*)clone->object, m_text.toUtf8());
        hn_toggle_set_enabled((hn_toggle*)clone->object, m_enabled);
    }
    else
    {
        clone->object = hn_action_create(heaven(), this);
        hn_action_set_label((hn_action*)clone->object, m_text.toUtf8());
        hn_action_set_enabled((hn_action*)clone->object, m_enabled);
        hn_action_set_icon((hn_action*)clone->object, pixels, w, h);
    }

    clones.append(clone);
    return clone;
}

hn_client *HNMenuItem::heaven() const
{
    return m_data->heaven;
}



QT_END_NAMESPACE
