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
    QObject(NULL), mProject(NULL), mProject1(NULL), mProject2(NULL)
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
    mProject1 = NULL;
    mProject2 = NULL;
}

void QMakeMakeCheckTest::cleanup(void)
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

    // Sub menu action id:
    Core::Id runProjectTestsCommandId(Constants::TestRunActionId);
    runProjectTestsCommandId = runProjectTestsCommandId.withSuffix(mProject->projectFilePath().toString());

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

    // Sub menu action id:
    Core::Id runProjectTestsCommandId(Constants::TestRunActionId);
    runProjectTestsCommandId = runProjectTestsCommandId.withSuffix(mProject->projectFilePath().toString());

    // Check context menu action:
    runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
    Core::ICore::addAdditionalContext(projectTreeContext);
    QVERIFY(!runTestsCommand->action()->isEnabled());
    QCOMPARE(runTestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject->displayName()).arg(mProject->activeTarget()->displayName()));
    Core::ICore::removeAdditionalContext(projectTreeContext);

    // Check submenu action:
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

void QMakeMakeCheckTest::testChangeTarget(void)
{
    QFETCH(QString, projectPath);

    Core::Command* runTestsCommand;
    Core::Command* runProjectTestsCommand;
    Core::ActionContainer* runTestsMenu;
    Core::Context projectTreeContext(ProjectExplorer::Constants::C_PROJECT_TREE);

    // Open project
    SUB_TEST_FUNCTION(openProject(projectPath));
    if (mProject->targets().size() < 2)
        QSKIP("This test requires the project to have 2 targets.");

    // Sub menu action id:
    Core::Id runProjectTestsCommandId(Constants::TestRunActionId);
    runProjectTestsCommandId = runProjectTestsCommandId.withSuffix(mProject->projectFilePath().toString());

    foreach (ProjectExplorer::Target* target, mProject->targets()) {
        ProjectExplorer::SessionManager::setActiveTarget(mProject, target, ProjectExplorer::SetActive::Cascade);

        // Check context menu action:
        runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
        QVERIFY(runTestsCommand != NULL);
        Core::ICore::addAdditionalContext(projectTreeContext);
        QVERIFY(runTestsCommand->action()->isEnabled());
        QCOMPARE(runTestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject->displayName()).arg(target->displayName()));
        Core::ICore::removeAdditionalContext(projectTreeContext);

        // Check submenu action:
        runProjectTestsCommand = Core::ActionManager::command(runProjectTestsCommandId);
        QVERIFY(runProjectTestsCommand != NULL);
        QVERIFY(runProjectTestsCommand->action()->isEnabled());
        QCOMPARE(runProjectTestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject->displayName()).arg(target->displayName()));

        // Check menu
        runTestsMenu = Core::ActionManager::actionContainer(Constants::TestRunMenuId);
        QVERIFY(runTestsMenu != NULL);
        QVERIFY(runTestsMenu->menu() != NULL);
        QCOMPARE(runTestsMenu->menu()->actions().size(), 1);
    }

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

void QMakeMakeCheckTest::testTwoProjectsWithTests_data(void)
{
    QTest::addColumn<QString>("project1FilePath");
    QTest::addColumn<QString>("project2FilePath");

    QTest::newRow("OneTwo") << TESTS_DIR "/OneSubTest/OneSubTest.pro" << TESTS_DIR "/TwoSubTests/TwoSubTests.pro";
    QTest::newRow("TwoOne") << TESTS_DIR "/TwoSubTests/TwoSubTests.pro" << TESTS_DIR "/OneSubTest/OneSubTest.pro";
}

