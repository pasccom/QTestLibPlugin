#ifndef LOCALAPPLICATIONRUNCONTROL_H
#define LOCALAPPLICATIONRUNCONTROL_H

#include <QProcess>
#include "runconfiguration.h"

namespace ProjectExplorer {

class LocalApplicationRunConfiguration;

class LocalApplicationRunControl : public RunControl
{
    Q_OBJECT
public:
    LocalApplicationRunControl(LocalApplicationRunConfiguration *runConfig, QObject *parent = NULL);
signals:
    void appendMessage(ProjectExplorer::RunControl *runControl, const QString& msg, Utils::OutputFormat format);
    void finished(void);
    void started(void);
public slots:
    void start(void);
private slots:
    void readStandardOutput(void);
    void readStandardError(void);
    void testProcessFinished(int exitCode, QProcess::ExitStatus status);
private:
    QProcess* mTestProc;
    RunConfiguration* mConfig;
    QString mName;
};

} // ProjectExplorer

#endif // LOCALAPPLICATIONRUNCONTROL_H
