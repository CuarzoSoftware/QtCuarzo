#include "PlatformTheme.h"
#include "HNMenuBar.h"
#include "HNMenu.h"
#include "HNMenuItem.h"

#include <QApplication>
#include <QFont>
#include <QPalette>
#include <QString>
#include <QVariant>
#include <QDebug>
#include <QLibrary>
#include <QStyleFactory>
#include <QtQuickControls2/QQuickStyle>

// Qt Private
#include <private/qicon_p.h>
#include <private/qiconloader_p.h>
#include <private/qwindow_p.h>
#include <private/qguiapplication_p.h>


PlatformTheme::PlatformTheme(hn_client *heaven)
{
    m_data.heaven = heaven;
    hn_client_set_user_data(m_data.heaven, this);
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, false);
}

PlatformTheme::~PlatformTheme(){}

QVariant PlatformTheme::themeHint(QPlatformTheme::ThemeHint hintType) const
{
    return QPlatformTheme::themeHint(hintType);
}

const QFont* PlatformTheme::font(Font type) const
{
    QFont *f = new QFont("Inter");
    f->setPixelSize(13);
    return f;
}

QPlatformMenuBar *PlatformTheme::createPlatformMenuBar() const
{
    return new HNMenuBar(&m_data);
}

hn_client *PlatformTheme::heaven() const
{
    return m_data.heaven;
}

