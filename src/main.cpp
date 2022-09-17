#include <qpa/qplatformthemeplugin.h>
#include <QGuiApplication>
#include "PlatformTheme.h"

QT_BEGIN_NAMESPACE

static void disconnected_from_server(hn_client *)
{
    qDebug() << "Disconnected from Heaven server.";
}

static void object_destroy(hn_object *)
{

}

static void server_action_invoke(hn_action *)
{

}

static void server_toggle_set_checked(hn_toggle *, hn_bool)
{

}

static void server_option_set_active(hn_option *, hn_select *)
{

}

static void server_send_custom_event(hn_client *, void *, u_int32_t size)
{

}

hn_client_events_interface heavenClientInterface
{
    &disconnected_from_server,
    &object_destroy,
    &server_action_invoke,
    &server_toggle_set_checked,
    &server_option_set_active,
    &server_send_custom_event
};

class PlatformThemePlugin : public QPlatformThemePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QPlatformThemeFactoryInterface_iid FILE "cuarzo-platformtheme.json")

public:
    PlatformThemePlugin(QObject *parent = nullptr) : QPlatformThemePlugin(parent)
    {
    }

    QPlatformTheme *create(const QString &key, const QStringList &paramList) override
    {
        Q_UNUSED(key)
        Q_UNUSED(paramList)

        QString appName = "Qt Application";

        if(QGuiApplication::applicationName().length() != 0)
            appName = QGuiApplication::applicationName();

        heaven = hn_client_create(NULL, appName.toUtf8(), NULL, &heavenClientInterface);

        if(heaven)
        {
            connect(QGuiApplication::instance(), &QCoreApplication::applicationNameChanged, this, &PlatformThemePlugin::appNameChanged);
            return new PlatformTheme(heaven);
        }


        return nullptr;
    }
private:
    hn_client *heaven = nullptr;

private slots:
    void appNameChanged()
    {
        hn_client_set_app_name(heaven, QGuiApplication::applicationName().toUtf8());
    }
};

QT_END_NAMESPACE

#include "main.moc"
