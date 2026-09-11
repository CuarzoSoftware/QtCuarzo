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
    void setFont(const QFont &font) override { Q_UNUSED(font) }
    void setRole(MenuRole role) override;
    void setCheckable(bool checkable) override;
    void setChecked(bool isChecked) override;
#if QT_CONFIG(shortcut)
    void setShortcut(const QKeySequence &shortcut) override;
#endif
    void setEnabled(bool enabled) override;
    void setIconSize(int size) override;

    void setTag(quintptr tag) override;
    quintptr tag() const override;

    ItemClone *createClone();
    std::shared_ptr<HNClient> heaven() const;

    HNMenu *menu = nullptr;

    std::string m_icon; // freedesktop icon name

    QString m_text;
    QString m_shortcut;
    bool m_visible = true;
    bool m_isSeparator = false;
    bool m_isCheckable = false;
    bool m_checked = false;
    bool m_enabled = true;

    QList<ItemClone*> clones;

private:
    mutable HNData *m_data;
    quintptr m_tag = 0;
};

QT_END_NAMESPACE

#endif // HNMENUITEM_H
