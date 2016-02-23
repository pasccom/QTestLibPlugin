#include "qmakemakechecktest.h"

#include "../qtestlibpluginconstants.h"

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projecttree.h>
#include <projectexplorer/session.h>
#include <projectexplorer/target.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/buildinfo.h>

#include <qmakeprojectmanager/qmakeproject.h>

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/icore.h>

#include <utils/proxyaction.h>

#include <QAction>
#include <QtTest>

namespace QTestLibPlugin {
namespace Test {

QMakeMakeCheckTest::QMakeMakeCheckTest(void):
    QObject(NULL), mProject(NULL)
{
}

void QMakeMakeCheckTest::init(void)
{
    mProject = NULL;
}

void QMakeMakeCheckTest::cleanup(void)
{
    if (mProject != NULL)
        ProjectExplorer::SessionManager::removeProject(mProject);

    Core::Context projectTreeContext(ProjectExplorer::Constants::C_PROJECT_TREE);
    Core::ICore::removeAdditionalContext(projectTreeContext);
}

void QMakeMakeCheckTest::testOpenProjectWithTests_data(void)
{
    QTest::addColumn<QString>("projectPath");

    QTest::newRow("OneSubTest") << TESTS_DIR "/OneSubTest/OneSubTest.pro";
    QTest::newRow("TwoSubTests") << TESTS_DIR "/TwoSubTests/TwoSubTests.pro";
}

void QMakeMakeCheckTest::testOpenProjectWithTests(void)
{
    QFETCH(QString, projectPath);

    // Open project
    SUB_TEST_FUNCTION(openProject(projectPath));

    // Wait for project evaluated
    QmakeProjectManager::QmakeProject* qMakeProject = qobject_cast<QmakeProjectManager::QmakeProject*>(mProject);
    QVERIFY(qMakeProject != NULL);
    QSignalSpy evaluateSpy(qMakeProject, SIGNAL(proFilesEvaluated()));
    evaluateSpy.wait();

    // Retrieve commands
    Core::Command* runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
    Core::Command* runProjectTestsCommand = Core::ActionManager::command(Core::Id(Constants::TestRunActionId).withSuffix(mProject->projectFilePath().toString()).withSuffix(mProject->activeTarget()->id().toString()));
    QVERIFY(runProjectTestsCommand != NULL);

    // Enter the required context
    Core::Context projectTreeContext(ProjectExplorer::Constants::C_PROJECT_TREE);
    Core::ICore::addAdditionalContext(projectTreeContext);

    // Check actions are enabled
    QVERIFY(runTestsCommand->action()->isEnabled());
    QVERIFY(runProjectTestsCommand->action()->isEnabled());

    // Check action text is right
    QCOMPARE(runTestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject->displayName()).arg(mProject->activeTarget()->displayName()));
    QCOMPARE(runProjectTestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject->displayName()).arg(mProject->activeTarget()->displayName()));
}

void QMakeMakeCheckTest::testOpenProjectWithoutTests_data(void)
{
    QTest::addColumn<QString>("projectPath");

    QTest::newRow("NoSubTestOne") << TESTS_DIR "/NoSubTestOne/NoSubTestOne.pro";
    QTest::newRow("NoSubTestTwo") << TESTS_DIR "/NoSubTestTwo/NoSubTestTwo.pro";
}

void QMakeMakeCheckTest::testOpenProjectWithoutTests(void)
{
    QFETCH(QString, projectPath);

    // Open project
    SUB_TEST_FUNCTION(openProject(projectPath));

    // Wait for project evaluated
    QmakeProjectManager::QmakeProject* qMakeProject = qobject_cast<QmakeProjectManager::QmakeProject*>(mProject);
    QVERIFY(qMakeProject != NULL);
    QSignalSpy evaluateSpy(qMakeProject, SIGNAL(proFilesEvaluated()));
    evaluateSpy.wait();

    // Retrieve commands
    Core::Command* runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
    Core::Command* runProjectTestsCommand = Core::ActionManager::command(Core::Id(Constants::TestRunActionId).withSuffix(mProject->projectFilePath().toString()).withSuffix(mProject->activeTarget()->id().toString()));
    QVERIFY(runProjectTestsCommand == NULL);

    // Enter the required context
    Core::Context projectTreeContext(ProjectExplorer::Constants::C_PROJECT_TREE);
    Core::ICore::addAdditionalContext(projectTreeContext);

    // Check actions are enabled
    QVERIFY(!runTestsCommand->action()->isEnabled());

    // Check action text is right
    QCOMPARE(runTestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject->displayName()).arg(mProject->activeTarget()->displayName()));
}

void QMakeMakeCheckTest::openProject(const QString& projectFilePath)
{
    BEGIN_SUB_TEST_FUNCTION

    ProjectExplorer::ProjectExplorerPlugin::OpenProjectResult result = ProjectExplorer::ProjectExplorerPlugin::openProject(projectFilePath);
    QVERIFY((bool) result);
    mProject = result.project();
    if (mProject->activeTarget() == NULL) {
        foreach (ProjectExplorer::Kit* kit, ProjectExplorer::KitManager::kits())
            mProject->addTarget(new ProjectExplorer::Target(mProject, kit));
        foreach (ProjectExplorer::Target* target, mProject->targets()) {
            if (target->kit() == ProjectExplorer::KitManager::defaultKit())
                ProjectExplorer::SessionManager::setActiveTarget(mProject, target, ProjectExplorer::SetActive::Cascade);
        }
    }
    QVERIFY(mProject->activeTarget() != NULL);
    if (mProject->activeTarget()->activeBuildConfiguration() == NULL) {
        ProjectExplorer::IBuildConfigurationFactory* factory = ProjectExplorer::IBuildConfigurationFactory::find(mProject->activeTarget());
        QVERIFY(factory != NULL);
        QList<ProjectExplorer::BuildInfo *> buildInfos = factory->availableBuilds(mProject->activeTarget());
        ProjectExplorer::BuildInfo* releaseBuildInfo = NULL;
        ProjectExplorer::BuildInfo* debugBuildInfo = NULL;
        foreach (ProjectExplorer::BuildInfo* bi, buildInfos) {
            if (QString::compare(bi->typeName, QLatin1String("Release"), Qt::CaseInsensitive) == 0) {
                releaseBuildInfo = bi;
                releaseBuildInfo->displayName = bi->typeName;
            } else if (QString::compare(bi->typeName, QLatin1String("Release"), Qt::CaseInsensitive) == 0) {
                debugBuildInfo = bi;
                debugBuildInfo->displayName = bi->typeName;
            }
        }
        QVERIFY((releaseBuildInfo != NULL) || (debugBuildInfo != NULL));
        if (debugBuildInfo != NULL)
            mProject->activeTarget()->addBuildConfiguration(factory->create(mProject->activeTarget(), debugBuildInfo));
        else
            mProject->activeTarget()->addBuildConfiguration(factory->create(mProject->activeTarget(), releaseBuildInfo));
    }
    QVERIFY(mProject->activeTarget()->activeBuildConfiguration() != NULL);

    END_SUB_TEST_FUNCTION
}

} // Test
} // QTestLibPlugin
