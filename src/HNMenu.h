#ifndef HNMENU_H
#define HNMENU_H

#include "Common.h"

class PlatformTheme;
class QPlatformMenuItem;
class HNMenuItem;
class HNMenuBar;


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
    QPlatformMenuItem *createMenuItem() const override;
    QPlatformMenu *createSubMenu() const override;

    void setTag(quintptr tag) override;
    quintptr tag()const override;

    MenuClone *createClone(hn_object *parent);

#if QT_VERSION < 0x060000

#endif

    hn_client *heaven() const;

    HNMenuItem *action = nullptr;

    QString m_text;
    bool m_visible = true;
    bool m_enabled = true;

    QList<HNMenuItem*>children;
    QList<MenuClone*>clones;
private:
    mutable HNData *m_data;
    quintptr m_tag;

};



#endif // HNMENU_H
