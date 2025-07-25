/* Copyright 2015-2020 Pascal COMBES <pascom@orange.fr>
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
#include <projectexplorer/runcontrol.h>
#include <projectexplorer/target.h>
#include <qmakeprojectmanager/qmakeproject.h>
#include <qmakeprojectmanager/qmakenodes.h>

#include <utils/hostosinfo.h>
#include <utils/processinterface.h>

#include <QRegExp>
#include <QtDebug>

namespace QTestLibPlugin {
namespace Internal {

bool BaseQMakeQTestLibParserFactory::canParseRunConfiguration(ProjectExplorer::RunControl* runControl) const
{
    Q_ASSERT(runControl != nullptr);

    // Only accept test run configurations:
    qDebug() << "Run control build key:" << runControl->buildKey();
    if (QString::compare(runControl->buildKey(), QLatin1String("make check"), Qt::CaseSensitive) != 0)
        return false;
    TestRunConfiguration* testRunConfiguration;
    for (ProjectExplorer::RunConfiguration* runConfig : runControl->target()->runConfigurations()) {
        testRunConfiguration = qobject_cast<TestRunConfiguration*>(runConfig);
        if (testRunConfiguration != nullptr)
            break;
    }
    if (testRunConfiguration == nullptr)
        return false;

    // Check test command line arguments:
    QRegExp extraTestArgsRegExp(QLatin1String("TESTARGS=\"([^\"]*)\""));
    for (QString arg : testRunConfiguration->commandLineArguments()) {
        if (extraTestArgsRegExp.indexIn(arg) != -1)
            break;
    }
    qDebug() << "Extra test args:" << extraTestArgsRegExp.capturedTexts();
    if (extraTestArgsRegExp.capturedTexts().isEmpty())
        return (mFormat == QTestLibArgsParser::TxtFormat);
    return canParseArguments(extraTestArgsRegExp.capturedTexts().at(1));
}

bool BaseQMakeQTestLibParserFactory::canParseModule(ProjectExplorer::RunControl* runControl) const
{
    Q_ASSERT(runControl != nullptr);

    // Only accept qMake projects:
    QmakeProjectManager::QmakePriFileNode* qMakeRootNode = dynamic_cast<QmakeProjectManager::QmakePriFileNode*>(runControl->target()->project()->rootProjectNode());
    if (qMakeRootNode == nullptr)
        return false;
    Utils::ProcessRunData runnable = runControl->runnable();

    foreach(QmakeProjectManager::QmakeProFile *pro, qMakeRootNode->proFileNode()->proFile()->allProFiles()) {
        qDebug() << "Project name:" << pro->displayName();
        // Check the executable matches the target:
        Utils::FilePath destDir = pro->targetInformation().destDir;
        if (!destDir.isAbsolutePath())
            destDir = pro->targetInformation().buildDir.pathAppended(pro->targetInformation().destDir.toFSPathString()).cleanPath();
        qDebug() << "TARGET:" << destDir.pathAppended(Utils::HostOsInfo::withExecutableSuffix(pro->targetInformation().target));
        qDebug() << "Executable:" << runnable.command.executable();
        if (destDir.pathAppended(Utils::HostOsInfo::withExecutableSuffix(pro->targetInformation().target)) != runnable.command.executable())
            continue;
        // Check the testlib is included:
        qDebug() << "QT variable:" << pro->variableValue(QmakeProjectManager::Variable::Qt);
        if (pro->variableValue(QmakeProjectManager::Variable::Qt).contains(QLatin1String("testlib"), Qt::CaseSensitive))
            return canParseArguments(runnable.command.arguments());
    }

    return false;
}

bool BaseQMakeQTestLibParserFactory::canParseArguments(const QString& cmdArgs) const
{
    QTC_ASSERT(mFormat != QTestLibArgsParser::NoneFormat, return false);

    qDebug() << "Command line args:" << cmdArgs;
    QTestLibArgsParser parser(cmdArgs);
    qDebug() << parser.error() << parser.outputFormat() << parser.outFileName();
    return ((parser.error() == QTestLibArgsParser::NoError)
         && (parser.outputFormat() == mFormat)
          && parser.outFileName().isEmpty());
}

} // namespace Internal
} // namespace QTestLibPlugin
