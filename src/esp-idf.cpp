#include "esp-idf.h"
#include "flashjob.h"

#include <locale>

#include <KPluginFactory>
#include <KProcess>
#include <KConfigGroup>
#include <KMessageBox>
#include <KParts/MainWindow>
#include <KLocalizedString>
#include <KConfigGroup>
#include <KOpenWithDialog>
#include <QIcon>

#include <interfaces/launchconfigurationtype.h>
#include <interfaces/launchconfigurationpage.h>
#include <interfaces/iprojectcontroller.h>
#include <interfaces/iproject.h>
#include <interfaces/iplugincontroller.h>
#include <outputview/outputmodel.h>
#include <interfaces/icore.h>
#include <interfaces/iruncontroller.h>
#include <interfaces/ilaunchconfiguration.h>
#include <outputview/outputjob.h>
#include <util/path.h>

#include "ui_flashjob.h"

QString _ESPIDFConfigTypeId = "ESP-IDF";
QString portEntry = "Port";
QString baudrateEntry = "Baudrate";

K_PLUGIN_FACTORY_WITH_JSON(esp_idfFactory, "esp-idf.json", registerPlugin<esp_idf>(); )


QIcon ESPIDFConfigPage::icon() const
{
    return QIcon::fromTheme("system-run");
}

KDevelop::LaunchConfigurationPage* ESPIDFPageFactory::createWidget(QWidget* parent)
{
    return new ESPIDFConfigPage( parent );
}


void ESPIDFConfigPage::loadFromConfiguration(const KConfigGroup& cfg, KDevelop::IProject* project )
{
    Q_UNUSED(project);

    bool b = blockSignals( true );
    port->setText( cfg.readEntry( portEntry, DEFAULT_PORT ) );
    baudrate->setText( cfg.readEntry( baudrateEntry, QString::number(DEFAULT_BAUDRATE) ) );
    blockSignals( b );
}

ESPIDFPageFactory::ESPIDFPageFactory() {}


ESPIDFConfigPage::ESPIDFConfigPage( QWidget* parent )
    : LaunchConfigurationPage( parent )
{
    setupUi(this);

    // connect signals to changed signal
    connect(port, &KLineEdit::textEdited, this, &ESPIDFConfigPage::changed); 
    connect(baudrate, &KLineEdit::textEdited, this, &ESPIDFConfigPage::changed);
}

void ESPIDFConfigPage::saveToConfiguration( KConfigGroup cfg, KDevelop::IProject* project ) const
{
    Q_UNUSED( project );
    cfg.writeEntry( portEntry, port->text() );
    cfg.writeEntry( baudrateEntry, baudrate->text() );
}

QString ESPIDFConfigPage::title() const
{
    return i18n("Configure ESP-IDF");
}

QString ESPIDFLauncher::description() const
{
    return "Flashes ESP32";
}

QString ESPIDFLauncher::id()
{
    return "ESPIDFLauncher";
}

QString ESPIDFLauncher::name() const
{
    return "ESP-IDF - Flash";
}

QList< KDevelop::LaunchConfigurationPageFactory* > ESPIDFLauncher::configPages() const
{
    return QList<KDevelop::LaunchConfigurationPageFactory*>();
}

ESPIDFLauncher::ESPIDFLauncher(esp_idf *_parent)
{
    parent = _parent;
}

KJob* ESPIDFLauncher::start(const QString& launchMode, KDevelop::ILaunchConfiguration* cfg)
{
    Q_ASSERT(cfg);
    if( !cfg )
    {
        return nullptr;
    }
    if( launchMode == "execute" )
    {
        KConfigGroup grp = cfg->config();
        return new FlashJob(cfg, parent->project, parent->executor, parent->env, 
                            grp.readEntry( baudrateEntry, QString::number(DEFAULT_BAUDRATE) ), 
                            grp.readEntry( portEntry, DEFAULT_PORT ));
    }
    return nullptr;
}

QStringList ESPIDFLauncher::supportedModes() const
{
    return QStringList() << "execute";
}

ESPIDFConfigType::ESPIDFConfigType()
{
   factoryList.append( new ESPIDFPageFactory() );
}

QString ESPIDFConfigType::name() const
{
    return _ESPIDFConfigTypeId;
}

QList<KDevelop::LaunchConfigurationPageFactory*> ESPIDFConfigType::configPages() const
{
    return factoryList;
}

QString ESPIDFConfigType::id() const
{
    return _ESPIDFConfigTypeId;
}

QIcon ESPIDFConfigType::icon() const
{
    return QIcon::fromTheme("system-run");
}

bool ESPIDFConfigType::canLaunch(const QUrl& /*file*/) const
{
    return false;
}

bool ESPIDFConfigType::canLaunch(KDevelop::ProjectBaseItem* /*item*/) const
{
    return false;
}

void ESPIDFConfigType::configureLaunchFromItem(KConfigGroup /*config*/, KDevelop::ProjectBaseItem* /*item*/) const {}

void ESPIDFConfigType::configureLaunchFromCmdLineArguments(KConfigGroup /*config*/, const QStringList &/*args*/) const {}

esp_idf::esp_idf(QObject *parent, const QVariantList& args)
    : KDevelop::IPlugin(QStringLiteral("esp-idf"), parent)
{

    executor = new QProcess(this);
    // init idf paths
    // shouldn't have done that
//     executor->start("bash", QStringList() << "-c" << "source /home/user/esp-idf/export.sh >/dev/null 2>&1 && printenv | grep -e IDF -e PATH");
//     executor->waitForFinished();
// 
//    env = QProcessEnvironment::systemEnvironment();
// 
//     while (executor->canReadLine())
//     {
//         QString line = QString::fromLocal8Bit(executor->readLine());
//         env.insert(line.split("=")[0].trimmed(), line.split("=")[1].trimmed());
//         printf("%s\n", line.split("=")[1].toStdString().c_str());
//     }
    
    ESPIDFConfigType* t = new ESPIDFConfigType();
    t->addLauncher( new ESPIDFLauncher(this) );
    Q_UNUSED(args);

    core()->runController()->addConfigurationType( t );
    
    connect(core()->projectController(), &KDevelop::IProjectController::projectOpened, [this](KDevelop::IProject* _project){
        project = _project;
    });
}

// needed for QObject class created from K_PLUGIN_FACTORY_WITH_JSON
#include "esp-idf.moc"
