/* Copyright 2015-2016 Pascal COMBES <pascom@orange.fr>
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

#ifndef LOCALAPPLICATIONRUNCONTROLFAKE_H
#define LOCALAPPLICATIONRUNCONTROLFAKE_H

#include <QProcess>
#include <projectexplorer/runconfiguration.h>

#include <utils/outputformat.h>

namespace ProjectExplorer {

class LocalApplicationRunControlFake : public RunControl
{
    Q_OBJECT
public:
    LocalApplicationRunControlFake(RunConfiguration *runConfig);

    inline bool isRunning(void) const {return mTestProc != NULL;}
    inline QString displayName() const {return runConfiguration()->displayName();}

    inline void setOutputFormat(Utils::OutputFormat format) {mOutputFormat = format;}
    inline Utils::OutputFormat outputFormat(void) const {return mOutputFormat;}
    inline void setErrorFormat(Utils::OutputFormat format) {mErrorFormat = format;}
    inline Utils::OutputFormat errorFormat(void) const {return mErrorFormat;}
signals:
    void appendMessage(ProjectExplorer::RunControl *runControl, const QString& msg, Utils::OutputFormat format);
    void finished(void);
    void started(void);
public slots:
    void start(void);
    StopResult stop(void);
private slots:
    void readStandardOutput(void);
    void readStandardError(void);
    void testProcessFinished(int exitCode, QProcess::ExitStatus status);
private:
    QProcess* mTestProc;
    Utils::OutputFormat mOutputFormat;
    Utils::OutputFormat mErrorFormat;
};

} // ProjectExplorer

#endif // LOCALAPPLICATIONRUNCONTROLFAKE_H
