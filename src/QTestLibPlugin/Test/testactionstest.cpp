#include "testactionstest.h"
#include "testhelper.h"

#include "../qtestlibpluginconstants.h"

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projecttree.h>
#include <projectexplorer/session.h>
#include <projectexplorer/target.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/buildinfo.h>
#include <projectexplorer/toolchain.h>
#include <projectexplorer/localapplicationrunconfiguration.h>

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

TestActionsTest::TestActionsTest(void):
    QObject(NULL), mProject(NULL), mProject1(NULL), mProject2(NULL)
{
}

void TestActionsTest::initTestCase(void)
{
    QStringList projectPathes;

    // NOTE _data() function is not available for initTestCase()
    projectPathes << QLatin1String(TESTS_DIR "/OneSubTest");
    projectPathes << QLatin1String(TESTS_DIR "/TwoSubTests");
    projectPathes << QLatin1String(TESTS_DIR "/NoSubTestOne");
    projectPathes << QLatin1String(TESTS_DIR "/NoSubTestTwo");

    foreach (QString projectPath, projectPathes)
        QVERIFY(removeProjectUserFiles(projectPath));
}

void TestActionsTest::init(void)
{
    mProject = NULL;
    mProject1 = NULL;
    mProject2 = NULL;
}

void TestActionsTest::cleanup(void)
{
    if (mProject != NULL)
        ProjectExplorer::SessionManager::removeProject(mProject);
    if (mProject1 != NULL)
        ProjectExplorer::SessionManager::removeProject(mProject1);
    if (mProject2 != NULL)
        ProjectExplorer::SessionManager::removeProject(mProject2);

    Core::Context projectTreeContext(ProjectExplorer::Constants::C_PROJECT_TREE);
    Core::ICore::removeAdditionalContext(projectTreeContext);
}

void TestActionsTest::testOpenProjectWithTests_data(void)
{
    QTest::addColumn<QString>("projectPath");

    QTest::newRow("OneSubTest") << TESTS_DIR "/OneSubTest/OneSubTest.pro";
    QTest::newRow("TwoSubTests") << TESTS_DIR "/TwoSubTests/TwoSubTests.pro";
}

void TestActionsTest::testOpenProjectWithTests(void)
{
    QFETCH(QString, projectPath);

    QVERIFY(openQMakeProject(projectPath, &mProject));
    QCOMPARE(mProject->projectFilePath().toString(), projectPath);
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject, true, true));
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject, true));
    SUB_TEST_FUNCTION(checkSubMenu(1));

    SUB_TEST_FUNCTION(closeProject());
    SUB_TEST_FUNCTION(checkSubMenuAction(projectPath));
    SUB_TEST_FUNCTION(checkContextMenuAction(NULL, false));
    SUB_TEST_FUNCTION(checkSubMenu(0));
}

void TestActionsTest::testOpenProjectWithoutTests_data(void)
{
    QTest::addColumn<QString>("projectPath");

    QTest::newRow("NoSubTestOne") << TESTS_DIR "/NoSubTestOne/NoSubTestOne.pro";
    QTest::newRow("NoSubTestTwo") << TESTS_DIR "/NoSubTestTwo/NoSubTestTwo.pro";
}

void TestActionsTest::testOpenProjectWithoutTests(void)
{
    QFETCH(QString, projectPath);

    QVERIFY(openQMakeProject(projectPath, &mProject));
    QCOMPARE(mProject->projectFilePath().toString(), projectPath);
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject, false, false));
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject, false));
    SUB_TEST_FUNCTION(checkSubMenu(1));

    SUB_TEST_FUNCTION(closeProject());
    SUB_TEST_FUNCTION(checkSubMenuAction(projectPath));
    SUB_TEST_FUNCTION(checkContextMenuAction(NULL, false));
    SUB_TEST_FUNCTION(checkSubMenu(0));
}

void TestActionsTest::testChangeTarget(void)
{
    QFETCH(QString, projectPath);

    QVERIFY(openQMakeProject(projectPath, &mProject));
    QCOMPARE(mProject->projectFilePath().toString(), projectPath);
    if (mProject->targets().size() < 2)
        QSKIP("This test requires the project to have 2 targets.");

    foreach (ProjectExplorer::Target* target, mProject->targets()) {
        ProjectExplorer::SessionManager::setActiveTarget(mProject, target, ProjectExplorer::SetActive::Cascade);
        SUB_TEST_FUNCTION(checkSubMenuAction(mProject, true, true));
        SUB_TEST_FUNCTION(checkContextMenuAction(mProject, true));
        SUB_TEST_FUNCTION(checkSubMenu(1));
    }

    SUB_TEST_FUNCTION(closeProject());
    SUB_TEST_FUNCTION(checkSubMenuAction(projectPath));
    SUB_TEST_FUNCTION(checkContextMenuAction(NULL, false));
    SUB_TEST_FUNCTION(checkSubMenu(0));
}

