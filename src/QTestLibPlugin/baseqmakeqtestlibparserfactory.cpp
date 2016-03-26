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

#include "baseqmakeqtestlibparserfactory.h"

#include "testrunconfiguration.h"

#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/localapplicationrunconfiguration.h>
#include <projectexplorer/target.h>
#include <qmakeprojectmanager/qmakeproject.h>
#include <qmakeprojectmanager/qmakenodes.h>

#include <utils/hostosinfo.h>

#include <QtDebug>

namespace QTestLibPlugin {
namespace Internal {

bool BaseQMakeQTestLibParserFactory::canParseRunConfiguration(ProjectExplorer::RunConfiguration* runConfiguration) const
{
    Q_ASSERT(runConfiguration != NULL);

    // Only accept test run configurations:
    TestRunConfiguration* testRunConfiguration = qobject_cast<TestRunConfiguration*>(runConfiguration);
    if (testRunConfiguration == NULL)
        return false;

    // Check test command line arguments:
    QRegExp extraTestArgsRegExp(QLatin1String("TESTARGS=\"([^\"]*)\""));
    if (extraTestArgsRegExp.indexIn(testRunConfiguration->commandLineArguments()) == -1)
        return (mFormat == QTestLibArgsParser::TxtFormat);
    return canParseArguments(extraTestArgsRegExp.capturedTexts().at(1));
}

bool BaseQMakeQTestLibParserFactory::canParseModule(ProjectExplorer::RunConfiguration *runConfiguration) const
{
    Q_ASSERT(runConfiguration != NULL);

    // Only accept local run configurations:
    ProjectExplorer::LocalApplicationRunConfiguration *localRunConfig = qobject_cast<ProjectExplorer::LocalApplicationRunConfiguration *>(runConfiguration);
    if (localRunConfig == NULL)
        return false;

    // Only accept qMake projects:
    QmakeProjectManager::QmakeProject *qMakeProject = qobject_cast<QmakeProjectManager::QmakeProject *>(runConfiguration->target()->project());
    if (qMakeProject == NULL)
        return false;

    foreach(QmakeProjectManager::QmakeProFileNode *pro, qMakeProject->allProFiles()) {
        qDebug() << "Project name:" << pro->displayName();
        // Check the executable matches the target:
        QDir destDir(pro->targetInformation().destDir);
        if (!destDir.isAbsolute())
            destDir.setPath(pro->targetInformation().buildDir + QLatin1Char('/') + pro->targetInformation().destDir);
        qDebug() << "TARGET:" << destDir.absoluteFilePath(Utils::HostOsInfo::withExecutableSuffix(pro->targetInformation().target));
        qDebug() << "Executable:" << localRunConfig->executable();
        if (QDir(destDir.absoluteFilePath(Utils::HostOsInfo::withExecutableSuffix(pro->targetInformation().target))) != QDir(localRunConfig->executable()))
            continue;
        // Check the testlib is included:
        qDebug() << "QT variable:" << pro->variableValue(QmakeProjectManager::QtVar);
        if (pro->variableValue(QmakeProjectManager::QtVar).contains(QLatin1String("testlib"), Qt::CaseSensitive))
            return canParseArguments(localRunConfig->commandLineArguments());
    }

    return false;
}

bool BaseQMakeQTestLibParserFactory::canParseArguments(const QString& cmdArgs) const
{
    QTC_ASSERT(mFormat != QTestLibArgsParser::NoneFormat, return false);

    qDebug() << "Command line args:" << cmdArgs;
    QTestLibArgsParser parser(cmdArgs);
    qDebug() << parser.error() << parser.outputFormat() << parser.outFileName().toString();
    return ((parser.error() == QTestLibArgsParser::NoError)
         && (parser.outputFormat() == mFormat)
          && parser.outFileName().toString().isEmpty());
}

} // namespace Internal
} // namespace QTestLibPlugin
