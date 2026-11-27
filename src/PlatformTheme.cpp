#include "PlatformTheme.h"
#include "HNMenuBar.h"

#include <QFont>
#include <QVariant>
#include <QCoreApplication>
#include <CZ/Core/CZMouseSettings.h>

PlatformTheme::PlatformTheme(std::shared_ptr<HNClient> heaven)
{
    m_data.heaven = heaven;
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, false);
}

PlatformTheme::~PlatformTheme() {}

QVariant PlatformTheme::themeHint(QPlatformTheme::ThemeHint hintType) const
{
    // As chosen in System Settings (Mouse). Qt asks when the app starts
    if (hintType == QPlatformTheme::MouseDoubleClickInterval)
        return QVariant(int(CZ::CZMouseSettings::DoubleClickMs()));

    return QPlatformTheme::themeHint(hintType);
}

const QFont *PlatformTheme::font(Font type) const
{
    Q_UNUSED(type)
    QFont *f = new QFont("Inter");
    f->setPixelSize(13);
    return f;
}

QPlatformMenuBar *PlatformTheme::createPlatformMenuBar() const
{
    return new HNMenuBar(&m_data);
}

std::shared_ptr<HNClient> PlatformTheme::heaven() const
{
    return m_data.heaven;
}
