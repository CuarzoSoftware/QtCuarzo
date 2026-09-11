#ifndef PLATFORMTHEME_H
#define PLATFORMTHEME_H

#include <qpa/qplatformtheme.h>
#include "Common.h"

QT_BEGIN_NAMESPACE

class PlatformTheme : public QObject, public QPlatformTheme
{
    Q_OBJECT

public:
    PlatformTheme(std::shared_ptr<HNClient> heaven);
    ~PlatformTheme() override;

    QVariant themeHint(ThemeHint hint) const override;
    const QFont *font(Font type) const override;
    QPlatformMenuBar *createPlatformMenuBar() const override;

    std::shared_ptr<HNClient> heaven() const;

private:
    mutable HNData m_data;
};

QT_END_NAMESPACE

#endif // PLATFORMTHEME_H
