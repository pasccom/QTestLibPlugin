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
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/icore.h>

#include <utils/proxyaction.h>

#include <QAction>
#include <QMenu>
#include <QtTest>

namespace QTestLibPlugin {
namespace Test {

QMakeMakeCheckTest::QMakeMakeCheckTest(void):
    QObject(NULL), mProject(NULL)
{
}

void QMakeMakeCheckTest::initTestCase(void)
{
    QStringList projectUserPathes;

    // NOTE _data() function is not available for initTestCase()
    projectUserPathes << QLatin1String(TESTS_DIR "/OneSubTest/OneSubTest.pro.user");
    projectUserPathes << QLatin1String(TESTS_DIR "/TwoSubTests/TwoSubTests.pro.user");
    projectUserPathes << QLatin1String(TESTS_DIR "/NoSubTestOne/NoSubTestOne.pro.user");
    projectUserPathes << QLatin1String(TESTS_DIR "/NoSubTestTwo/NoSubTestTwo.pro.user");

    foreach (QString projectUserPath, projectUserPathes) {
        if (QFile::exists(projectUserPath))
            QVERIFY(QFile::remove(projectUserPath));
    }
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

    Core::Command* runTestsCommand;
    Core::Command* runProjectTestsCommand;
    Core::ActionContainer* runTestsMenu;
    Core::Context projectTreeContext(ProjectExplorer::Constants::C_PROJECT_TREE);

    // Open project
    SUB_TEST_FUNCTION(openProject(projectPath));

    // Wait for project evaluated
    QmakeProjectManager::QmakeProject* qMakeProject = qobject_cast<QmakeProjectManager::QmakeProject*>(mProject);
    QVERIFY(qMakeProject != NULL);
    QSignalSpy evaluateSpy(qMakeProject, SIGNAL(proFilesEvaluated()));
    evaluateSpy.wait();

    // Sub menu action id:
    Core::Id runProjectTestsCommandId(Constants::TestRunActionId);
    runProjectTestsCommandId = runProjectTestsCommandId.withSuffix(mProject->projectFilePath().toString());
    runProjectTestsCommandId = runProjectTestsCommandId.withSuffix(mProject->activeTarget()->id().toString());

    // Check context menu action:
    runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
    Core::ICore::addAdditionalContext(projectTreeContext);
    QVERIFY(runTestsCommand->action()->isEnabled());
    QCOMPARE(runTestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject->displayName()).arg(mProject->activeTarget()->displayName()));
    Core::ICore::removeAdditionalContext(projectTreeContext);

    // Check submenu action:
    runProjectTestsCommand = Core::ActionManager::command(runProjectTestsCommandId);
    QVERIFY(runProjectTestsCommand != NULL);
    QVERIFY(runProjectTestsCommand->action()->isEnabled());
    QCOMPARE(runProjectTestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject->displayName()).arg(mProject->activeTarget()->displayName()));

    // Check menu
    runTestsMenu = Core::ActionManager::actionContainer(Constants::TestRunMenuId);
    QVERIFY(runTestsMenu != NULL);
    QVERIFY(runTestsMenu->menu() != NULL);
    QCOMPARE(runTestsMenu->menu()->actions().size(), 1);

    ProjectExplorer::SessionManager::removeProject(mProject);
    mProject = NULL;

    // Check context menu action:
    runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
    Core::ICore::addAdditionalContext(projectTreeContext);
    QVERIFY(!runTestsCommand->action()->isEnabled());
    Core::ICore::removeAdditionalContext(projectTreeContext);

    // Re check submenu action:
    runProjectTestsCommand = Core::ActionManager::command(runProjectTestsCommandId);
    QVERIFY(runProjectTestsCommand == NULL);

    // Re check menu
    runTestsMenu = Core::ActionManager::actionContainer(Constants::TestRunMenuId);
    QVERIFY(runTestsMenu != NULL);
    QVERIFY(runTestsMenu->menu() != NULL);
    QCOMPARE(runTestsMenu->menu()->actions().size(), 0);

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

    Core::Command* runTestsCommand;
    Core::Command* runProjectTestsCommand;
    Core::ActionContainer* runTestsMenu;
    Core::Context projectTreeContext(ProjectExplorer::Constants::C_PROJECT_TREE);

    // Open project
    SUB_TEST_FUNCTION(openProject(projectPath));

    // Wait for project evaluated
    QmakeProjectManager::QmakeProject* qMakeProject = qobject_cast<QmakeProjectManager::QmakeProject*>(mProject);
    QVERIFY(qMakeProject != NULL);
    QSignalSpy evaluateSpy(qMakeProject, SIGNAL(proFilesEvaluated()));
    evaluateSpy.wait();

    // Sub menu action id:
    Core::Id runProjectTestsCommandId(Constants::TestRunActionId);
    runProjectTestsCommandId = runProjectTestsCommandId.withSuffix(mProject->projectFilePath().toString());
    runProjectTestsCommandId = runProjectTestsCommandId.withSuffix(mProject->activeTarget()->id().toString());

    // Check context menu action:
    runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
    Core::ICore::addAdditionalContext(projectTreeContext);
    QVERIFY(!runTestsCommand->action()->isEnabled());
    QCOMPARE(runTestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject->displayName()).arg(mProject->activeTarget()->displayName()));
    Core::ICore::removeAdditionalContext(projectTreeContext);

    // Check submenuc action:
    runProjectTestsCommand = Core::ActionManager::command(runProjectTestsCommandId);
    QVERIFY(runProjectTestsCommand == NULL);

    // Check menu is disabled
    runTestsMenu = Core::ActionManager::actionContainer(Constants::TestRunMenuId);
    QVERIFY(runTestsMenu != NULL);
    QVERIFY(runTestsMenu->menu() != NULL);
    QCOMPARE(runTestsMenu->menu()->actions().size(), 0);

    // Close project:
    ProjectExplorer::SessionManager::removeProject(mProject);
    mProject = NULL;

    // Check context menu action:
    runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
    Core::ICore::addAdditionalContext(projectTreeContext);
    QVERIFY(!runTestsCommand->action()->isEnabled());
    Core::ICore::removeAdditionalContext(projectTreeContext);

    // Re check submenu action:
    runProjectTestsCommand = Core::ActionManager::command(runProjectTestsCommandId);
    QVERIFY(runProjectTestsCommand == NULL);

    // Re check menu
    runTestsMenu = Core::ActionManager::actionContainer(Constants::TestRunMenuId);
    QVERIFY(runTestsMenu != NULL);
    QVERIFY(runTestsMenu->menu() != NULL);
    QCOMPARE(runTestsMenu->menu()->actions().size(), 0);
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
        /*foreach (ProjectExplorer::Target* target, mProject->targets()) {
            if (target->kit() == ProjectExplorer::KitManager::defaultKit())
                ProjectExplorer::SessionManager::setActiveTarget(mProject, target, ProjectExplorer::SetActive::Cascade);
        }*/
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
