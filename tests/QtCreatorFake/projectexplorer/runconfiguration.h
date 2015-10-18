#ifndef RUNCONFIGURATION_H
#define RUNCONFIGURATION_H

#include <QProcess>
#include <utils/outputformat.h>
#include "target.h"

namespace ProjectExplorer {

class RunConfiguration : public QObject
{
    Q_OBJECT
public:
    inline RunConfiguration(ProjectExplorer::Target *target) :
        QObject(target), mTarget(target) {}
    inline virtual QString displayName(void) const {return QString::null;}
    inline Target *target(void) const {return mTarget;}
private:
    Target *mTarget;
};

class RunControl : public QObject
{
    Q_OBJECT
public:
    RunControl(RunConfiguration *runConfig, const QString& name = QString::null, QObject *parent = NULL);
    inline QString displayName(void) const {return mName;}
    inline RunConfiguration* runConfiguration() const {return mConfig;}

    inline Utils::OutputFormat outputFormat(void) const {return mOutputFormat;}
    inline Utils::OutputFormat errorFormat(void) const {return mErrorFormat;}
    inline void setOutputFormat(Utils::OutputFormat format) {mOutputFormat = format;}
    inline void setErrorFormat(Utils::OutputFormat format) {mErrorFormat = format;}
    inline void setFormats(Utils::OutputFormat outputFormat, Utils::OutputFormat errorFormat) {mOutputFormat = outputFormat; mErrorFormat = errorFormat;}
signals:
    void appendMessage(ProjectExplorer::RunControl *runControl, const QString& msg, Utils::OutputFormat format);
    void finished(void);
    void started(void);
public slots:
    virtual void start(void) = 0;
private:
    QProcess* mTestProc;
    Utils::OutputFormat mOutputFormat;
    Utils::OutputFormat mErrorFormat;
    RunConfiguration* mConfig;
    QString mName;
};

} // ProjectExplorer

#endif // RUNCONFIGURATION_H