void QMakeMakeCheckTest::testTwoProjectsWithTests(void)
{
    QFETCH(QString, project1FilePath);
    QFETCH(QString, project2FilePath);

    Core::Command* runTestsCommand;
    Core::Command* runProject1TestsCommand;
    Core::Command* runProject2TestsCommand;
    Core::ActionContainer* runTestsMenu;
    Core::Context projectTreeContext(ProjectExplorer::Constants::C_PROJECT_TREE);

    // Open projects
    SUB_TEST_FUNCTION(openProject(project1FilePath, 1));
    SUB_TEST_FUNCTION(openProject(project2FilePath, 2));

    // Sub menu action id:
    Core::Id runProject1TestsCommandId(Constants::TestRunActionId);
    runProject1TestsCommandId = runProject1TestsCommandId.withSuffix(mProject1->projectFilePath().toString());
    Core::Id runProject2TestsCommandId(Constants::TestRunActionId);
    runProject2TestsCommandId = runProject2TestsCommandId.withSuffix(mProject2->projectFilePath().toString());

    // Check context menu action:
    ProjectExplorer::ProjectTree::highlightProject(mProject1, QString::null);
    runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
    Core::ICore::addAdditionalContext(projectTreeContext);
    QVERIFY(runTestsCommand->action()->isEnabled());
    QCOMPARE(runTestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject1->displayName()).arg(mProject1->activeTarget()->displayName()));
    Core::ICore::removeAdditionalContext(projectTreeContext);

    ProjectExplorer::ProjectTree::highlightProject(mProject2, QString::null);
    runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
    Core::ICore::addAdditionalContext(projectTreeContext);
    QVERIFY(runTestsCommand->action()->isEnabled());
    QCOMPARE(runTestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject2->displayName()).arg(mProject1->activeTarget()->displayName()));
    Core::ICore::removeAdditionalContext(projectTreeContext);

    // Check submenu actions:
    runProject1TestsCommand = Core::ActionManager::command(runProject1TestsCommandId);
    QVERIFY(runProject1TestsCommand != NULL);
    QVERIFY(runProject1TestsCommand->action()->isEnabled());
    QCOMPARE(runProject1TestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject1->displayName()).arg(mProject1->activeTarget()->displayName()));
    runProject2TestsCommand = Core::ActionManager::command(runProject2TestsCommandId);
    QVERIFY(runProject2TestsCommand != NULL);
    QVERIFY(runProject2TestsCommand->action()->isEnabled());
    QCOMPARE(runProject2TestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject2->displayName()).arg(mProject2->activeTarget()->displayName()));

    // Check menu is enabled
    runTestsMenu = Core::ActionManager::actionContainer(Constants::TestRunMenuId);
    QVERIFY(runTestsMenu != NULL);
    QVERIFY(runTestsMenu->menu() != NULL);
    QCOMPARE(runTestsMenu->menu()->actions().size(), 2);

    // Close project2:
    ProjectExplorer::SessionManager::removeProject(mProject2);
    mProject2 = NULL;

    // Check context menu action:
    runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
    Core::ICore::addAdditionalContext(projectTreeContext);
    QVERIFY(runTestsCommand->action()->isEnabled());
    QCOMPARE(runTestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject1->displayName()).arg(mProject1->activeTarget()->displayName()));
    Core::ICore::removeAdditionalContext(projectTreeContext);

    // Re check submenu action:
    runProject1TestsCommand = Core::ActionManager::command(runProject1TestsCommandId);
    QVERIFY(runProject1TestsCommand != NULL);
    QVERIFY(runProject1TestsCommand->action()->isEnabled());
    QCOMPARE(runProject1TestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject1->displayName()).arg(mProject1->activeTarget()->displayName()));
    runProject2TestsCommand = Core::ActionManager::command(runProject2TestsCommandId);
    QVERIFY(runProject2TestsCommand == NULL);

    // Re check menu
    runTestsMenu = Core::ActionManager::actionContainer(Constants::TestRunMenuId);
    QVERIFY(runTestsMenu != NULL);
    QVERIFY(runTestsMenu->menu() != NULL);
    QCOMPARE(runTestsMenu->menu()->actions().size(), 1);

    // Close project1:
    ProjectExplorer::SessionManager::removeProject(mProject1);
    mProject1 = NULL;

    // Check context menu action:
    runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
    Core::ICore::addAdditionalContext(projectTreeContext);
    QVERIFY(!runTestsCommand->action()->isEnabled());
    Core::ICore::removeAdditionalContext(projectTreeContext);

    // Re check submenu action:
    runProject1TestsCommand = Core::ActionManager::command(runProject1TestsCommandId);
    QVERIFY(runProject1TestsCommand == NULL);
    runProject2TestsCommand = Core::ActionManager::command(runProject2TestsCommandId);
    QVERIFY(runProject2TestsCommand == NULL);

    // Re check menu
    runTestsMenu = Core::ActionManager::actionContainer(Constants::TestRunMenuId);
    QVERIFY(runTestsMenu != NULL);
    QVERIFY(runTestsMenu->menu() != NULL);
    QCOMPARE(runTestsMenu->menu()->actions().size(), 0);
}

