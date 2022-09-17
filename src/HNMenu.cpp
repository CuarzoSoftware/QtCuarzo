#include "HNMenu.h"
#include "PlatformTheme.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

HNMenu::HNMenu(HNData *data) : QPlatformMenu(), m_data(data)
{
    qDebug() << "HNMenu::createdMenu" << this;
    m_object = hn_menu_create(heaven(), this);
    setTag(hn_object_get_id(m_object));
}

HNMenu::~HNMenu()
{
    qDebug() << "Destroy menu" << this;
}

void HNMenu::insertMenuItem(QPlatformMenuItem *menuItem, QPlatformMenuItem *before)
{
    qDebug() << "HNMenu::insertMenuItem" << menuItem << before << this;
}

void HNMenu::removeMenuItem(QPlatformMenuItem *menuItem)
{
    qDebug() << "HNMenu::removeMenuItem" << menuItem << this;

    if(menuItem)
    {

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
}

void HNMenu::setIcon(const QIcon &icon)
{
    qDebug() << "HNMenu::setIcon" << icon << this;
}

void HNMenu::setEnabled(bool enabled)
{
    qDebug() << "HNMenu::setEnabed" << enabled << this;
}

void HNMenu::setVisible(bool visible)
{
    qDebug() << "HNMenu::setVisible" << visible << this;
}

QPlatformMenuItem *HNMenu::menuItemAt(int position) const
{
    qDebug() << "HNMenu::menuItemAt" << position << this;
    return nullptr;
}

QPlatformMenuItem *HNMenu::menuItemForTag(quintptr tag) const
{
    qDebug() << "HNMenu::menuItemForTag" << tag << this;
    return nullptr;
}

hn_client *HNMenu::heaven() const
{
    return m_data->heaven;
}

hn_object *HNMenu::object() const
{
    return m_object;
}

QT_END_NAMESPACE
