#ifndef PLUGIN_H
#define PLUGIN_H

#include <qpa/qplatformthemeplugin.h>
#include <memory>
#include "Common.h"

namespace CZ { class CZCore; }

QT_BEGIN_NAMESPACE

class PlatformThemePlugin : public QPlatformThemePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QPlatformThemeFactoryInterface_iid FILE "cuarzo-platformtheme.json")

public:
    PlatformThemePlugin(QObject *parent = nullptr);

    QPlatformTheme *create(const QString &key, const QStringList &paramList) override;

private slots:
    void appNameChanged();
    void heavenEvent();

private:
    std::shared_ptr<CZ::CZCore> m_core;
    std::shared_ptr<HNClient> m_heaven;
};

QT_END_NAMESPACE

#endif // PLUGIN_H
