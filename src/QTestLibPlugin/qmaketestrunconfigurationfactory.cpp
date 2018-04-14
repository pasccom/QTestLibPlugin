/* Copyright 2016 Pascal COMBES <pascom@orange.fr>
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

#include "qmaketestrunconfigurationfactory.h"
#include "testrunconfiguration.h"
#include "qtestlibpluginconstants.h"

#include <projectexplorer/target.h>

#include <qmakeprojectmanager/qmakeprojectmanagerconstants.h>
#include <qmakeprojectmanager/qmakeproject.h>

#include <qobject.h>

namespace QTestLibPlugin {
namespace Internal {

QMakeTestRunConfigurationFactory::QMakeTestRunConfigurationFactory(QObject *parent) :
    IRunConfigurationFactory(parent)
{
    addSupportedProjectType(QmakeProjectManager::Constants::QMAKEPROJECT_ID);
    setSupportedTargetDeviceTypes({ProjectExplorer::Constants::DESKTOP_DEVICE_TYPE});

    addFixedBuildTarget(QLatin1String("make check"));
    registerRunConfiguration<TestRunConfiguration>(Core::Id(Constants::TestRunConfigurationId));
}

bool QMakeTestRunConfigurationFactory::canHandle(ProjectExplorer::Target* target) const
{
    return IRunConfigurationFactory::canHandle(target) && (!isReady(target->project()) || isUseful(target->project()));
}

bool QMakeTestRunConfigurationFactory::canCreateHelper(ProjectExplorer::Target* target, const QString& extra) const
{
    Q_UNUSED(extra);
    return IRunConfigurationFactory::canHandle(target) && isReady(target->project()) && isUseful(target->project());
}

bool QMakeTestRunConfigurationFactory::isReady(ProjectExplorer::Project* project)
{
    QmakeProjectManager::QmakeProject* qMakeProject = qobject_cast<QmakeProjectManager::QmakeProject*>(project);
    if (qMakeProject == NULL)
        return false;
    return qMakeProject->rootProFile()->validParse();
}

bool QMakeTestRunConfigurationFactory::isUseful(ProjectExplorer::Project* project)
{
    bool hasTests = false;
    QmakeProjectManager::QmakeProject* qMakeProject = qobject_cast<QmakeProjectManager::QmakeProject*>(project);

    QTC_ASSERT(qMakeProject != NULL, return false);
    QTC_ASSERT(qMakeProject->rootProFile() != NULL, return false);
    QTC_ASSERT(qMakeProject->rootProFile()->validParse(), return false);

    //qDebug() << "QMake project:" << qMakeProject->displayName();
    //qDebug() << "    Valid parse:" << qMakeProject->rootQmakeProjectNode()->validParse();
    //qDebug() << "    Parse in progress:" << qMakeProject->rootQmakeProjectNode()->parseInProgress();

    if (qMakeProject->rootProFile()->projectType() == QmakeProjectManager::ProjectType::SubDirsTemplate) {
        foreach (QmakeProjectManager::QmakeProFile *pro, qMakeProject->applicationProFiles()) {
            //qDebug() << "    Pro file:" << pro->displayName();
            //qDebug() << "        Config:" << pro->variableValue(QmakeProjectManager::ConfigVar);
            if (!pro->variableValue(QmakeProjectManager::Variable::Config).contains(QLatin1String("testcase"), Qt::CaseSensitive))
                continue;

            hasTests = true;
            break;
        }
    }

    return hasTests;
}

} // Internal
} // QTestLibPlugin
