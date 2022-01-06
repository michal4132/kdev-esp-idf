#include "flashjob.h"

#include <interfaces/icore.h>
#include <interfaces/iplugincontroller.h>
#include <outputview/outputmodel.h>
#include <interfaces/iproject.h>
#include <util/path.h>

FlashJob::FlashJob(KDevelop::ILaunchConfiguration* cfg, KDevelop::IProject* _project, QProcess* _executor, QProcessEnvironment _env, QString _baudrate, QString _port)
{
    Q_UNUSED(cfg);
    setCapabilities(Killable);

    projectWorkingDir = _project->path().path();

    executor = _executor;
    env = _env;
    port = _port;
    baudrate = _baudrate;
}


void FlashJob::printOutput(){
    while (executor->canReadLine())
    {
        QString line = QString::fromLocal8Bit(executor->readLine());
        line.chop(1);
        model->appendLine(line);
    }
} 


void FlashJob::start()
{
    setStandardToolView( KDevelop::IOutputView::RunView );
    setBehaviours( KDevelop::IOutputView::AllowUserClose | KDevelop::IOutputView::AutoScroll );
    setTitle("esp-idf flash");
    
    model = new KDevelop::OutputModel();
    setModel( model );

    startOutput(); 

    executor->setProcessEnvironment(env);
    executor->setWorkingDirectory( projectWorkingDir );
    executor->setProcessChannelMode(QProcess::MergedChannels);
    executor->start( "bash", QStringList() << "-c" << "idf.py flash -b " + baudrate + " -p " + port);
    executor->setReadChannel(QProcess::StandardOutput);
        
    connect( executor, &QProcess::readyReadStandardError, this, &FlashJob::printOutput );
    connect( executor, &QProcess::readyReadStandardOutput, this, &FlashJob::printOutput );
}

bool FlashJob::doKill()
{
    return true;
}

void FlashJob::sessionFinished()
{
    emitResult();
}

#include "flashjob.moc"
