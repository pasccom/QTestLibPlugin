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

#ifndef TARGET_H
#define TARGET_H

#include <QObject>
#include <coreplugin/id.h>
#include "project.h"

namespace ProjectExplorer {

class Kit;
class BuildConfiguration;

class Target : public QObject
{
    Q_OBJECT
public:
    inline Target(Project *project, Kit *kit) :
        QObject(project), mProject(project), mKit(kit), mActiveBuildConfiguration(NULL) {}

    inline Kit *kit(void) const {return mKit;}
    inline Project *project(void) const {return mProject;}

    inline void setActiveBuildConfiguration(BuildConfiguration* buildConfig) {mActiveBuildConfiguration = buildConfig;}
    inline BuildConfiguration* activeBuildConfiguration(void) const {return mActiveBuildConfiguration;}
private:
    Project *mProject;
    Kit *mKit;
    BuildConfiguration* mActiveBuildConfiguration;
};

} // ProjectExplorer

#endif // TARGET_H
