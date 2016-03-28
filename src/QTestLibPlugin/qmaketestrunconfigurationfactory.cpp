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

void updateRunConfiguration(TestRunConfiguration* runConfig, QmakeProjectManager::QmakeProFileNode* qMakeRoot)
{
    if (qMakeRoot == NULL)
        return;
    QStringList makefile = qMakeRoot->variableValue(QmakeProjectManager::Makefile);
    if (makefile.size() == 0) {
        runConfig->setMakefile(Utils::FileName());
    } else {
        QTC_ASSERT(makefile.size() == 1, );
        runConfig->setMakefile(Utils::FileName::fromString(qMakeRoot->targetInformation().buildDir).appendPath(makefile.first()));
    }
}


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

bool QMakeTestRunConfigurationFactory::canHandle(ProjectExplorer::Target* target)
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

bool QMakeTestRunConfigurationFactory::isReady(ProjectExplorer::Project* project)
{
    QmakeProjectManager::QmakeProject* qMakeProject = qobject_cast<QmakeProjectManager::QmakeProject*>(project);
    if (qMakeProject == NULL)
        return false;
    return qMakeProject->rootQmakeProjectNode()->validParse();
}

bool QMakeTestRunConfigurationFactory::isUseful(ProjectExplorer::Project* project)
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
    QTC_ASSERT(canHandle(target) && isUseful(target->project()), return false);
    if (id == Core::Id(Constants::TestRunConfigurationId))
        return true;

    return false;
}

bool QMakeTestRunConfigurationFactory::canRestore(ProjectExplorer::Target *target, const QVariantMap &map) const
{
    return canHandle(target) && (ProjectExplorer::idFromMap(map) == Core::Id(Constants::TestRunConfigurationId));
}

bool QMakeTestRunConfigurationFactory::canClone(ProjectExplorer::Target *target, ProjectExplorer::RunConfiguration *product) const
{
    return canHandle(target) && (qobject_cast<TestRunConfiguration *>(product) != NULL);
}

ProjectExplorer::RunConfiguration *QMakeTestRunConfigurationFactory::clone(ProjectExplorer::Target *target, ProjectExplorer::RunConfiguration *product)
{
    QTC_ASSERT(canHandle(target), return NULL);
    QTC_ASSERT(qobject_cast<TestRunConfiguration *>(product) != NULL, return NULL);

    qDebug() << "Cloning run configuration for target:" << target->displayName();
    TestRunConfiguration* runConfig = new TestRunConfiguration(target, Core::Id(Constants::TestRunConfigurationId));
    runConfig->fromMap(product->toMap());

    QmakeProjectManager::QmakeProject* qMakeProject = qobject_cast<QmakeProjectManager::QmakeProject*>(target->project());
    connect(qMakeProject, &QmakeProjectManager::QmakeProject::proFilesEvaluated,
            this, [runConfig, qMakeProject] () {
        updateRunConfiguration(runConfig, qMakeProject->rootQmakeProjectNode());
    });
    updateRunConfiguration(runConfig, qMakeProject->rootQmakeProjectNode());

    return runConfig;
}

ProjectExplorer::RunConfiguration* QMakeTestRunConfigurationFactory::doCreate(ProjectExplorer::Target* target, Core::Id id)
{
    qDebug() << "Creating run configuration for target:" << target->displayName();
    TestRunConfiguration* runConfig = new TestRunConfiguration(target, id);

    QmakeProjectManager::QmakeProject* qMakeProject = qobject_cast<QmakeProjectManager::QmakeProject*>(target->project());
    connect(qMakeProject, &QmakeProjectManager::QmakeProject::proFilesEvaluated,
            this, [runConfig, qMakeProject] () {
        updateRunConfiguration(runConfig, qMakeProject->rootQmakeProjectNode());
    });
    updateRunConfiguration(runConfig, qMakeProject->rootQmakeProjectNode());

    return runConfig;
}

ProjectExplorer::RunConfiguration* QMakeTestRunConfigurationFactory::doRestore(ProjectExplorer::Target* target, const QVariantMap& map)
{
    qDebug() << "Restoring run configuration for target:" << target->displayName();
    TestRunConfiguration* runConfig = new TestRunConfiguration(target, ProjectExplorer::idFromMap(map));

    QmakeProjectManager::QmakeProject* qMakeProject = qobject_cast<QmakeProjectManager::QmakeProject*>(target->project());
    connect(qMakeProject, &QmakeProjectManager::QmakeProject::proFilesEvaluated,
            this, [runConfig, qMakeProject] () {
        updateRunConfiguration(runConfig, qMakeProject->rootQmakeProjectNode());
    });
    updateRunConfiguration(runConfig, qMakeProject->rootQmakeProjectNode());

    return runConfig;
}

} // Internal
} // QTestLibPlugin
