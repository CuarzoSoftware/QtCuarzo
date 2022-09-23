#include "HNMenuItem.h"
#include "HNMenu.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

HNMenuItem::HNMenuItem(HNData *data) : QPlatformMenuItem() , m_data(data)
{
    qDebug() << "HNMenuItem NEW" << this;
    m_object = hn_action_create(m_data->heaven, this);
    m_ref.object = this;
}

HNMenuItem::~HNMenuItem()
{
    qDebug() << "Destroy item" << this;
    hn_object_destroy(object());
}

void HNMenuItem::setText(const QString &text)
{
    m_text = text;
    m_text = m_text.remove("&");

    switch(hn_object_get_type(m_object))
    {
        case HN_OBJECT_TYPE_MENU:
        {
            hn_menu_set_label((hn_menu*)m_object, m_text.toUtf8());
        }break;
        case HN_OBJECT_TYPE_ACTION:
        {
            hn_action_set_label((hn_action*)m_object, m_text.toUtf8());
        }break;
        case HN_OBJECT_TYPE_TOGGLE:
        {
            hn_toggle_set_label((hn_toggle*)m_object, m_text.toUtf8());
        }break;
        case HN_OBJECT_TYPE_SEPARATOR:
        {
            hn_separator_set_label((hn_separator*)m_object, m_text.toUtf8());
        }break;
    }
}

void HNMenuItem::setIcon(const QIcon &icon)
{
    m_icon = icon;

    u_int32_t width = 0, height = 0;
    hn_pixel *pixels = qIconToHNPixels(icon, &width, &height);

    switch(hn_object_get_type(m_object))
    {
        case HN_OBJECT_TYPE_MENU:
        {
            hn_menu_set_icon((hn_menu*)m_object, pixels, width, height);
        }break;
        case HN_OBJECT_TYPE_ACTION:
        {
            hn_action_set_icon((hn_action*)m_object, pixels, width, height);
        }break;
    }

    if(pixels)
        delete []pixels;
}

void HNMenuItem::setMenu(QPlatformMenu *menu)
{
    qDebug() << "-----> HNMenuItem::setMenu" << menu;

    if(menu)
    {
        hn_object *parent = hn_object_get_parent(object());
        const hn_node *node;
        hn_object *before = NULL;

        if(parent)
        {
            node = hn_object_get_parent_link(object());

            if(node && node->next)
                before = node->next->data;
        }

        hn_object_remove_fom_parent(object());

        this->menu = (HNMenu*)menu;
        this->menu->action = this;

        hn_menu_set_label((hn_menu*)this->menu->object(), m_text.toUtf8());
        u_int32_t width = 0, height = 0;
        hn_pixel *pixels = qIconToHNPixels(m_icon, &width, &height);
        hn_menu_set_icon((hn_menu*)this->menu->object(), pixels, width, height);

        if(pixels)
            delete []pixels;

        if(parent)
            hn_menu_add_to_menu((hn_menu*)this->menu->object(), (hn_menu*)parent, before);
    }
    else
    {
        if(this->menu)
        {
            hn_object *parent = hn_object_get_parent(this->menu->object());
            const hn_node *node;
            hn_object *before = NULL;

            if(parent)
            {
                node = hn_object_get_parent_link(this->menu->object());

                if(node && node->next)
                    before = node->next->data;
            }

            hn_object_remove_fom_parent(this->menu->object());

            this->menu->action = nullptr;
            this->menu = nullptr;

            hn_object_destroy(object());
            m_object = hn_action_create(m_data->heaven, this);

            hn_action_set_label((hn_action*)object(), m_text.toUtf8());
            hn_action_set_enabled((hn_action*)object(), (hn_bool)m_enabled);

            u_int32_t width = 0, height = 0;
            hn_pixel *pixels = qIconToHNPixels(m_icon, &width, &height);
            hn_action_set_icon((hn_action*)object(), pixels, width, height);

            if(pixels)
                delete []pixels;

            if(parent)
                hn_action_add_to_menu((hn_action*)object(), (hn_menu*)parent, before);

        }
    }
}

