#include "qmaketestrunconfigurationfactory.h"
#include "testrunconfiguration.h"
#include "qtestlibpluginconstants.h"

#include <projectexplorer/target.h>
#include <projectexplorer/kit.h>

#include <qtsupport/qtsupportconstants.h>

#include <qmakeprojectmanager/qmakeproject.h>
#include <qmakeprojectmanager/qmakenodes.h>

#include <qobject.h>

namespace QTestLibPlugin {
namespace Internal {

QMakeTestRunConfigurationFactory::QMakeTestRunConfigurationFactory(QObject *parent) :
    QObject(parent)
{
}

bool QMakeTestRunConfigurationFactory::canHandle(ProjectExplorer::Project* project) const
{
    QmakeProjectManager::QmakeProject* qMakeProject = qobject_cast<QmakeProjectManager::QmakeProject*>(project);

    if (qMakeProject == NULL)
        return false;

    bool hasDesktopTarget = false;
    foreach (ProjectExplorer::Target* t, qMakeProject->targets()) {
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

bool QMakeTestRunConfigurationFactory::isUseful(ProjectExplorer::Project* project) const
{
    bool hasTests = false;
    QmakeProjectManager::QmakeProject* qMakeProject = qobject_cast<QmakeProjectManager::QmakeProject*>(project);

    if (qMakeProject == NULL)
        return false;

    qDebug() << "QMake project:" << qMakeProject->displayName();
    qDebug() << "    Valid parse:" << qMakeProject->rootQmakeProjectNode()->validParse();
    qDebug() << "    Parse in progress:" << qMakeProject->rootQmakeProjectNode()->parseInProgress();

    if (qMakeProject->rootQmakeProjectNode()->projectType() == QmakeProjectManager::SubDirsTemplate) {
        foreach (QmakeProjectManager::QmakeProFileNode *pro, qMakeProject->applicationProFiles()) {
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

    foreach (ProjectExplorer::RunConfiguration* runConfig, target->runConfigurations()) {
        TestRunConfiguration* testRunConfig = qobject_cast<TestRunConfiguration *>(runConfig);
        if (testRunConfig != NULL)
            return testRunConfig;
    }

    TestRunConfiguration* runConfig = new TestRunConfiguration(target, Core::Id(Constants::TestRunConfigurationId));
    target->addRunConfiguration(runConfig);

    return runConfig;
}

void QMakeTestRunConfigurationFactory::remove(ProjectExplorer::Target* target)
{
    QList<ProjectExplorer::RunConfiguration*> runConfigs = target->runConfigurations();

    foreach (ProjectExplorer::RunConfiguration *runConfig, runConfigs) {
        if (qobject_cast<TestRunConfiguration*>(runConfig) != NULL)
            target->removeRunConfiguration(runConfig);
    }
}

void QMakeTestRunConfigurationFactory::createForAllTargets(ProjectExplorer::Project* project)
{
    foreach (ProjectExplorer::Target* t, project->targets()) {
        if (canHandle(t))
            create(t);
        connect(t, &ProjectExplorer::Target::kitChanged,
                this, [this, t] () {
            if (canHandle(t))
                create(t);
            else
                remove(t);
        });
    }
}

void QMakeTestRunConfigurationFactory::removeForAllTargets(ProjectExplorer::Project* project)
{
    foreach (ProjectExplorer::Target* t, project->targets())
        remove(t);
}

} // Internal
} // QTestLibPlugin

