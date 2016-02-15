#include "qmaketestrunconfigurationfactory.h"
#include "testrunconfiguration.h"

#include <projectexplorer/target.h>
#include <projectexplorer/kit.h>

#include <qtsupport/qtsupportconstants.h>

#include <qmakeprojectmanager/qmakeproject.h>
#include <qmakeprojectmanager/qmakenodes.h>

#include <qobject.h>

namespace QTestLibPlugin {
namespace Internal {

QMakeTestRunConfigurationFactory::QMakeTestRunConfigurationFactory(ProjectExplorer::Project *project) :
    mQMakeProject(NULL)
{
    if (project != NULL)
        mQMakeProject = qobject_cast<QmakeProjectManager::QmakeProject *>(project);
}

bool QMakeTestRunConfigurationFactory::canHandle(void) const
{
    if (mQMakeProject == NULL)
        return false;

    bool hasDesktopTarget = false;
    foreach (ProjectExplorer::Target* t, mQMakeProject->targets()) {
        if (!canHandle(t))
            continue;

        hasDesktopTarget = true;
        break;
    }

    return hasDesktopTarget;
}

bool QMakeTestRunConfigurationFactory::canHandle(ProjectExplorer::Target* target) const
{
    Q_ASSERT((target != NULL) && (target->kit() != NULL));
    if (target->kit() == NULL)
        return false;
    if (target->kit()->hasFeatures(Core::FeatureSet(QtSupport::Constants::FEATURE_DESKTOP)))
        return true;
    return false;
}

bool QMakeTestRunConfigurationFactory::isUseful(void) const
{
    bool hasTests = false;

    qDebug() << "QMake project:" << mQMakeProject->displayName();
    qDebug() << "    Valid parse:" << mQMakeProject->rootQmakeProjectNode()->validParse();
    qDebug() << "    Parse in progress:" << mQMakeProject->rootQmakeProjectNode()->parseInProgress();

    if (mQMakeProject->rootQmakeProjectNode()->projectType() == QmakeProjectManager::SubDirsTemplate) {
        foreach (QmakeProjectManager::QmakeProFileNode *pro, mQMakeProject->applicationProFiles()) {
            qDebug() << "    Pro file:" << pro->displayName();
            qDebug() << "        Config:" << pro->variableValue(QmakeProjectManager::ConfigVar);
            if (!pro->variableValue(QmakeProjectManager::ConfigVar).contains(QLatin1String("testcase"), Qt::CaseSensitive))
                continue;

            hasTests = true;
            break;
        }
    }

    return hasTests;
}

TestRunConfiguration* QMakeTestRunConfigurationFactory::create(ProjectExplorer::Target* target)
{
    if (!canHandle(target))
        return NULL;

    TestRunConfiguration* runConfig = new TestRunConfiguration(target, Core::Id());
    target->addRunConfiguration(runConfig);

    return runConfig;
}

void QMakeTestRunConfigurationFactory::createForAllTargets(void)
{
    if (!isUseful())
        return;

    foreach (ProjectExplorer::Target* t, mQMakeProject->targets()) {
        Q_ASSERT(t->kit() != NULL);
        if (t->kit() == NULL)
            continue;
        if (!t->kit()->hasFeatures(Core::FeatureSet(QtSupport::Constants::FEATURE_DESKTOP)))
            continue;

        create(t);
    }
}

} // Internal
} // QTestLibPlugin