void HNMenuItem::setVisible(bool isVisible)
{
    m_visible = isVisible;
    qDebug() << "HNMenuItem::setVisible" << isVisible;
}

void HNMenuItem::setIsSeparator(bool isSeparator)
{
    qDebug() << "HNMenuItem::setIsSeparator" << isSeparator;

    if(isSeparator)
    {
        if(hn_object_get_type(object()) == HN_OBJECT_TYPE_SEPARATOR)
            return;

        hn_object *parent = hn_object_get_parent(object());
        const hn_node *node;
        hn_object *before = NULL;

        if(parent)
        {
            node = hn_object_get_parent_link(object());

            if(node && node->next)
                before = node->next->data;
        }

        hn_object_destroy(object());
        m_object = hn_separator_create(m_data->heaven, this);
        hn_separator_set_label((hn_separator*)object(), m_text.toUtf8());

        if(parent)
            hn_separator_add_to_menu((hn_separator*)object(), (hn_menu*)parent, before);
    }
    else
    {
        if(hn_object_get_type(object()) != HN_OBJECT_TYPE_SEPARATOR)
            return;

        hn_object *parent = hn_object_get_parent(object());
        const hn_node *node;
        hn_object *before = NULL;

        if(parent)
        {
            node = hn_object_get_parent_link(object());

            if(node && node->next)
                before = node->next->data;
        }

        hn_object_destroy(object());
        m_object = hn_action_create(m_data->heaven, this);

        hn_action_set_label((hn_action*)object(), m_text.toUtf8());
        hn_action_set_enabled((hn_action*)object(), (hn_bool)m_enabled);

        u_int32_t width = 0, height = 0;
        hn_pixel *pixels = qIconToHNPixels(m_icon, &width, &height);
        hn_action_set_icon((hn_action*)object(), pixels, width, height);

        if(pixels)
            delete []pixels;

        if(parent)
            hn_action_add_to_menu((hn_action*)object(), (hn_menu*)parent, before);
    }
}

void HNMenuItem::setRole(MenuRole role) {}

#if QT_CONFIG(shortcut)
void HNMenuItem::setShortcut(const QKeySequence& shortcut)
{
    m_shortcut = shortcut.toString();
    m_shortcut = m_shortcut.remove(" ");

    switch(hn_object_get_type(m_object))
    {
        case HN_OBJECT_TYPE_ACTION:
        {
            if(m_shortcut.isEmpty())
                hn_action_set_shortcuts((hn_action*)m_object, NULL);
            else
                hn_action_set_shortcuts((hn_action*)m_object, m_shortcut.toUtf8());
        }break;
        case HN_OBJECT_TYPE_TOGGLE:
        {
            if(m_shortcut.isEmpty())
                hn_toggle_set_shortcuts((hn_toggle*)m_object, NULL);
            else
                hn_toggle_set_shortcuts((hn_toggle*)m_object, m_shortcut.toUtf8());
        }break;
    }

}
#endif

