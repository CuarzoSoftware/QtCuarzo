#ifndef HNMENUITEM_H
#define HNMENUITEM_H

#include <qpa/qplatformmenu.h>

#include "Common.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

class PlatformTheme;

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
    void setShortcut(const QKeySequence& shortcut) override { qDebug() << "HNMenuItem::setShortcut" << shortcut; };
#endif
    void setEnabled(bool enabled) override;
    void setIconSize(int size) override;

    hn_client *heaven() const;
    hn_object *object() const;

private:
    mutable HNData *m_data;
    hn_object *m_object = nullptr;
    QString m_text;
    QIcon m_icon;
    bool m_visible = true;
    bool m_isSeparator = false;
    bool m_isCheckable = false;


};

QT_END_NAMESPACE

#endif // HNMENUITEM_H
