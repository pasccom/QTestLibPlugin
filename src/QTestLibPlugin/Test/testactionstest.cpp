/* Copyright 2016-2020 Pascal COMBES <pascom@orange.fr>
 * 
 * This file is part of QTestLibPlugin.
 * 
 * QTestLibPlugin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * QTestLibPlugin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with QTestLibPlugin. If not, see <http://www.gnu.org/licenses/>
 */

#include "testactionstest.h"
#include "testhelper.h"

#include "../qtestlibpluginconstants.h"

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projecttree.h>
#include <projectexplorer/target.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/buildinfo.h>
#include <projectexplorer/toolchain.h>
#include <projectexplorer/toolchainkitaspect.h>
#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/runcontrol.h>

#include <qmakeprojectmanager/qmakeproject.h>

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/icore.h>

#include <utils/proxyaction.h>
#include <utils/processinterface.h>

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
    Utils::FilePaths projectPathes;

    // NOTE _data() function is not available for initTestCase()
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "OneSubTest");
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "TwoSubTests");
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "NoSubTestOne");
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "NoSubTestTwo");

    foreach (Utils::FilePath projectPath, projectPathes)
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
    if (mProject != nullptr)
        QVERIFY(closeProject(mProject));
    if (mProject1 != nullptr)
        QVERIFY(closeProject(mProject1));
    if (mProject2 != nullptr)
        QVERIFY(closeProject(mProject2));

    Core::Context projectTreeContext(ProjectExplorer::Constants::C_PROJECT_TREE);
    Core::ICore::removeAdditionalContext(projectTreeContext);
}

void TestActionsTest::testOpenProjectWithTests_data(void)
{
    QTest::addColumn<Utils::FilePath>("projectPath");

    QTest::newRow("OneSubTest") << Utils::FilePath::fromString(TESTS_DIR "OneSubTest/OneSubTest.pro");
    QTest::newRow("TwoSubTests") << Utils::FilePath::fromString(TESTS_DIR "TwoSubTests/TwoSubTests.pro");
}

void TestActionsTest::testOpenProjectWithTests(void)
{
    QFETCH(Utils::FilePath, projectPath);

    QVERIFY(openQMakeProject(projectPath, &mProject));
    QCOMPARE(mProject->projectFilePath(), projectPath);
    QApplication::processEvents();
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject, true, true));
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject, true));
    SUB_TEST_FUNCTION(checkSubMenu(1));

    SUB_TEST_FUNCTION(cleanupProject());
    QApplication::processEvents();
    SUB_TEST_FUNCTION(checkSubMenuAction(projectPath));
    SUB_TEST_FUNCTION(checkContextMenuAction(NULL, false));
    SUB_TEST_FUNCTION(checkSubMenu(0));
}

void TestActionsTest::testOpenProjectWithoutTests_data(void)
{
    QTest::addColumn<Utils::FilePath>("projectPath");

    QTest::newRow("NoSubTestOne") << Utils::FilePath::fromString(TESTS_DIR "NoSubTestOne/NoSubTestOne.pro");
    QTest::newRow("NoSubTestTwo") << Utils::FilePath::fromString(TESTS_DIR "NoSubTestTwo/NoSubTestTwo.pro");
}

void TestActionsTest::testOpenProjectWithoutTests(void)
{
    QFETCH(Utils::FilePath, projectPath);

    QVERIFY(openQMakeProject(projectPath, &mProject));
    QCOMPARE(mProject->projectFilePath(), projectPath);
    QApplication::processEvents();

    SUB_TEST_FUNCTION(checkSubMenuAction(mProject, false, false));
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject, false));
    SUB_TEST_FUNCTION(checkSubMenu(0));

    SUB_TEST_FUNCTION(cleanupProject());
    QApplication::processEvents();
    SUB_TEST_FUNCTION(checkSubMenuAction(projectPath));
    SUB_TEST_FUNCTION(checkContextMenuAction(NULL, false));
    SUB_TEST_FUNCTION(checkSubMenu(0));
}

void TestActionsTest::testChangeTarget(void)
{
    QFETCH(Utils::FilePath, projectPath);

    QVERIFY(openQMakeProject(projectPath, &mProject));
    QCOMPARE(mProject->projectFilePath(), projectPath);
    if (mProject->targets().size() < 2)
        QSKIP("This test requires the project to have 2 targets.");

    foreach (ProjectExplorer::Target* target, mProject->targets()) {
        mProject->setActiveTarget(target, ProjectExplorer::SetActive::Cascade);
        QApplication::processEvents();
        if (mProject->activeTarget()->buildSystem()->isParsing()) {
            QSignalSpy parsingFinishedSpy(mProject->activeTarget(), SIGNAL(parsingFinished(bool)));
            parsingFinishedSpy.wait(5000);
        }
        SUB_TEST_FUNCTION(checkSubMenuAction(mProject, true, true));
        SUB_TEST_FUNCTION(checkContextMenuAction(mProject, true));
        SUB_TEST_FUNCTION(checkSubMenu(1));
    }

    SUB_TEST_FUNCTION(cleanupProject());
    QApplication::processEvents();
    SUB_TEST_FUNCTION(checkSubMenuAction(projectPath));
    SUB_TEST_FUNCTION(checkContextMenuAction(NULL, false));
    SUB_TEST_FUNCTION(checkSubMenu(0));
}

