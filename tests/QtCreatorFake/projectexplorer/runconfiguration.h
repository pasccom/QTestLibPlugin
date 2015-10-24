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
