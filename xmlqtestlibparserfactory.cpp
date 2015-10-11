#include "xmlqtestlibparserfactory.h"

#include "qtestlibargsparser.h"

#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/localapplicationrunconfiguration.h>
#include <projectexplorer/target.h>
#include <qmakeprojectmanager/qmakeproject.h>
#include <qmakeprojectmanager/qmakenodes.h>

namespace QTestLibPlugin {
namespace Internal {

AbstractTestParser* XMLQTestLibParserFactory::getParserInstance(ProjectExplorer::RunConfiguration *runConfiguration) const
{
    Q_ASSERT(runConfiguration != NULL);

    if (!canParse(runConfiguration))
        return NULL;
    qDebug() << "XMLTextQTestLibParser can parse this file";
    return new XMLQTestLibParser(runConfiguration);
}

bool XMLQTestLibParserFactory::canParseModule(ProjectExplorer::RunConfiguration *runConfiguration) const
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

bool XMLQTestLibParserFactory::canParseArguments(ProjectExplorer::RunConfiguration *runConfiguration) const
{
    ProjectExplorer::LocalApplicationRunConfiguration *localRunConfig = qobject_cast<ProjectExplorer::LocalApplicationRunConfiguration *>(runConfiguration);
    if (localRunConfig != NULL) {
        qDebug() << "Command line args:" << localRunConfig->commandLineArguments();
        QTestLibArgsParser parser(localRunConfig->commandLineArguments());

        return ((parser.error() == QTestLibArgsParser::NoError)
             && (parser.outputFormat() == QTestLibArgsParser::XmlFormat)
              && parser.outFileName().toString().isNull());
    }

    return false;
}

} // namespace Internal
} // namespace QTestLibPlugin
