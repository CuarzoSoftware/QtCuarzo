#ifndef QDBUSMENUBAR_P_H
#define QDBUSMENUBAR_P_H

#include <QWindow>
#include "Common.h"
#include "HNMenu.h"
QT_BEGIN_NAMESPACE

class PlatformTheme;

class HNMenuBar : public QPlatformMenuBar
{
    Q_OBJECT

public:
    HNMenuBar(HNData *data);
    ~HNMenuBar() override;

    void insertMenu(QPlatformMenu *menu, QPlatformMenu *before) override;
    void removeMenu(QPlatformMenu *menu) override;
    void syncMenu(QPlatformMenu *menuItem) override;
    void handleReparent(QWindow *newParentWindow) override;

    QPlatformMenu *menuForTag(quintptr tag) const override;
    QPlatformMenu *createMenu() const override;

    hn_object *object() const;

    QList<MenuClone*>children;
private slots:
    void windowActivatedChanged();
private:
    mutable HNData *m_data;
    hn_object *m_object = nullptr;
    QWindow *m_window = nullptr;
};

QT_END_NAMESPACE

#endif // QDBUSMENUBAR_P_H
