#include "xmlqtestlibparserfactory.h"

#include "projectexplorer/runconfiguration.h"
#include "projectexplorer/target.h"
#include "qmakeprojectmanager/qmakeproject.h"
#include "qmakeprojectmanager/qmakenodes.h"

namespace QTestLibPlugin {
namespace Internal {

bool XMLQTestLibParserFactory::canParse(ProjectExplorer::RunConfiguration *runConfiguration) const
{
    Q_ASSERT(runConfiguration != NULL);

    QmakeProjectManager::QmakeProject *project = qobject_cast<QmakeProjectManager::QmakeProject *>(runConfiguration->target()->project());
    foreach(QmakeProjectManager::QmakeProFileNode *pro, project->allProFiles()) {
        qDebug() << "Project name:" << pro->displayName();
        qDebug() << "QT variable:" << pro->variableValue(QmakeProjectManager::QtVar);
        if (pro->variableValue(QmakeProjectManager::QtVar).contains(QLatin1String("testlib"), Qt::CaseSensitive)) {
            qDebug() << "XMLTextQTestLibParser can parse this file";
            return true;
        }
    }

    return false;
}

} // namespace Internal
} // namespace QTestLibPlugin
