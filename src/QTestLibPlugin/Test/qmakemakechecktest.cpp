#include "qmakemakechecktest.h"

#include "../qtestlibpluginconstants.h"

#include <projectexplorer/projectexplorer.h>

#include <coreplugin/actionmanager/actionmanager.h>

#include <QtTest>

namespace QTestLibPlugin {
namespace Test {

QMakeMakeCheckTest::QMakeMakeCheckTest(void):
    QObject(NULL)
{}

void QMakeMakeCheckTest::testOpenProjectWithTests_data(void)
{
    QTest::addColumn<QString>("projectPath");

    QTest::newRow("OneSubTest") << TESTS_DIR "/OneSubTest/OneSubTest.pro";
    QTest::newRow("TwoSubTests") << TESTS_DIR "/TwoSubTests/TwoSubTests.pro";
}

void QMakeMakeCheckTest::testOpenProjectWithTests(void)
{
    QFETCH(QString, projectPath);

    Core::Command* runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);

    ProjectExplorer::ProjectExplorerPlugin::openProject(projectPath);
}

void QMakeMakeCheckTest::testOpenProjectWithoutTests_data(void)
{
    QTest::addColumn<QString>("projectPath");

    QTest::newRow("NoSubTestOne") << TESTS_DIR "/NoSubTestOne";
    QTest::newRow("NoSubTestTwo") << TESTS_DIR "/NoSubTestTwo";
}

void QMakeMakeCheckTest::testOpenProjectWithoutTests(void)
{
    QFETCH(QString, projectPath);

    Core::Command* runTestsCommand = Core::ActionManager::command(Constants::TestRunActionId);
    QVERIFY(runTestsCommand != NULL);
}

} // Test
} // QTestLibPlugin