void QMakeMakeCheckTest::testTwoProjectsWithAndWithoutTests_data(void)
{
    QTest::addColumn<QString>("project1FilePath");
    QTest::addColumn<QString>("project2FilePath");

    QTest::newRow("OneTwo") << TESTS_DIR "/OneSubTest/OneSubTest.pro" << TESTS_DIR "/NoSubTestTwo/NoSubTestTwo.pro";
    QTest::newRow("TwoOne") << TESTS_DIR "/TwoSubTests/TwoSubTests.pro" << TESTS_DIR "/NoSubTestOne/NoSubTestOne.pro";
}

void QMakeMakeCheckTest::testTwoProjectsWithAndWithoutTests(void)
{
    QFETCH(QString, project1FilePath);
    QFETCH(QString, project2FilePath);

    Core::Command* runTestsCommand;
    Core::Command* runProject1TestsCommand;
    Core::Command* runProject2TestsCommand;
    Core::ActionContainer* runTestsMenu;
    Core::Context projectTreeContext(ProjectExplorer::Constants::C_PROJECT_TREE);

    // Open projects
    SUB_TEST_FUNCTION(openProject(project1FilePath, 1));
    SUB_TEST_FUNCTION(openProject(project2FilePath, 2));

    // Sub menu action id:
    Core::Id runProject1TestsCommandId(Constants::TestRunActionId);
    runProject1TestsCommandId = runProject1TestsCommandId.withSuffix(mProject1->projectFilePath().toString());
    Core::Id runProject2TestsCommandId(Constants::TestRunActionId);
    runProject2TestsCommandId = runProject2TestsCommandId.withSuffix(mProject2->projectFilePath().toString());

    // Check context menu action:
    ProjectExplorer::ProjectTree::highlightProject(mProject1, QString::null);
    runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
    Core::ICore::addAdditionalContext(projectTreeContext);
    QVERIFY(runTestsCommand->action()->isEnabled());
    QCOMPARE(runTestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject1->displayName()).arg(mProject1->activeTarget()->displayName()));
    Core::ICore::removeAdditionalContext(projectTreeContext);

    ProjectExplorer::ProjectTree::highlightProject(mProject2, QString::null);
    runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
    Core::ICore::addAdditionalContext(projectTreeContext);
    QVERIFY(!runTestsCommand->action()->isEnabled());
    QCOMPARE(runTestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject2->displayName()).arg(mProject1->activeTarget()->displayName()));
    Core::ICore::removeAdditionalContext(projectTreeContext);

    // Check submenu actions:
    runProject1TestsCommand = Core::ActionManager::command(runProject1TestsCommandId);
    QVERIFY(runProject1TestsCommand != NULL);
    QVERIFY(runProject1TestsCommand->action()->isEnabled());
    QCOMPARE(runProject1TestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject1->displayName()).arg(mProject1->activeTarget()->displayName()));
    runProject2TestsCommand = Core::ActionManager::command(runProject2TestsCommandId);
    QVERIFY(runProject2TestsCommand == NULL);

    // Check menu is enabled
    runTestsMenu = Core::ActionManager::actionContainer(Constants::TestRunMenuId);
    QVERIFY(runTestsMenu != NULL);
    QVERIFY(runTestsMenu->menu() != NULL);
    QCOMPARE(runTestsMenu->menu()->actions().size(), 1);

    // Close project2:
    ProjectExplorer::SessionManager::removeProject(mProject2);
    mProject2 = NULL;

    // Check context menu action:
    runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
    Core::ICore::addAdditionalContext(projectTreeContext);
    QVERIFY(runTestsCommand->action()->isEnabled());
    QCOMPARE(runTestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject1->displayName()).arg(mProject1->activeTarget()->displayName()));
    Core::ICore::removeAdditionalContext(projectTreeContext);

    // Re check submenu action:
    runProject1TestsCommand = Core::ActionManager::command(runProject1TestsCommandId);
    QVERIFY(runProject1TestsCommand != NULL);
    QVERIFY(runProject1TestsCommand->action()->isEnabled());
    QCOMPARE(runProject1TestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject1->displayName()).arg(mProject1->activeTarget()->displayName()));
    runProject2TestsCommand = Core::ActionManager::command(runProject2TestsCommandId);
    QVERIFY(runProject2TestsCommand == NULL);

    // Re check menu
    runTestsMenu = Core::ActionManager::actionContainer(Constants::TestRunMenuId);
    QVERIFY(runTestsMenu != NULL);
    QVERIFY(runTestsMenu->menu() != NULL);
    QCOMPARE(runTestsMenu->menu()->actions().size(), 1);

    // Close project1:
    ProjectExplorer::SessionManager::removeProject(mProject1);
    mProject1 = NULL;

    // Check context menu action:
    runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
    Core::ICore::addAdditionalContext(projectTreeContext);
    QVERIFY(!runTestsCommand->action()->isEnabled());
    Core::ICore::removeAdditionalContext(projectTreeContext);

    // Re check submenu action:
    runProject1TestsCommand = Core::ActionManager::command(runProject1TestsCommandId);
    QVERIFY(runProject1TestsCommand == NULL);
    runProject2TestsCommand = Core::ActionManager::command(runProject2TestsCommandId);
    QVERIFY(runProject2TestsCommand == NULL);

    // Re check menu
    runTestsMenu = Core::ActionManager::actionContainer(Constants::TestRunMenuId);
    QVERIFY(runTestsMenu != NULL);
    QVERIFY(runTestsMenu->menu() != NULL);
    QCOMPARE(runTestsMenu->menu()->actions().size(), 0);
}

