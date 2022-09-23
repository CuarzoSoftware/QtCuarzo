#ifndef HNMENUITEM_H
#define HNMENUITEM_H

#include "Common.h"

QT_BEGIN_NAMESPACE

class PlatformTheme;
class HNMenu;

class HNMenuItem : public QPlatformMenuItem
{
    Q_OBJECT

public:
    HNMenuItem(HNData *data);
    ~HNMenuItem() override;
    void setText(const QString &text) override;
    void setIcon(const QIcon &icon) override;
    void setMenu(QPlatformMenu *menu) override;
    void setVisible(bool isVisible) override;
    void setIsSeparator(bool isSeparator) override;
    void setFont(const QFont &font) override {};
    void setRole(MenuRole role) override;
    void setCheckable(bool checkable) override;
    void setChecked(bool isChecked) override;
#if QT_CONFIG(shortcut)
    void setShortcut(const QKeySequence& shortcut) override;
#endif
    void setEnabled(bool enabled) override;
    void setIconSize(int size) override;

    void setTag(quintptr tag) override;
    quintptr tag()const override;

    void restoreBackup();

    hn_client *heaven() const;
    hn_object *object() const;
    HNObjectRef &ref();

    HNMenu *menu = nullptr;

private:
    mutable HNData *m_data;
    HNObjectRef m_ref;
    hn_object *m_object = nullptr;

    QString m_text;
    QString m_shortcut;
    QIcon m_icon;
    bool m_visible = true;
    bool m_isSeparator = false;
    bool m_isCheckable = false;
    bool m_checked = false;
    bool m_enabled = true;



};

QT_END_NAMESPACE

#endif // HNMENUITEM_H