void TestActionsTest::testTwoProjectsWithTests_data(void)
{
    QTest::addColumn<QString>("project1FilePath");
    QTest::addColumn<QString>("project2FilePath");

    QTest::newRow("OneTwo") << TESTS_DIR "/OneSubTest/OneSubTest.pro" << TESTS_DIR "/TwoSubTests/TwoSubTests.pro";
    QTest::newRow("TwoOne") << TESTS_DIR "/TwoSubTests/TwoSubTests.pro" << TESTS_DIR "/OneSubTest/OneSubTest.pro";
}

void TestActionsTest::testTwoProjectsWithTests(void)
{
    QFETCH(QString, project1FilePath);
    QFETCH(QString, project2FilePath);

    QVERIFY(openQMakeProject(project1FilePath, &mProject1));
    QCOMPARE(mProject1->projectFilePath().toString(), project1FilePath);
    QVERIFY(openQMakeProject(project2FilePath, &mProject2));
    QCOMPARE(mProject2->projectFilePath().toString(), project2FilePath);
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject1, true, true));
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject2, true, true));
    SUB_TEST_FUNCTION(setCurrentProjectTree(mProject1));
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject1, true));
    SUB_TEST_FUNCTION(setCurrentProjectTree(mProject2));
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject2, true));
    SUB_TEST_FUNCTION(checkSubMenu(2));

    SUB_TEST_FUNCTION(closeProject(2));
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject1, true, true));
    SUB_TEST_FUNCTION(checkSubMenuAction(project2FilePath));
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject1, true));
    SUB_TEST_FUNCTION(checkSubMenu(1));

    SUB_TEST_FUNCTION(closeProject(1));
    SUB_TEST_FUNCTION(checkSubMenuAction(project1FilePath));
    SUB_TEST_FUNCTION(checkSubMenuAction(project2FilePath));
    SUB_TEST_FUNCTION(checkContextMenuAction(NULL, false));
    SUB_TEST_FUNCTION(checkSubMenu(0));
}

void TestActionsTest::testTwoProjectsWithAndWithoutTests_data(void)
{
    QTest::addColumn<QString>("project1FilePath");
    QTest::addColumn<QString>("project2FilePath");

    QTest::newRow("OneTwo") << TESTS_DIR "/OneSubTest/OneSubTest.pro" << TESTS_DIR "/NoSubTestTwo/NoSubTestTwo.pro";
    QTest::newRow("TwoOne") << TESTS_DIR "/TwoSubTests/TwoSubTests.pro" << TESTS_DIR "/NoSubTestOne/NoSubTestOne.pro";
}

void TestActionsTest::testTwoProjectsWithAndWithoutTests(void)
{
    QFETCH(QString, project1FilePath);
    QFETCH(QString, project2FilePath);

    QVERIFY(openQMakeProject(project1FilePath, &mProject1));
    QCOMPARE(mProject1->projectFilePath().toString(), project1FilePath);
    QVERIFY(openQMakeProject(project2FilePath, &mProject2));
    QCOMPARE(mProject2->projectFilePath().toString(), project2FilePath);
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject1, true, true));
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject2, false, false));
    SUB_TEST_FUNCTION(setCurrentProjectTree(mProject1));
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject1, true));
    SUB_TEST_FUNCTION(setCurrentProjectTree(mProject2));
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject2, false));
    SUB_TEST_FUNCTION(checkSubMenu(1));

    SUB_TEST_FUNCTION(closeProject(2));
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject1, true, true));
    SUB_TEST_FUNCTION(checkSubMenuAction(project2FilePath));
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject1, true));
    SUB_TEST_FUNCTION(checkSubMenu(1));

    SUB_TEST_FUNCTION(closeProject(1));
    SUB_TEST_FUNCTION(checkSubMenuAction(project1FilePath));
    SUB_TEST_FUNCTION(checkSubMenuAction(project2FilePath));
    SUB_TEST_FUNCTION(checkContextMenuAction(NULL, false));
    SUB_TEST_FUNCTION(checkSubMenu(0));
}

void TestActionsTest::testTwoProjectsWithoutAndWithTests_data(void)
{
    QTest::addColumn<QString>("project1FilePath");
    QTest::addColumn<QString>("project2FilePath");

    QTest::newRow("OneTwo") << TESTS_DIR "/NoSubTestOne/NoSubTestOne.pro" << TESTS_DIR "/TwoSubTests/TwoSubTests.pro";
    QTest::newRow("TwoOne") << TESTS_DIR "/NoSubTestTwo/NoSubTestTwo.pro" << TESTS_DIR "/OneSubTest/OneSubTest.pro";
}

