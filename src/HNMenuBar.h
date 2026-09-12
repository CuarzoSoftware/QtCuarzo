#ifndef HNMENUBAR_H
#define HNMENUBAR_H

#include <QWindow>
#include <QPointer>
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

    HNTopbar *object() const { return m_object.get(); }

    QList<MenuClone*> children;

private slots:
    void windowActivatedChanged();

private:
    mutable HNData *m_data;
    std::shared_ptr<HNTopbar> m_object;
    // QPointer so it auto-nulls if the parent window is destroyed during reparenting; a raw pointer
    // would dangle and crash the disconnect() in handleReparent().
    QPointer<QWindow> m_window;
};

QT_END_NAMESPACE

#endif // HNMENUBAR_H
