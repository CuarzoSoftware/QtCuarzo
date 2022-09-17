#ifndef HNMENU_H
#define HNMENU_H

#include <qpa/qplatformmenu.h>

#include "Common.h"

class PlatformTheme;
class QPlatformMenuItem;
class HNMenuItem;

class HNMenu : public QPlatformMenu
{
    Q_OBJECT

public:
    HNMenu(HNData *data);
    ~HNMenu() override;
    void insertMenuItem(QPlatformMenuItem *menuItem, QPlatformMenuItem *before) override;
    void removeMenuItem(QPlatformMenuItem *menuItem) override;
    void syncMenuItem(QPlatformMenuItem *menuItem) override;
    void syncSeparatorsCollapsible(bool enable) override;

    void setText(const QString &text) override;
    void setIcon(const QIcon &icon) override;
    void setEnabled(bool enabled) override;
    void setVisible(bool visible) override;

    void dismiss() override { } // Closes this and all its related menu popups

    QPlatformMenuItem *menuItemAt(int position) const override;
    QPlatformMenuItem *menuItemForTag(quintptr tag) const override;

    hn_client *heaven() const;
    hn_object *object() const;

private:
    mutable HNData *m_data;
    hn_object *m_object = nullptr;
};



#endif // HNMENU_H
