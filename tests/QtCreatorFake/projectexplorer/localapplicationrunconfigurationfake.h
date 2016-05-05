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

#ifndef LOCALAPPLICATIONRUNCONFIGURATIONFAKE_H
#define LOCALAPPLICATIONRUNCONFIGURATIONFAKE_H

#include <projectexplorer/localapplicationrunconfiguration.h>
#include <projectexplorer/applicationlauncher.h>

#include <coreplugin/id.h>

class QWidget;

namespace ProjectExplorer {

class LocalApplicationRunConfigurationFake : public LocalApplicationRunConfiguration
{
    Q_OBJECT
public:
    inline LocalApplicationRunConfigurationFake(ProjectExplorer::Target *target, Core::Id id = Core::Id()) :
        LocalApplicationRunConfiguration(target, id) {};

    inline QWidget* createConfigurationWidget(void) {return NULL;}
    inline ApplicationLauncher::Mode runMode() const {return ApplicationLauncher::Gui;}

    inline QString executable() const {return mExe;}
    inline void setExecutable(const QString& exe) {mExe = exe;}
    inline QString workingDirectory() const {return mDir;}
    inline void setWorkingDirectory(const QString& dir) {mDir = dir;}
    inline QString commandLineArguments() const {return mArgs;}
    inline void setCommandLineArguments(const QString& args) {mArgs = args;}

    //inline QString displayName(void) const {return mExe.section('/', -1, -1, QString::SectionSkipEmpty).section('.', 0, 0, QString::SectionSkipEmpty);}
private:
    QString mExe;
    QString mArgs;
    QString mDir;
};

} // ProjectExplorer

#endif // LOCALAPPLICATIONRUNCONFIGURATIONFAKE_H