void QMakeMakeCheckTest::testTwoProjectsWithoutAndWithTests_data(void)
{
    QTest::addColumn<QString>("project1FilePath");
    QTest::addColumn<QString>("project2FilePath");

    QTest::newRow("OneTwo") << TESTS_DIR "/NoSubTestOne/NoSubTestOne.pro" << TESTS_DIR "/TwoSubTests/TwoSubTests.pro";
    QTest::newRow("TwoOne") << TESTS_DIR "/NoSubTestTwo/NoSubTestTwo.pro" << TESTS_DIR "/OneSubTest/OneSubTest.pro";
}

void QMakeMakeCheckTest::testTwoProjectsWithoutAndWithTests(void)
{
    QFETCH(QString, project1FilePath);
    QFETCH(QString, project2FilePath);

    Core::Command* runTestsCommand;
    Core::Command* runProject1TestsCommand;
    Core::Command* runProject2TestsCommand;
    Core::ActionContainer* runTestsMenu;
    Core::Context projectTreeContext(ProjectExplorer::Constants::C_PROJECT_TREE);

    // Open projects
    SUB_TEST_FUNCTION(openProject(project1FilePath, 1));
    SUB_TEST_FUNCTION(openProject(project2FilePath, 2));

    // Sub menu action id:
    Core::Id runProject1TestsCommandId(Constants::TestRunActionId);
    runProject1TestsCommandId = runProject1TestsCommandId.withSuffix(mProject1->projectFilePath().toString());
    Core::Id runProject2TestsCommandId(Constants::TestRunActionId);
    runProject2TestsCommandId = runProject2TestsCommandId.withSuffix(mProject2->projectFilePath().toString());

    // Check context menu action:
    ProjectExplorer::ProjectTree::highlightProject(mProject1, QString::null);
    runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
    Core::ICore::addAdditionalContext(projectTreeContext);
    QVERIFY(!runTestsCommand->action()->isEnabled());
    QCOMPARE(runTestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject1->displayName()).arg(mProject1->activeTarget()->displayName()));
    Core::ICore::removeAdditionalContext(projectTreeContext);

    ProjectExplorer::ProjectTree::highlightProject(mProject2, QString::null);
    runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
    Core::ICore::addAdditionalContext(projectTreeContext);
    QVERIFY(runTestsCommand->action()->isEnabled());
    QCOMPARE(runTestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject2->displayName()).arg(mProject1->activeTarget()->displayName()));
    Core::ICore::removeAdditionalContext(projectTreeContext);

    // Check submenu actions:
    runProject1TestsCommand = Core::ActionManager::command(runProject1TestsCommandId);
    QVERIFY(runProject1TestsCommand == NULL);
    runProject2TestsCommand = Core::ActionManager::command(runProject2TestsCommandId);
    QVERIFY(runProject2TestsCommand != NULL);
    QVERIFY(runProject2TestsCommand->action()->isEnabled());
    QCOMPARE(runProject2TestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject2->displayName()).arg(mProject2->activeTarget()->displayName()));

    // Check menu is enabled
    runTestsMenu = Core::ActionManager::actionContainer(Constants::TestRunMenuId);
    QVERIFY(runTestsMenu != NULL);
    QVERIFY(runTestsMenu->menu() != NULL);
    QCOMPARE(runTestsMenu->menu()->actions().size(), 1);

    // Close project2:
    ProjectExplorer::SessionManager::removeProject(mProject2);
    mProject2 = NULL;

    // Check context menu action:
    runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
    Core::ICore::addAdditionalContext(projectTreeContext);
    QVERIFY(!runTestsCommand->action()->isEnabled());
    QCOMPARE(runTestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject1->displayName()).arg(mProject1->activeTarget()->displayName()));
    Core::ICore::removeAdditionalContext(projectTreeContext);

    // Re check submenu action:
    runProject1TestsCommand = Core::ActionManager::command(runProject1TestsCommandId);
    QVERIFY(runProject1TestsCommand == NULL);
    runProject2TestsCommand = Core::ActionManager::command(runProject2TestsCommandId);
    QVERIFY(runProject2TestsCommand == NULL);

    // Re check menu
    runTestsMenu = Core::ActionManager::actionContainer(Constants::TestRunMenuId);
    QVERIFY(runTestsMenu != NULL);
    QVERIFY(runTestsMenu->menu() != NULL);
    QCOMPARE(runTestsMenu->menu()->actions().size(), 0);

    // Close project1:
    ProjectExplorer::SessionManager::removeProject(mProject1);
    mProject1 = NULL;

    // Check context menu action:
    runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
    Core::ICore::addAdditionalContext(projectTreeContext);
    QVERIFY(!runTestsCommand->action()->isEnabled());
    Core::ICore::removeAdditionalContext(projectTreeContext);

    // Re check submenu action:
    runProject1TestsCommand = Core::ActionManager::command(runProject1TestsCommandId);
    QVERIFY(runProject1TestsCommand == NULL);
    runProject2TestsCommand = Core::ActionManager::command(runProject2TestsCommandId);
    QVERIFY(runProject2TestsCommand == NULL);

    // Re check menu
    runTestsMenu = Core::ActionManager::actionContainer(Constants::TestRunMenuId);
    QVERIFY(runTestsMenu != NULL);
    QVERIFY(runTestsMenu->menu() != NULL);
    QCOMPARE(runTestsMenu->menu()->actions().size(), 0);
}

