#ifndef PLATFORMTHEME_H
#define PLATFORMTHEME_H

#include <qpa/qplatformtheme.h>
#include "Common.h"

QT_BEGIN_NAMESPACE

class PlatformTheme : public QObject, public QPlatformTheme
{
    Q_OBJECT

public:
    PlatformTheme(hn_client *heaven);
    ~PlatformTheme() override;

    QVariant themeHint(ThemeHint hint) const override;
    const QFont *font(Font type) const override;
    QPlatformMenuBar *createPlatformMenuBar() const override;
    //QPlatformMenu *createPlatformMenu() const override;
    //QPlatformMenuItem *createPlatformMenuItem() const override;
    hn_client *heaven() const;

private:
    mutable HNData m_data;

};

QT_END_NAMESPACE

#endif // PLATFORMTHEME_H