void HNMenuItem::setCheckable(bool checkable)
{
    if(checkable)
    {
        if(hn_object_get_type(object()) == HN_OBJECT_TYPE_TOGGLE)
            return;

        hn_object *parent = hn_object_get_parent(object());
        const hn_node *node;
        hn_object *before = NULL;

        if(parent)
        {
            node = hn_object_get_parent_link(object());

            if(node && node->next)
                before = node->next->data;
        }

        hn_object_destroy(object());

        m_object = hn_toggle_create(m_data->heaven, this);

        hn_toggle_set_checked((hn_toggle*)object(), (hn_bool)m_checked);
        hn_toggle_set_label((hn_toggle*)object(), m_text.toUtf8());
        hn_toggle_set_enabled((hn_toggle*)object(), (hn_bool)m_enabled);

        if(parent)
            hn_toggle_add_to_menu((hn_toggle*)object(), (hn_menu*)parent, before);
    }
    else
    {
        if(hn_object_get_type(object()) != HN_OBJECT_TYPE_TOGGLE)
            return;

        hn_object *parent = hn_object_get_parent(object());
        const hn_node *node;
        hn_object *before = NULL;

        if(parent)
        {
            node = hn_object_get_parent_link(object());

            if(node && node->next)
                before = node->next->data;
        }

        hn_object_destroy(object());
        m_object = hn_action_create(m_data->heaven, this);

        hn_action_set_label((hn_action*)object(), m_text.toUtf8());
        hn_action_set_enabled((hn_action*)object(), (hn_bool)m_enabled);

        u_int32_t width = 0, height = 0;
        hn_pixel *pixels = qIconToHNPixels(m_icon, &width, &height);
        hn_action_set_icon((hn_action*)object(), pixels, width, height);

        if(pixels)
            delete []pixels;

        if(parent)
            hn_action_add_to_menu((hn_action*)object(), (hn_menu*)parent, before);
    }
}

void HNMenuItem::setChecked(bool isChecked)
{
    m_checked = isChecked;

    if(hn_object_get_type(object()) != HN_OBJECT_TYPE_TOGGLE)
        return;

    hn_toggle_set_checked((hn_toggle*)object(), (hn_bool)isChecked);
}

void HNMenuItem::setEnabled(bool enabled)
{
    m_enabled = enabled;

    if(menu)
    {
        menu->setEnabled(enabled);
        return;
    }

    switch(hn_object_get_type(m_object))
    {
        case HN_OBJECT_TYPE_ACTION:
        {
            hn_action_set_enabled((hn_action*)m_object, (hn_bool)enabled);
        }break;
        case HN_OBJECT_TYPE_TOGGLE:
        {
            hn_toggle_set_enabled((hn_toggle*)m_object, (hn_bool)enabled);
        }break;
    }
}

void HNMenuItem::setIconSize(int size) {}

void HNMenuItem::setTag(quintptr tag)
{
    m_ref.tag = tag;
}

quintptr HNMenuItem::tag() const
{
    return m_ref.tag;
}

void HNMenuItem::restoreBackup()
{
    /*
    if(m_backup)
    {
        switch(hn_object_get_type(m_backup))
        {
            case HN_OBJECT_TYPE_MENU:
            {
                hn_menu_set_label((hn_menu*)m_object, m_text.toUtf8());
            }break;
            case HN_OBJECT_TYPE_ACTION:
            {
                hn_object *parent = hn_object_get_parent(object());
                const hn_node *node = hn_object_get_parent_link(object());
                hn_object *before = NULL;

                if(node && node->next)
                    before = node->next->data;

                if(hn_object_get_type(object()) != HN_OBJECT_TYPE_MENU)
                    hn_object_destroy(object());

                m_object = m_backup;
                m_backup = nullptr;

                hn_action_set_label((hn_action*)object(), m_text.toUtf8());
                hn_action_set_enabled((hn_action*)object(), (hn_bool)m_enabled);

                u_int32_t width = 0, height = 0;
                hn_pixel *pixels = qIconToHNPixels(m_icon, &width, &height);
                hn_action_set_icon((hn_action*)object(), pixels, width, height);

                if(pixels)
                    delete []pixels;

                hn_action_add_to_menu((hn_action*)object(), (hn_menu*)parent, before);
            }break;
            case HN_OBJECT_TYPE_TOGGLE:
            {
                hn_toggle_set_label((hn_toggle*)m_object, m_text.toUtf8());
            }break;
            case HN_OBJECT_TYPE_SEPARATOR:
            {
                hn_separator_set_label((hn_separator*)m_object, m_text.toUtf8());
            }break;
        }

    }
    */
}

hn_object *HNMenuItem::object() const
{
    return m_object;
}

HNObjectRef &HNMenuItem::ref()
{
    return m_ref;
}

QT_END_NAMESPACE
