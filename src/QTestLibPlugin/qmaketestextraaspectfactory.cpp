#include "qmaketestextraaspectfactory.h"

#include <projectexplorer/runnables.h>
#include <projectexplorer/target.h>

#include <qmakeprojectmanager/qmakeproject.h>

namespace QTestLibPlugin {
namespace Internal {

bool QMakeTestExtraAspectFactory::isUseful(ProjectExplorer::RunConfiguration* runConfiguration) const
{
    Q_ASSERT(runConfiguration != NULL);

    // Only accept local run configurations:
    qDebug() << runConfiguration->metaObject()->className();
    if (!runConfiguration->runnable().is<ProjectExplorer::StandardRunnable>())
        return false;
    ProjectExplorer::StandardRunnable localRunnable = runConfiguration->runnable().as<ProjectExplorer::StandardRunnable>();

    // Only accept qMake projects:
    QmakeProjectManager::QmakeProject *qMakeProject = qobject_cast<QmakeProjectManager::QmakeProject *>(runConfiguration->target()->project());
    if (qMakeProject == NULL)
        return false;

    foreach(QmakeProjectManager::QmakeProFile *pro, qMakeProject->allProFiles()) {
        qDebug() << "Project name:" << pro->displayName();

        if (!pro->validParse())
            continue;

        // Check the executable matches the target:
        QDir destDir(pro->targetInformation().destDir.toString());
        if (!destDir.isAbsolute())
            destDir.setPath(pro->targetInformation().buildDir.appendPath(pro->targetInformation().destDir.toString()).toString());
        qDebug() << "TARGET:" << destDir.absoluteFilePath(Utils::HostOsInfo::withExecutableSuffix(pro->targetInformation().target));
        qDebug() << "Executable:" << localRunnable.executable;
        if (QDir(destDir.absoluteFilePath(Utils::HostOsInfo::withExecutableSuffix(pro->targetInformation().target))) != QDir(localRunnable.executable))
            continue;
        // Check the testlib is included:
        qDebug() << "QT variable:" << pro->variableValue(QmakeProjectManager::Variable::Qt);
        if (pro->variableValue(QmakeProjectManager::Variable::Qt).contains(QLatin1String("testlib"), Qt::CaseSensitive))
            return true;
    }

    return false;
}

} // Internal
} // QTestLibPlugin