void QMakeMakeCheckTest::testTwoProjectsWithoutTests_data(void)
{
    QTest::addColumn<QString>("project1FilePath");
    QTest::addColumn<QString>("project2FilePath");

    QTest::newRow("OneTwo") << TESTS_DIR "/NoSubTestOne/NoSubTestOne.pro" << TESTS_DIR "/NoSubTestTwo/NoSubTestTwo.pro";
    QTest::newRow("TwoOne") << TESTS_DIR "/NoSubTestTwo/NoSubTestTwo.pro" << TESTS_DIR "/NoSubTestOne/NoSubTestOne.pro";
}

void QMakeMakeCheckTest::testTwoProjectsWithoutTests(void)
{
    QFETCH(QString, project1FilePath);
    QFETCH(QString, project2FilePath);

    Core::Command* runTestsCommand;
    Core::Command* runProject1TestsCommand;
    Core::Command* runProject2TestsCommand;
    Core::ActionContainer* runTestsMenu;
    Core::Context projectTreeContext(ProjectExplorer::Constants::C_PROJECT_TREE);

    // Open projects
    SUB_TEST_FUNCTION(openProject(project1FilePath, 1));
    SUB_TEST_FUNCTION(openProject(project2FilePath, 2));

    // Sub menu action id:
    Core::Id runProject1TestsCommandId(Constants::TestRunActionId);
    runProject1TestsCommandId = runProject1TestsCommandId.withSuffix(mProject1->projectFilePath().toString());
    Core::Id runProject2TestsCommandId(Constants::TestRunActionId);
    runProject2TestsCommandId = runProject2TestsCommandId.withSuffix(mProject2->projectFilePath().toString());

    // Check context menu action:
    ProjectExplorer::ProjectTree::highlightProject(mProject1, QString::null);
    runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
    Core::ICore::addAdditionalContext(projectTreeContext);
    QVERIFY(!runTestsCommand->action()->isEnabled());
    QCOMPARE(runTestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject1->displayName()).arg(mProject1->activeTarget()->displayName()));
    Core::ICore::removeAdditionalContext(projectTreeContext);

    ProjectExplorer::ProjectTree::highlightProject(mProject2, QString::null);
    runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
    Core::ICore::addAdditionalContext(projectTreeContext);
    QVERIFY(!runTestsCommand->action()->isEnabled());
    QCOMPARE(runTestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject2->displayName()).arg(mProject1->activeTarget()->displayName()));
    Core::ICore::removeAdditionalContext(projectTreeContext);

    // Check submenu actions:
    runProject1TestsCommand = Core::ActionManager::command(runProject1TestsCommandId);
    QVERIFY(runProject1TestsCommand == NULL);
    runProject2TestsCommand = Core::ActionManager::command(runProject2TestsCommandId);
    QVERIFY(runProject2TestsCommand == NULL);

    // Check menu is enabled
    runTestsMenu = Core::ActionManager::actionContainer(Constants::TestRunMenuId);
    QVERIFY(runTestsMenu != NULL);
    QVERIFY(runTestsMenu->menu() != NULL);
    QCOMPARE(runTestsMenu->menu()->actions().size(), 0);

    // Close project2:
    ProjectExplorer::SessionManager::removeProject(mProject2);
    mProject2 = NULL;

    // Check context menu action:
    runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
    Core::ICore::addAdditionalContext(projectTreeContext);
    QVERIFY(!runTestsCommand->action()->isEnabled());
    QCOMPARE(runTestsCommand->action()->text(), tr("Run tests for \"%1\" (%2)").arg(mProject1->displayName()).arg(mProject1->activeTarget()->displayName()));
    Core::ICore::removeAdditionalContext(projectTreeContext);

    // Re check submenu action:
    runProject1TestsCommand = Core::ActionManager::command(runProject1TestsCommandId);
    QVERIFY(runProject1TestsCommand == NULL);
    runProject2TestsCommand = Core::ActionManager::command(runProject2TestsCommandId);
    QVERIFY(runProject2TestsCommand == NULL);

    // Re check menu
    runTestsMenu = Core::ActionManager::actionContainer(Constants::TestRunMenuId);
    QVERIFY(runTestsMenu != NULL);
    QVERIFY(runTestsMenu->menu() != NULL);
    QCOMPARE(runTestsMenu->menu()->actions().size(), 0);

    // Close project1:
    ProjectExplorer::SessionManager::removeProject(mProject1);
    mProject1 = NULL;

    // Check context menu action:
    runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
    Core::ICore::addAdditionalContext(projectTreeContext);
    QVERIFY(!runTestsCommand->action()->isEnabled());
    Core::ICore::removeAdditionalContext(projectTreeContext);

    // Re check submenu action:
    runProject1TestsCommand = Core::ActionManager::command(runProject1TestsCommandId);
    QVERIFY(runProject1TestsCommand == NULL);
    runProject2TestsCommand = Core::ActionManager::command(runProject2TestsCommandId);
    QVERIFY(runProject2TestsCommand == NULL);

    // Re check menu
    runTestsMenu = Core::ActionManager::actionContainer(Constants::TestRunMenuId);
    QVERIFY(runTestsMenu != NULL);
    QVERIFY(runTestsMenu->menu() != NULL);
    QCOMPARE(runTestsMenu->menu()->actions().size(), 0);
}