void TestActionsTest::testTwoProjectsWithTests_data(void)
{
    QTest::addColumn<Utils::FilePath>("project1FilePath");
    QTest::addColumn<Utils::FilePath>("project2FilePath");

    QTest::newRow("OneTwo") << Utils::FilePath::fromString(TESTS_DIR "OneSubTest/OneSubTest.pro") << Utils::FilePath::fromString(TESTS_DIR "TwoSubTests/TwoSubTests.pro");
    QTest::newRow("TwoOne") << Utils::FilePath::fromString(TESTS_DIR "TwoSubTests/TwoSubTests.pro") << Utils::FilePath::fromString(TESTS_DIR "OneSubTest/OneSubTest.pro");
}

void TestActionsTest::testTwoProjectsWithTests(void)
{
    QFETCH(Utils::FilePath, project1FilePath);
    QFETCH(Utils::FilePath, project2FilePath);

    QVERIFY(openQMakeProject(project1FilePath, &mProject1));
    QCOMPARE(mProject1->projectFilePath(), project1FilePath);
    QVERIFY(openQMakeProject(project2FilePath, &mProject2));
    QCOMPARE(mProject2->projectFilePath(), project2FilePath);
    QApplication::processEvents();
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject1, true, true));
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject2, true, true));
    SUB_TEST_FUNCTION(setCurrentProjectTree(mProject1));
    QApplication::processEvents();
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject1, true));
    SUB_TEST_FUNCTION(setCurrentProjectTree(mProject2));
    QApplication::processEvents();
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject2, true));
    SUB_TEST_FUNCTION(checkSubMenu(2));

    SUB_TEST_FUNCTION(cleanupProject(2));
    QApplication::processEvents();
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject1, true, true));
    SUB_TEST_FUNCTION(checkSubMenuAction(project2FilePath));
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject1, true));
    SUB_TEST_FUNCTION(checkSubMenu(1));

    SUB_TEST_FUNCTION(cleanupProject(1));
    QApplication::processEvents();
    SUB_TEST_FUNCTION(checkSubMenuAction(project1FilePath));
    SUB_TEST_FUNCTION(checkSubMenuAction(project2FilePath));
    SUB_TEST_FUNCTION(checkContextMenuAction(NULL, false));
    SUB_TEST_FUNCTION(checkSubMenu(0));
}

void TestActionsTest::testTwoProjectsWithAndWithoutTests_data(void)
{
    QTest::addColumn<Utils::FilePath>("project1FilePath");
    QTest::addColumn<Utils::FilePath>("project2FilePath");

    QTest::newRow("OneTwo") << Utils::FilePath::fromString(TESTS_DIR "OneSubTest/OneSubTest.pro") << Utils::FilePath::fromString(TESTS_DIR "NoSubTestTwo/NoSubTestTwo.pro");
    QTest::newRow("TwoOne") << Utils::FilePath::fromString(TESTS_DIR "TwoSubTests/TwoSubTests.pro") << Utils::FilePath::fromString(TESTS_DIR "NoSubTestOne/NoSubTestOne.pro");
}

void TestActionsTest::testTwoProjectsWithAndWithoutTests(void)
{
    QFETCH(Utils::FilePath, project1FilePath);
    QFETCH(Utils::FilePath, project2FilePath);

    QVERIFY(openQMakeProject(project1FilePath, &mProject1));
    QCOMPARE(mProject1->projectFilePath(), project1FilePath);
    QVERIFY(openQMakeProject(project2FilePath, &mProject2));
    QCOMPARE(mProject2->projectFilePath(), project2FilePath);
    QApplication::processEvents();
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject1, true, true));
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject2, false, false));
    SUB_TEST_FUNCTION(setCurrentProjectTree(mProject1));
    QApplication::processEvents();
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject1, true));
    SUB_TEST_FUNCTION(setCurrentProjectTree(mProject2));
    QApplication::processEvents();
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject2, false));
    SUB_TEST_FUNCTION(checkSubMenu(1));

    SUB_TEST_FUNCTION(cleanupProject(2));
    QApplication::processEvents();
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject1, true, true));
    SUB_TEST_FUNCTION(checkSubMenuAction(project2FilePath));
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject1, true));
    SUB_TEST_FUNCTION(checkSubMenu(1));

    SUB_TEST_FUNCTION(cleanupProject(1));
    QApplication::processEvents();
    SUB_TEST_FUNCTION(checkSubMenuAction(project1FilePath));
    SUB_TEST_FUNCTION(checkSubMenuAction(project2FilePath));
    SUB_TEST_FUNCTION(checkContextMenuAction(NULL, false));
    SUB_TEST_FUNCTION(checkSubMenu(0));
}