void TestActionsTest::testTwoProjectsWithoutAndWithTests(void)
{
    QFETCH(QString, project1FilePath);
    QFETCH(QString, project2FilePath);

    QVERIFY(openQMakeProject(project1FilePath, &mProject1));
    QCOMPARE(mProject1->projectFilePath().toString(), project1FilePath);
    QVERIFY(openQMakeProject(project2FilePath, &mProject2));
    QCOMPARE(mProject2->projectFilePath().toString(), project2FilePath);
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject1, false, false));
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject2, true, true));
    SUB_TEST_FUNCTION(setCurrentProjectTree(mProject1));
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject1, false));
    SUB_TEST_FUNCTION(setCurrentProjectTree(mProject2));
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject2, true));
    SUB_TEST_FUNCTION(checkSubMenu(1));

    SUB_TEST_FUNCTION(closeProject(2));
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject1, false, false));
    SUB_TEST_FUNCTION(checkSubMenuAction(project2FilePath));
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject1, false));
    SUB_TEST_FUNCTION(checkSubMenu(0));

    SUB_TEST_FUNCTION(closeProject(1));
    SUB_TEST_FUNCTION(checkSubMenuAction(project1FilePath));
    SUB_TEST_FUNCTION(checkSubMenuAction(project2FilePath));
    SUB_TEST_FUNCTION(checkContextMenuAction(NULL, false));
    SUB_TEST_FUNCTION(checkSubMenu(0));
}

void TestActionsTest::testTwoProjectsWithoutTests_data(void)
{
    QTest::addColumn<QString>("project1FilePath");
    QTest::addColumn<QString>("project2FilePath");

    QTest::newRow("OneTwo") << TESTS_DIR "/NoSubTestOne/NoSubTestOne.pro" << TESTS_DIR "/NoSubTestTwo/NoSubTestTwo.pro";
    QTest::newRow("TwoOne") << TESTS_DIR "/NoSubTestTwo/NoSubTestTwo.pro" << TESTS_DIR "/NoSubTestOne/NoSubTestOne.pro";
}

void TestActionsTest::testTwoProjectsWithoutTests(void)
{
    QFETCH(QString, project1FilePath);
    QFETCH(QString, project2FilePath);

    QVERIFY(openQMakeProject(project1FilePath, &mProject1));
    QCOMPARE(mProject1->projectFilePath().toString(), project1FilePath);
    QVERIFY(openQMakeProject(project2FilePath, &mProject2));
    QCOMPARE(mProject2->projectFilePath().toString(), project2FilePath);
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject1, false, false));
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject2, false, false));
    SUB_TEST_FUNCTION(setCurrentProjectTree(mProject1));
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject1, false));
    SUB_TEST_FUNCTION(setCurrentProjectTree(mProject2));
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject2, false));
    SUB_TEST_FUNCTION(checkSubMenu(0));

    SUB_TEST_FUNCTION(closeProject(2));
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject1, false, false));
    SUB_TEST_FUNCTION(checkSubMenuAction(project2FilePath));
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject1, false));
    SUB_TEST_FUNCTION(checkSubMenu(0));

    SUB_TEST_FUNCTION(closeProject(1));
    SUB_TEST_FUNCTION(checkSubMenuAction(project1FilePath));
    SUB_TEST_FUNCTION(checkSubMenuAction(project2FilePath));
    SUB_TEST_FUNCTION(checkContextMenuAction(NULL, false));
    SUB_TEST_FUNCTION(checkSubMenu(0));
}

void TestActionsTest::closeProject(int number)
{
    BEGIN_SUB_TEST_FUNCTION

    switch (number) {
    case 1:
        QVERIFY(mProject1 != NULL);
        ProjectExplorer::SessionManager::removeProject(mProject1);
        mProject1 = NULL;
        break;
    case 2:
        QVERIFY(mProject2 != NULL);
        ProjectExplorer::SessionManager::removeProject(mProject2);
        mProject2 = NULL;
        break;
    default:
        QVERIFY(mProject != NULL);
        ProjectExplorer::SessionManager::removeProject(mProject);
        mProject = NULL;
        break;
    }

    END_SUB_TEST_FUNCTION
}

void TestActionsTest::checkSubMenuAction(const QString& projectPath)
{
    BEGIN_SUB_TEST_FUNCTION

    Core::Id runProjectTestsCommandId(Constants::TestRunActionId);
    runProjectTestsCommandId = runProjectTestsCommandId.withSuffix(projectPath);

    Core::Command* runProjectTestsCommand = Core::ActionManager::command(runProjectTestsCommandId);
    QVERIFY(runProjectTestsCommand == NULL);

    END_SUB_TEST_FUNCTION
}

