/* Copyright 2015 Pascal COMBES <pascom@orange.fr>
 * 
 * This file is part of QTestLibPlugin.
 * 
 * QTestLibPlugin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * QTestLibPlugin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with QTestLibPlugin. If not, see <http://www.gnu.org/licenses/>
 */

#include "localapplicationruncontrol.h"

#include "../qtcreatorfake_global.h"

#include "localapplicationrunconfiguration.h"

#include <QDir>
#include <QFileInfo>
#include <QProcess>

#include <QtDebug>

namespace ProjectExplorer {

LocalApplicationRunControl::LocalApplicationRunControl(LocalApplicationRunConfiguration *runConfig, QObject *parent) :
    RunControl(runConfig, runConfig->displayName(), parent)
{

}

void LocalApplicationRunControl::start(void)
{
    LocalApplicationRunConfiguration *localConfig = qobject_cast<LocalApplicationRunConfiguration *>(runConfiguration());
    Q_ASSERT(localConfig != NULL);

    mTestProc = new QProcess(this);
    mTestProc->setWorkingDirectory(localConfig->workingDirectory());

    connect(mTestProc, SIGNAL(started()), this, SIGNAL(started()));
    connect(mTestProc, SIGNAL(readyReadStandardOutput()), this, SLOT(readStandardOutput()));
    connect(mTestProc, SIGNAL(readyReadStandardError()), this, SLOT(readStandardError()));
    connect(mTestProc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(testProcessFinished(int, QProcess::ExitStatus)));

    debugMsg("Starting:" << localConfig->executable() << "with arguments:" << localConfig->commandLineArguments());
    mTestProc->start(localConfig->executable() + " " + localConfig->commandLineArguments(), QIODevice::ReadOnly);
}


void LocalApplicationRunControl::readStandardOutput(void)
{
    mTestProc->setReadChannel(QProcess::StandardOutput);
    while (!mTestProc->atEnd()) {
        QString line = mTestProc->readLine();
        while (line.endsWith('\n') || line.endsWith('\r'))
            line.chop(1);
        traceMsg("stdout:" << line);
        emit appendMessage(this, line, outputFormat());
    }
}

void LocalApplicationRunControl::readStandardError(void)
{
    mTestProc->setReadChannel(QProcess::StandardError);
    while (!mTestProc->atEnd()) {
        QString line = mTestProc->readLine();
        while (line.endsWith('\n') || line.endsWith('\r'))
            line.chop(1);
        traceMsg("stderr:" << line);
        emit appendMessage(this, line, errorFormat());
    }
}

void LocalApplicationRunControl::testProcessFinished(int exitCode, QProcess::ExitStatus status)
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