void TestActionsTest::testTwoProjectsWithoutAndWithTests_data(void)
{
    QTest::addColumn<Utils::FilePath>("project1FilePath");
    QTest::addColumn<Utils::FilePath>("project2FilePath");

    QTest::newRow("OneTwo") << Utils::FilePath::fromString(TESTS_DIR "NoSubTestOne/NoSubTestOne.pro") << Utils::FilePath::fromString(TESTS_DIR "TwoSubTests/TwoSubTests.pro");
    QTest::newRow("TwoOne") << Utils::FilePath::fromString(TESTS_DIR "NoSubTestTwo/NoSubTestTwo.pro") << Utils::FilePath::fromString(TESTS_DIR "OneSubTest/OneSubTest.pro");
}

void TestActionsTest::testTwoProjectsWithoutAndWithTests(void)
{
    QFETCH(Utils::FilePath, project1FilePath);
    QFETCH(Utils::FilePath, project2FilePath);

    QVERIFY(openQMakeProject(project1FilePath, &mProject1));
    QCOMPARE(mProject1->projectFilePath(), project1FilePath);
    QVERIFY(openQMakeProject(project2FilePath, &mProject2));
    QCOMPARE(mProject2->projectFilePath(), project2FilePath);
    QApplication::processEvents();
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject1, false, false));
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject2, true, true));
    SUB_TEST_FUNCTION(setCurrentProjectTree(mProject1));
    QApplication::processEvents();
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject1, false));
    SUB_TEST_FUNCTION(setCurrentProjectTree(mProject2));
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject2, true));
    SUB_TEST_FUNCTION(checkSubMenu(1));

    SUB_TEST_FUNCTION(cleanupProject(2));
    QApplication::processEvents();
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject1, false, false));
    SUB_TEST_FUNCTION(checkSubMenuAction(project2FilePath));
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject1, false));
    SUB_TEST_FUNCTION(checkSubMenu(0));

    SUB_TEST_FUNCTION(cleanupProject(1));
    QApplication::processEvents();
    SUB_TEST_FUNCTION(checkSubMenuAction(project1FilePath));
    SUB_TEST_FUNCTION(checkSubMenuAction(project2FilePath));
    SUB_TEST_FUNCTION(checkContextMenuAction(NULL, false));
    SUB_TEST_FUNCTION(checkSubMenu(0));
}

void TestActionsTest::testTwoProjectsWithoutTests_data(void)
{
    QTest::addColumn<Utils::FilePath>("project1FilePath");
    QTest::addColumn<Utils::FilePath>("project2FilePath");

    QTest::newRow("OneTwo") << Utils::FilePath::fromString(TESTS_DIR "NoSubTestOne/NoSubTestOne.pro") << Utils::FilePath::fromString(TESTS_DIR "NoSubTestTwo/NoSubTestTwo.pro");
    QTest::newRow("TwoOne") << Utils::FilePath::fromString(TESTS_DIR "NoSubTestTwo/NoSubTestTwo.pro") << Utils::FilePath::fromString(TESTS_DIR "NoSubTestOne/NoSubTestOne.pro");
}

void TestActionsTest::testTwoProjectsWithoutTests(void)
{
    QFETCH(Utils::FilePath, project1FilePath);
    QFETCH(Utils::FilePath, project2FilePath);

    QVERIFY(openQMakeProject(project1FilePath, &mProject1));
    QCOMPARE(mProject1->projectFilePath(), project1FilePath);
    QVERIFY(openQMakeProject(project2FilePath, &mProject2));
    QCOMPARE(mProject2->projectFilePath(), project2FilePath);
    QApplication::processEvents();
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject1, false, false));
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject2, false, false));
    SUB_TEST_FUNCTION(setCurrentProjectTree(mProject1));
    QApplication::processEvents();
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject1, false));
    SUB_TEST_FUNCTION(setCurrentProjectTree(mProject2));
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject2, false));
    SUB_TEST_FUNCTION(checkSubMenu(0));

    SUB_TEST_FUNCTION(cleanupProject(2));
    QApplication::processEvents();
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject1, false, false));
    SUB_TEST_FUNCTION(checkSubMenuAction(project2FilePath));
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject1, false));
    SUB_TEST_FUNCTION(checkSubMenu(0));

    SUB_TEST_FUNCTION(cleanupProject(1));
    QApplication::processEvents();
    SUB_TEST_FUNCTION(checkSubMenuAction(project1FilePath));
    SUB_TEST_FUNCTION(checkSubMenuAction(project2FilePath));
    SUB_TEST_FUNCTION(checkContextMenuAction(NULL, false));
    SUB_TEST_FUNCTION(checkSubMenu(0));
}

