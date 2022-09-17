#include "HNMenuItem.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

HNMenuItem::HNMenuItem(HNData *data) : QPlatformMenuItem() , m_data(data)
{
    qDebug() << "HNMenuItem NEW" << this;
    m_object = hn_action_create(m_data->heaven, this);
    setTag(hn_object_get_id(m_object));
}

HNMenuItem::~HNMenuItem()
{
    qDebug() << "Destroy item" << this;
}

void HNMenuItem::setText(const QString &text)
{
    qDebug() << "HNMenuItem::setText" << text;

    m_text = text;

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
    qDebug() << "HNMenuItem::setIcon" << icon;

    m_icon = icon;

    hn_pixel *pixels = NULL;
    u_int32_t width = 0;
    u_int32_t height = 0;

    QImage img = icon.pixmap(icon.actualSize(QSize(64, 64))).toImage();
    int totalPixels = img.width()*img.height();

    if(totalPixels > 0)
    {
        width = img.width();
        height = img.height();
        pixels = new hn_pixel[totalPixels];
        u_int32_t i = 0;

        for(u_int32_t y = 0; y < height; y++)
        {
            for(u_int32_t x = 0; y < width; x++)
            {
                pixels[i] = (hn_pixel)qAlpha(img.pixel(x,y));
                i++;
            }
        }
    }

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
    qDebug() << "HNMenuItem::setMenu" << menu;
}

void HNMenuItem::setVisible(bool isVisible)
{
    qDebug() << "HNMenuItem::setVisible" << isVisible;
}

void HNMenuItem::setIsSeparator(bool isSeparator)
{
    qDebug() << "HNMenuItem::setIsSeparator" << isSeparator;
}

void HNMenuItem::setRole(MenuRole role)
{
    qDebug() << "HNMenuItem::setRoler" << role;
}

void HNMenuItem::setCheckable(bool checkable)
{
    qDebug() << "HNMenuItem::setCheckable" << checkable;
}

void HNMenuItem::setChecked(bool isChecked)
{
    qDebug() << "HNMenuItem::setChecked" << isChecked;
}

void HNMenuItem::setEnabled(bool enabled)
{
    qDebug() << "HNMenuItem::setEnabled" << enabled;
}

void HNMenuItem::setIconSize(int size)
{
    qDebug() << "HNMenuItem::setIconSize" << size;
}

QT_END_NAMESPACE