void QMakeMakeCheckTest::openProject(const QString& projectFilePath, int number)
{
    BEGIN_SUB_TEST_FUNCTION

    // Open project
    ProjectExplorer::ProjectExplorerPlugin::OpenProjectResult result = ProjectExplorer::ProjectExplorerPlugin::openProject(projectFilePath);
    QVERIFY((bool) result);
    mProject = result.project();

    // Initialize targets if required
    if (mProject->activeTarget() == NULL) {
        foreach (ProjectExplorer::Kit* kit, ProjectExplorer::KitManager::kits())
            mProject->addTarget(new ProjectExplorer::Target(mProject, kit));
        foreach (ProjectExplorer::Target* target, mProject->targets()) {
            if (target->kit() == ProjectExplorer::KitManager::defaultKit())
                ProjectExplorer::SessionManager::setActiveTarget(mProject, target, ProjectExplorer::SetActive::Cascade);
        }
    }
    QVERIFY(mProject->activeTarget() != NULL);

    // Initialize build confirgurations if required
    foreach (ProjectExplorer::Target* target, mProject->targets()) {
        if (target->activeBuildConfiguration() == NULL) {
            ProjectExplorer::IBuildConfigurationFactory* factory = ProjectExplorer::IBuildConfigurationFactory::find(target);
            QVERIFY(factory != NULL);
            QList<ProjectExplorer::BuildInfo *> buildInfos = factory->availableBuilds(target);
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
                target->addBuildConfiguration(factory->create(target, debugBuildInfo));
            else
                target->addBuildConfiguration(factory->create(target, releaseBuildInfo));
        }
    }
    QVERIFY(mProject->activeTarget()->activeBuildConfiguration() != NULL);

    // Wait for project evaluated
    QmakeProjectManager::QmakeProject* qMakeProject = qobject_cast<QmakeProjectManager::QmakeProject*>(mProject);
    QVERIFY(qMakeProject != NULL);
    QSignalSpy evaluateSpy(qMakeProject, SIGNAL(proFilesEvaluated()));
    evaluateSpy.wait();

    // Update targets:
    foreach (ProjectExplorer::Target* target, mProject->targets())
        target->updateDefaultRunConfigurations();

    switch (number) {
    case 1:
        mProject1 = mProject;
        mProject = NULL;
        break;
    case 2:
        mProject2 = mProject;
        mProject = NULL;
        break;
    default:
        break;
    }

    END_SUB_TEST_FUNCTION
}

} // Test
} // QTestLibPlugin
