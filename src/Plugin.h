#ifndef PLUGIN_H
#define PLUGIN_H

#include <qpa/qplatformthemeplugin.h>
#include <memory>
#include "Common.h"
#include <CZ/Core/CZObject.h>

namespace CZ { class CZCore; }

QT_BEGIN_NAMESPACE

class PlatformThemePlugin : public QPlatformThemePlugin
{
    Q_OBJECT
    // The literal QPlatformThemeFactoryInterface_iid (qpa/qplatformthemeplugin.h): moc can't expand
    // the macro where it doesn't get Qt's private include paths (e.g. Debian / Raspberry Pi OS),
    // failing with "Parse error at IID"
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QPA.QPlatformThemeFactoryInterface.5.1" FILE "cuarzo-platformtheme.json")

public:
    PlatformThemePlugin(QObject *parent = nullptr);

    QPlatformTheme *create(const QString &key, const QStringList &paramList) override;

private slots:
    void appNameChanged();
    void heavenEvent();

private:
    std::shared_ptr<CZ::CZCore> m_core;
    std::shared_ptr<HNClient> m_heaven;
    // Owns the Heaven app-menu signal subscriptions (About/Settings/Quit). Declared after m_heaven
    // so it is destroyed first, unsubscribing before the client's signals go away.
    CZ::CZObject m_heavenListener;
};

QT_END_NAMESPACE

#endif // PLUGIN_H
