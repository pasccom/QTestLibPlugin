#ifndef RUNCONTROL_H
#define RUNCONTROL_H

#include <QProcess>
#include <utils/outputformat.h>

namespace ProjectExplorer {

class RunConfiguration : public QObject
{
public:
    inline RunConfiguration(QObject *parent):
        QObject(parent) {}
};

class RunControl : public QObject
{
    Q_OBJECT
public:
    RunControl(const QString& file, const QStringList& args = QStringList(), const QString& name = QString::null, QObject *parent = 0);
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
    void start(void);
private slots:
    void readStandardOutput(void);
    void readStandardError(void);
    void testProcessFinished(int exitCode, QProcess::ExitStatus status);
private:
    QProcess* mTestProc;
    QString mPath;
    QString mFile;
    QStringList mArgs;
    QString mName;
    Utils::OutputFormat mOutputFormat;
    Utils::OutputFormat mErrorFormat;
    RunConfiguration* mConfig;
};

} // ProjectExplorer

#endif // RUNCONTROL_H