void TestActionsTest::cleanupProject(int number)
{
    BEGIN_SUB_TEST_FUNCTION

    switch (number) {
    case 1:
        QVERIFY(mProject1 != NULL);
        closeProject(mProject1);
        mProject1 = NULL;
        break;
    case 2:
        QVERIFY(mProject2 != NULL);
        closeProject(mProject2);
        mProject2 = NULL;
        break;
    default:
        QVERIFY(mProject != NULL);
        closeProject(mProject);
        mProject = NULL;
        break;
    }

    END_SUB_TEST_FUNCTION
}

void TestActionsTest::checkSubMenuAction(const Utils::FilePath& projectPath)
{
    BEGIN_SUB_TEST_FUNCTION

    Utils::Id runProjectTestsCommandId(Constants::TestRunActionId);
    runProjectTestsCommandId = runProjectTestsCommandId.withSuffix(projectPath.toFSPathString());

    Core::Command* runProjectTestsCommand = Core::ActionManager::command(runProjectTestsCommandId);
    QVERIFY(runProjectTestsCommand == NULL);

    END_SUB_TEST_FUNCTION
}

void TestActionsTest::checkSubMenuAction(ProjectExplorer::Project* project, bool present, bool enabled)
{
    BEGIN_SUB_TEST_FUNCTION

    Utils::Id runProjectTestsCommandId(Constants::TestRunActionId);
    runProjectTestsCommandId = runProjectTestsCommandId.withSuffix(project->projectFilePath().toFSPathString());

    Core::Command* runProjectTestsCommand = Core::ActionManager::command(runProjectTestsCommandId);
    QCOMPARE(runProjectTestsCommand != NULL, present);

    if (present) {
        QCOMPARE(runProjectTestsCommand->action()->isEnabled(), enabled);
        QCOMPARE(runProjectTestsCommand->action()->text().replace('&', ""), QString("Run tests for \"%1\" (%2)").arg(project->displayName()).arg(project->activeTarget()->displayName()));

        if (enabled)
            SUB_TEST_FUNCTION(runMakeCheck(project, runProjectTestsCommand->action()));
    }

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

    Q_UNUSED(project);
    SUB_TEST_FUNCTION_SKIP("Could not change the current project programatically any more.");

    QSignalSpy currentProjectChangedSpy(ProjectExplorer::ProjectTree::instance(), SIGNAL(currentProjectChanged(ProjectExplorer::Project*)));
    ProjectExplorer::ProjectTree::highlightProject(project, QString());
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

    QSignalSpy runControlStartedSpy(ProjectExplorer::ProjectExplorerPlugin::instance(), SIGNAL(aboutToExecuteRunControl(ProjectExplorer::RunControl*, Utils::Id)));
    runControlAction->trigger();

    QCOMPARE(runControlStartedSpy.size(), 1);
    qDebug() << runControlStartedSpy.at(0);
    Utils::Environment env = project->activeTarget()->activeBuildConfiguration()->environment();
    ProjectExplorer::Toolchain *toolChain = ProjectExplorer::ToolchainKitAspect::toolchain(project->activeTarget()->kit(), ProjectExplorer::Constants::CXX_LANGUAGE_ID);
    ProjectExplorer::RunControl* runControl = runControlStartedSpy.at(0).at(0).value<ProjectExplorer::RunControl*>();
    Utils::ProcessRunData runnable = runControl->runnable();

    QSignalSpy runControlStoppedSpy(runControl, SIGNAL(stopped()));
    //runControl->stop();
    runControl->initiateStop();
    runControlStoppedSpy.wait(1000);
    QCOMPARE(runControlStoppedSpy.size(), 1);

    QCOMPARE(runnable.command.executable(), toolChain->makeCommand(env));
    qDebug() << runnable.command.arguments();
    QVERIFY(runnable.command.splitArguments().size() >= 3);
    QCOMPARE(runnable.command.splitArguments().at(0), QLatin1String("-f"));
    QCOMPARE(runnable.command.splitArguments().at(2), QLatin1String("check"));

    END_SUB_TEST_FUNCTION
}

} // Test
} // QTestLibPlugin
