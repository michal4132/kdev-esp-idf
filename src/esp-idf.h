#ifndef ESP_IDF_H
#define ESP_IDF_H

#include <interfaces/launchconfigurationtype.h>
#include <interfaces/launchconfigurationpage.h>
#include <interfaces/ilauncher.h>
#include <interfaces/iplugin.h>
#include <QProcess>

#include "ui_flashjob.h"

#define DEFAULT_PORT      "/dev/ttyUSB0"
#define DEFAULT_BAUDRATE  115200


class esp_idf : public KDevelop::IPlugin
{
    Q_OBJECT

public:
    // KPluginFactory-based plugin wants constructor with this signature
    esp_idf(QObject* parent, const QVariantList& args);
    KDevelop::IProject *project;
    QProcessEnvironment env;
    QProcess* executor;
};

class ESPIDFConfigPage : public KDevelop::LaunchConfigurationPage, Ui::ESPIDFPage
{
    Q_OBJECT
public:
    ESPIDFConfigPage( QWidget* parent );

    void loadFromConfiguration( const KConfigGroup& cfg, KDevelop::IProject* project = nullptr ) override;
    void saveToConfiguration( KConfigGroup cfg, KDevelop::IProject* project = nullptr ) const override;
    QString title() const override;
    QIcon icon() const override;
private:
    void selectDialog();
};

class ESPIDFPageFactory : public KDevelop::LaunchConfigurationPageFactory
{
public:
    ESPIDFPageFactory();

    KDevelop::LaunchConfigurationPage* createWidget(QWidget* parent) override;
};

class ESPIDFLauncher : public KDevelop::ILauncher
{
public:
    ESPIDFLauncher(esp_idf *_parent);
    esp_idf *parent;

    QList< KDevelop::LaunchConfigurationPageFactory* > configPages() const override;
    QString description() const override;
    QString id() override;
    QString name() const override;
    KJob* start(const QString& launchMode, KDevelop::ILaunchConfiguration* cfg) override;
    QStringList supportedModes() const override;
};

class ESPIDFConfigType : public KDevelop::LaunchConfigurationType
{
public:
    ESPIDFConfigType();
    QString id() const override;
    QString name() const override;
    QList<KDevelop::LaunchConfigurationPageFactory*> configPages() const override;
    QIcon icon() const override;
    bool canLaunch( const QUrl& file ) const override;
    bool canLaunch(KDevelop::ProjectBaseItem* item) const override;
    void configureLaunchFromItem(KConfigGroup config, KDevelop::ProjectBaseItem* item) const override;
    void configureLaunchFromCmdLineArguments(KConfigGroup config, const QStringList& args) const override;
private:
    QList<KDevelop::LaunchConfigurationPageFactory*> factoryList;    
};

#endif // ESP_IDF_H
