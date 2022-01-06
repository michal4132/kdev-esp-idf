#ifndef FLASHJOB_H
#define FLASHJOB_H

#include <interfaces/iproject.h>
#include <outputview/outputjob.h>
#include <QProcess>

namespace KDevelop {
    class ILaunchConfiguration;
    class OutputModel;
}

class FlashJob : public KDevelop::OutputJob
{
    Q_OBJECT
public:
    FlashJob(KDevelop::ILaunchConfiguration* cfg, KDevelop::IProject* _project, QProcess* _executor, QProcessEnvironment _env, QString _baudrate, QString _port);
    virtual void start() override;

protected:
    virtual bool doKill() override;

private slots:
    void sessionFinished();

private:
    QString projectWorkingDir;
    QProcess* executor;
    QProcessEnvironment env;
    QString port;
    QString baudrate;
    KDevelop::OutputModel* model;
    void printOutput();
};


#endif
