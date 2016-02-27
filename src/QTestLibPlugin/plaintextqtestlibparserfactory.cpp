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

#include "plaintextqtestlibparserfactory.h"

#include "qtestlibargsparser.h"
#include "testrunconfiguration.h"

#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/localapplicationrunconfiguration.h>
#include <projectexplorer/target.h>
#include <qmakeprojectmanager/qmakeproject.h>
#include <qmakeprojectmanager/qmakenodes.h>

#include <QtDebug>

namespace QTestLibPlugin {
namespace Internal {

AbstractTestParser* PlainTextQTestLibParserFactory::getParserInstance(ProjectExplorer::RunConfiguration *runConfiguration) const
{
    Q_ASSERT(runConfiguration != NULL);

    if (!canParse(runConfiguration))
        return NULL;
    qDebug() << "PlainTextQTestLibParser can parse this file";
    return new PlainTextQTestLibParser(runConfiguration);
}

bool PlainTextQTestLibParserFactory::canParseRunConfiguration(ProjectExplorer::RunConfiguration* runConfiguration) const
{
    Q_ASSERT(runConfiguration != NULL);

    TestRunConfiguration* testRunConfiguration = qobject_cast<TestRunConfiguration*>(runConfiguration);
    if (testRunConfiguration == NULL)
        return false;

    QRegExp extraTestArgsRegExp(QLatin1String("TESTARGS=\"[^\"]*\""));
    if (extraTestArgsRegExp.indexIn(testRunConfiguration->commandLineArguments()) == -1)
        return true;
    return canParseArguments(runConfiguration);
}

bool PlainTextQTestLibParserFactory::canParseModule(ProjectExplorer::RunConfiguration *runConfiguration) const
{
    Q_ASSERT(runConfiguration != NULL);

    QmakeProjectManager::QmakeProject *qMakeProject = qobject_cast<QmakeProjectManager::QmakeProject *>(runConfiguration->target()->project());
    if (qMakeProject != NULL) {
        foreach(QmakeProjectManager::QmakeProFileNode *pro, qMakeProject->allProFiles()) {
            qDebug() << "Project name:" << pro->displayName();
            qDebug() << "QT variable:" << pro->variableValue(QmakeProjectManager::QtVar);
            if (pro->variableValue(QmakeProjectManager::QtVar).contains(QLatin1String("testlib"), Qt::CaseSensitive))
                return canParseArguments(runConfiguration);
        }
    }

    return false;
}

bool PlainTextQTestLibParserFactory::canParseArguments(ProjectExplorer::RunConfiguration *runConfiguration) const
{
    ProjectExplorer::LocalApplicationRunConfiguration *localRunConfig = qobject_cast<ProjectExplorer::LocalApplicationRunConfiguration *>(runConfiguration);
    if (localRunConfig != NULL) {
        qDebug() << "Command line args:" << localRunConfig->commandLineArguments();
        QTestLibArgsParser parser(localRunConfig->commandLineArguments());

        return ((parser.error() == QTestLibArgsParser::NoError)
             && (parser.outputFormat() == QTestLibArgsParser::TxtFormat)
              && parser.outFileName().toString().isEmpty());
    }

    return false;
}

} // namespace Internal
} // namespace QTestLibPlugin