void TestActionsTest::checkSubMenuAction(ProjectExplorer::Project* project, bool present, bool enabled)
{
    BEGIN_SUB_TEST_FUNCTION

    Core::Id runProjectTestsCommandId(Constants::TestRunActionId);
    runProjectTestsCommandId = runProjectTestsCommandId.withSuffix(project->projectFilePath().toString());

    Core::Command* runProjectTestsCommand = Core::ActionManager::command(runProjectTestsCommandId);
    QCOMPARE(runProjectTestsCommand != NULL, present);
    if (!present)
        return;
    QCOMPARE(runProjectTestsCommand->action()->isEnabled(), enabled);
    QCOMPARE(runProjectTestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(project->displayName()).arg(project->activeTarget()->displayName()));

    if (enabled)
        SUB_TEST_FUNCTION(runMakeCheck(project, runProjectTestsCommand->action()));

    END_SUB_TEST_FUNCTION
}

void TestActionsTest::checkContextMenuAction(ProjectExplorer::Project* project, bool enabled)
{
    BEGIN_SUB_TEST_FUNCTION

    Core::Context projectTreeContext(ProjectExplorer::Constants::C_PROJECT_TREE);

    Core::Command* runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
    Core::ICore::addAdditionalContext(projectTreeContext);
    QCOMPARE(runTestsCommand->action()->isEnabled(), enabled);
    if (project != NULL)
        QCOMPARE(runTestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(project->displayName()).arg(project->activeTarget()->displayName()));
    else
        QCOMPARE(runTestsCommand->action()->text(), tr("Run tests"));

    if (enabled)
        runMakeCheck(project, runTestsCommand->action());


    Core::ICore::removeAdditionalContext(projectTreeContext);
    END_SUB_TEST_FUNCTION
}

void TestActionsTest::checkSubMenu(int actionCount)
{
    BEGIN_SUB_TEST_FUNCTION

    Core::ActionContainer* runTestsMenu = Core::ActionManager::actionContainer(Constants::TestRunMenuId);
    QVERIFY(runTestsMenu != NULL);
    QVERIFY(runTestsMenu->menu() != NULL);
    QCOMPARE(runTestsMenu->menu()->actions().size(), actionCount);

    END_SUB_TEST_FUNCTION
}

void TestActionsTest::setCurrentProjectTree(ProjectExplorer::Project* project)
{
    BEGIN_SUB_TEST_FUNCTION

    QSignalSpy currentProjectChangedSpy(ProjectExplorer::ProjectTree::instance(), SIGNAL(currentProjectChanged(ProjectExplorer::Project*)));
    ProjectExplorer::ProjectTree::highlightProject(project, QString::null);
    if (currentProjectChangedSpy.size() == 0)
        currentProjectChangedSpy.wait();
    QVERIFY(currentProjectChangedSpy.size() > 0);
    ProjectExplorer::Project* currentProject = currentProjectChangedSpy.last().at(0).value<ProjectExplorer::Project*>();
    QVERIFY(currentProject == project);

    END_SUB_TEST_FUNCTION
}

void TestActionsTest::runMakeCheck(ProjectExplorer::Project* project, QAction* runControlAction)
{
    BEGIN_SUB_TEST_FUNCTION

    QSignalSpy runControlStartedSpy(ProjectExplorer::ProjectExplorerPlugin::instance(), SIGNAL(runControlStarted(ProjectExplorer::RunControl*)));
    runControlAction->trigger();

    QCOMPARE(runControlStartedSpy.size(), 1);
    qDebug() << runControlStartedSpy.at(0);
    Utils::Environment env = project->activeTarget()->activeBuildConfiguration()->environment();
    ProjectExplorer::ToolChain *toolChain = ProjectExplorer::ToolChainKitInformation::toolChain(project->activeTarget()->kit());
    ProjectExplorer::RunControl* runControl = runControlStartedSpy.at(0).at(0).value<ProjectExplorer::RunControl*>();
    ProjectExplorer::LocalApplicationRunConfiguration* runConfig = qobject_cast<ProjectExplorer::LocalApplicationRunConfiguration*>(runControl->runConfiguration());
    QVERIFY(runConfig != NULL);
    QCOMPARE(runConfig->executable(), toolChain->makeCommand(env));
    QCOMPARE(runConfig->commandLineArguments(), QLatin1String("check"));

    QSignalSpy runControlStoppedSpy(runControl, SIGNAL(finished()));
    runControl->stop();
    QCOMPARE(runControlStoppedSpy.size(), 1);

    END_SUB_TEST_FUNCTION
}

} // Test
} // QTestLibPlugin
