#include "runconfiguration.h"

#include <QDir>
#include <QFileInfo>
#include <QProcess>

#include <QtDebug>

namespace ProjectExplorer {

RunControl::RunControl(const QString& file, const QStringList& args, const QString& name, QObject *parent) :
    QObject(parent), mTestProc(NULL), mArgs(args), mName(name)
{
    mConfig = new RunConfiguration(this);
    mPath = QFileInfo(file).dir().absoluteFilePath("..");
    mFile = QFileInfo(file).absoluteFilePath();
    if (mName.isNull())
        mName = QFileInfo(file).fileName().section('.', 0, 0, QString::SectionSkipEmpty);
}

void RunControl::start(void)
{
    mTestProc = new QProcess(this);
    mTestProc->setWorkingDirectory(mPath);

    connect(mTestProc, SIGNAL(started()), this, SIGNAL(started()));
    connect(mTestProc, SIGNAL(readyReadStandardOutput()), this, SLOT(readStandardOutput()));
    connect(mTestProc, SIGNAL(readyReadStandardError()), this, SLOT(readStandardError()));
    connect(mTestProc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(testProcessFinished(int, QProcess::ExitStatus)));

    mTestProc->start(mFile, mArgs, QIODevice::ReadOnly);
}


void RunControl::readStandardOutput(void)
{
    mTestProc->setReadChannel(QProcess::StandardOutput);
    while (!mTestProc->atEnd()) {
        QString line = mTestProc->readLine();
        while (line.endsWith('\n') || line.endsWith('\r'))
            line.chop(1);
        //qDebug() << "stdout:" << line;
        emit appendMessage(this, line, mOutputFormat);
    }
}

void RunControl::readStandardError(void)
{
    mTestProc->setReadChannel(QProcess::StandardError);
    while (!mTestProc->atEnd()) {
        QString line = mTestProc->readLine();
        while (line.endsWith('\n') || line.endsWith('\r'))
            line.chop(1);
        //qDebug() << "stderr:" << line;
        emit appendMessage(this, line, mErrorFormat);
    }
}

void RunControl::testProcessFinished(int exitCode, QProcess::ExitStatus status)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(status);

    readStandardOutput();
    readStandardError();

    emit finished();

    delete mTestProc;
    mTestProc = NULL;
}

} // ProjectExplorer

