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

    SUB_TEST_FUNCTION(openProject(projectPath));
    QCOMPARE(mProject->projectFilePath().toString(), projectPath);
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject, true, true));
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject, true));
    SUB_TEST_FUNCTION(checkSubMenu(1));

    SUB_TEST_FUNCTION(closeProject());
    SUB_TEST_FUNCTION(checkSubMenuAction(projectPath));
    SUB_TEST_FUNCTION(checkContextMenuAction(NULL, false));
    SUB_TEST_FUNCTION(checkSubMenu(0));
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

    SUB_TEST_FUNCTION(openProject(projectPath));
    QCOMPARE(mProject->projectFilePath().toString(), projectPath);
    SUB_TEST_FUNCTION(checkSubMenuAction(mProject, false, false));
    SUB_TEST_FUNCTION(checkContextMenuAction(mProject, false));
    SUB_TEST_FUNCTION(checkSubMenu(1));

    SUB_TEST_FUNCTION(closeProject());
    SUB_TEST_FUNCTION(checkSubMenuAction(projectPath));
    SUB_TEST_FUNCTION(checkContextMenuAction(NULL, false));
    SUB_TEST_FUNCTION(checkSubMenu(0));
}

void QMakeMakeCheckTest::testChangeTarget(void)
{
    QFETCH(QString, projectPath);

    SUB_TEST_FUNCTION(openProject(projectPath));
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

    SUB_TEST_FUNCTION(openProject(project1FilePath, 1));
    QCOMPARE(mProject1->projectFilePath().toString(), project1FilePath);
    SUB_TEST_FUNCTION(openProject(project2FilePath, 2));
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

    SUB_TEST_FUNCTION(openProject(project1FilePath, 1));
    QCOMPARE(mProject1->projectFilePath().toString(), project1FilePath);
    SUB_TEST_FUNCTION(openProject(project2FilePath, 2));
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

    SUB_TEST_FUNCTION(openProject(project1FilePath, 1));
    QCOMPARE(mProject1->projectFilePath().toString(), project1FilePath);
    SUB_TEST_FUNCTION(openProject(project2FilePath, 2));
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

    SUB_TEST_FUNCTION(openProject(project1FilePath, 1));
    QCOMPARE(mProject1->projectFilePath().toString(), project1FilePath);
    SUB_TEST_FUNCTION(openProject(project2FilePath, 2));
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

void QMakeMakeCheckTest::closeProject(int number)
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

void QMakeMakeCheckTest::checkSubMenuAction(const QString& projectPath)
{
    BEGIN_SUB_TEST_FUNCTION

    Core::Id runProjectTestsCommandId(Constants::TestRunActionId);
    runProjectTestsCommandId = runProjectTestsCommandId.withSuffix(projectPath);

    Core::Command* runProjectTestsCommand = Core::ActionManager::command(runProjectTestsCommandId);
    QVERIFY(runProjectTestsCommand == NULL);

    END_SUB_TEST_FUNCTION
}

void QMakeMakeCheckTest::checkSubMenuAction(ProjectExplorer::Project* project, bool present, bool enabled)
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

    if (enabled) {
        QSignalSpy runControlStartedSpy(ProjectExplorer::ProjectExplorerPlugin::instance(), SIGNAL(runControlStarted(ProjectExplorer::RunControl*)));
        runProjectTestsCommand->action()->trigger();

        QCOMPARE(runControlStartedSpy.size(), 1);
        qDebug() << runControlStartedSpy.at(0);
        ProjectExplorer::RunControl* runControl = runControlStartedSpy.at(0).at(0).value<ProjectExplorer::RunControl*>();
        ProjectExplorer::LocalApplicationRunConfiguration* runConfig = qobject_cast<ProjectExplorer::LocalApplicationRunConfiguration*>(runControl->runConfiguration());
        QVERIFY(runConfig != NULL);
        // TODO test make?
        QVERIFY(runConfig->commandLineArguments().startsWith(QLatin1String("check")));

        QSignalSpy runControlStoppedSpy(runControl, SIGNAL(finished()));
        runControl->stop();
        QCOMPARE(runControlStoppedSpy.size(), 1);
    }

    END_SUB_TEST_FUNCTION
}

void QMakeMakeCheckTest::checkContextMenuAction(ProjectExplorer::Project* project, bool enabled)
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

    if (enabled) {
        QSignalSpy runControlStartedSpy(ProjectExplorer::ProjectExplorerPlugin::instance(), SIGNAL(runControlStarted(ProjectExplorer::RunControl*)));
        runTestsCommand->action()->trigger();

        QCOMPARE(runControlStartedSpy.size(), 1);
        qDebug() << runControlStartedSpy.at(0);
        ProjectExplorer::RunControl* runControl = runControlStartedSpy.at(0).at(0).value<ProjectExplorer::RunControl*>();
        ProjectExplorer::LocalApplicationRunConfiguration* runConfig = qobject_cast<ProjectExplorer::LocalApplicationRunConfiguration*>(runControl->runConfiguration());
        QVERIFY(runConfig != NULL);
        // TODO test make?
        QVERIFY(runConfig->commandLineArguments().startsWith(QLatin1String("check")));

        QSignalSpy runControlStoppedSpy(runControl, SIGNAL(finished()));
        runControl->stop();
        QCOMPARE(runControlStoppedSpy.size(), 1);
    }

    Core::ICore::removeAdditionalContext(projectTreeContext);
    END_SUB_TEST_FUNCTION
}

void QMakeMakeCheckTest::checkSubMenu(int actionCount)
{
    BEGIN_SUB_TEST_FUNCTION

    Core::ActionContainer* runTestsMenu = Core::ActionManager::actionContainer(Constants::TestRunMenuId);
    QVERIFY(runTestsMenu != NULL);
    QVERIFY(runTestsMenu->menu() != NULL);
    QCOMPARE(runTestsMenu->menu()->actions().size(), actionCount);

    END_SUB_TEST_FUNCTION
}

void QMakeMakeCheckTest::setCurrentProjectTree(ProjectExplorer::Project* project)
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

} // Test
} // QTestLibPlugin
