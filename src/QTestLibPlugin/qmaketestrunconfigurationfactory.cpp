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
    IRunConfigurationFactory(parent)
{
}

QList<Core::Id> QMakeTestRunConfigurationFactory::availableCreationIds(ProjectExplorer::Target *target, CreationMode mode) const
{
    Q_UNUSED(mode);

    if (canHandle(target) && isUseful(target->project()))
        return QList<Core::Id>() << Core::Id(Constants::TestRunConfigurationId);

    return QList<Core::Id>();
}

QString QMakeTestRunConfigurationFactory::displayNameForId(Core::Id id) const
{
    QTC_ASSERT(id == Core::Id(Constants::TestRunConfigurationId), return QString::null);
    return QLatin1String("make check");
}

bool QMakeTestRunConfigurationFactory::canHandle(ProjectExplorer::Target* target) const
{
    QTC_ASSERT((target != NULL) && (target->kit() != NULL), return false);

    if (qobject_cast<QmakeProjectManager::QmakeProject*>(target->project()) == NULL)
        return false;

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

    QTC_ASSERT(qMakeProject != NULL, return false);
    QTC_ASSERT(qMakeProject->rootQmakeProjectNode() != NULL, return false);
    QTC_ASSERT(qMakeProject->rootQmakeProjectNode()->validParse(), return false);

    //qDebug() << "QMake project:" << qMakeProject->displayName();
    //qDebug() << "    Valid parse:" << qMakeProject->rootQmakeProjectNode()->validParse();
    //qDebug() << "    Parse in progress:" << qMakeProject->rootQmakeProjectNode()->parseInProgress();

    if (qMakeProject->rootQmakeProjectNode()->projectType() == QmakeProjectManager::SubDirsTemplate) {
        foreach (QmakeProjectManager::QmakeProFileNode *pro, qMakeProject->applicationProFiles()) {
            //qDebug() << "    Pro file:" << pro->displayName();
            //qDebug() << "        Config:" << pro->variableValue(QmakeProjectManager::ConfigVar);
            if (!pro->variableValue(QmakeProjectManager::ConfigVar).contains(QLatin1String("testcase"), Qt::CaseSensitive))
                continue;

            hasTests = true;
            break;
        }
    }

    return hasTests;
}

bool QMakeTestRunConfigurationFactory::canCreate(ProjectExplorer::Target *target, Core::Id id) const
{
    qDebug() << __func__ << target->displayName() << id.toString();

    QTC_ASSERT(canHandle(target) && isUseful(target->project()), return false);
    QTC_ASSERT(id == Core::Id(Constants::TestRunConfigurationId), return false);

    return true;
}

bool QMakeTestRunConfigurationFactory::canRestore(ProjectExplorer::Target *target, const QVariantMap &map) const
{
    qDebug() << __func__ << target->displayName() << map;

    return canHandle(target) && (ProjectExplorer::idFromMap(map) == Core::Id(Constants::TestRunConfigurationId));
}

bool QMakeTestRunConfigurationFactory::canClone(ProjectExplorer::Target *target, ProjectExplorer::RunConfiguration *product) const
{
    qDebug() << __func__ << product->metaObject()->className();

    return canHandle(target) && (qobject_cast<TestRunConfiguration *>(product) != NULL);
}

ProjectExplorer::RunConfiguration *QMakeTestRunConfigurationFactory::clone(ProjectExplorer::Target *target, ProjectExplorer::RunConfiguration *product)
{
    QTC_ASSERT(canHandle(target), return NULL);
    QTC_ASSERT(qobject_cast<TestRunConfiguration *>(product) != NULL, return NULL);

    qDebug() << "Cloning run configuration for target:" << target->displayName();
    TestRunConfiguration* runConfig = new TestRunConfiguration(target, Core::Id(Constants::TestRunConfigurationId));
    runConfig->fromMap(product->toMap());

    return runConfig;
}

ProjectExplorer::RunConfiguration* QMakeTestRunConfigurationFactory::doCreate(ProjectExplorer::Target* target, Core::Id id)
{
    qDebug() << "Creating run configuration for target:" << target->displayName();
    TestRunConfiguration* runConfig = new TestRunConfiguration(target, id);

    return runConfig;
}

ProjectExplorer::RunConfiguration* QMakeTestRunConfigurationFactory::doRestore(ProjectExplorer::Target* target, const QVariantMap& map)
{
    qDebug() << "Restoring run configuration for target:" << target->displayName();
    TestRunConfiguration* runConfig = new TestRunConfiguration(target, ProjectExplorer::idFromMap(map));

    return runConfig;
}

/*TestRunConfiguration* QMakeTestRunConfigurationFactory::create(ProjectExplorer::Target* target)
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
}*/

/*void QMakeTestRunConfigurationFactory::remove(ProjectExplorer::Target* target)
{
    QList<ProjectExplorer::RunConfiguration*> runConfigs = target->runConfigurations();

    foreach (ProjectExplorer::RunConfiguration *runConfig, runConfigs) {
        if (qobject_cast<TestRunConfiguration*>(runConfig) != NULL)
            target->removeRunConfiguration(runConfig);
    }
}*/

/*void QMakeTestRunConfigurationFactory::createForAllTargets(ProjectExplorer::Project* project)
{
    foreach (ProjectExplorer::Target* t, project->targets()) {
        if (canHandle(t))
            t->addRunConfiguration(create(t, Core::Id(Constants::TestRunConfigurationId)));
        connect(t, SIGNAL(kitChanged()),
                this, SLOT(updateTargetKit()));
    }
}*/

/*void QMakeTestRunConfigurationFactory::updateTargetKit(void)
{
    ProjectExplorer::Target* target = qobject_cast<ProjectExplorer::Target*>(sender());
    Q_ASSERT(target != NULL);

    if (canHandle(target))
        create(target, Core::Id(Constants::TestRunConfigurationId));
    else
        remove(target);
}*/

/*void QMakeTestRunConfigurationFactory::removeForAllTargets(ProjectExplorer::Project* project)
{
    foreach (ProjectExplorer::Target* t, project->targets()) {
        remove(t);
        disconnect(t, SIGNAL(kitChanged()), this, SLOT(updateTargetKit()));
    }
}*/

} // Internal
} // QTestLibPlugin
