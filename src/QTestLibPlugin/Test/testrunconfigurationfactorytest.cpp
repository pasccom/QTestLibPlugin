#include "testrunconfigurationfactorytest.h"
#include "testhelper.h"

#include "../testrunconfiguration.h"
#include "../qtestlibpluginconstants.h"

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/session.h>
#include <projectexplorer/target.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/buildinfo.h>
#include <projectexplorer/toolchain.h>

#include <qmakeprojectmanager/qmakeproject.h>

#include <QtTest>

namespace QTestLibPlugin {
namespace Test {

TestRunConfigurationFactoryTest::TestRunConfigurationFactoryTest(void):
    QObject(NULL), mProject(NULL)
{
}

void TestRunConfigurationFactoryTest::initTestCase(void)
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

void TestRunConfigurationFactoryTest::init(void)
{
    mProject = NULL;
}

void TestRunConfigurationFactoryTest::cleanup(void)
{
    if (mProject != NULL)
        ProjectExplorer::SessionManager::removeProject(mProject);
}

void TestRunConfigurationFactoryTest::testOpenProjectWithTests_data(void)
{
    QTest::addColumn<QString>("projectPath");

    QTest::newRow("OneSubTest") << TESTS_DIR "/OneSubTest/OneSubTest.pro";
    QTest::newRow("TwoSubTests") << TESTS_DIR "/TwoSubTests/TwoSubTests.pro";
}

void TestRunConfigurationFactoryTest::testOpenProjectWithTests(void)
{
    QFETCH(QString, projectPath);

    QVERIFY(openQMakeProject(projectPath, &mProject));
    QCOMPARE(mProject->projectFilePath().toString(), projectPath);

    foreach (ProjectExplorer::Target* target, mProject->targets()) {
        Internal::TestRunConfiguration* testRunConfig = NULL;
        foreach (ProjectExplorer::RunConfiguration* runConfig, target->runConfigurations()) {
            if (testRunConfig != NULL)
                QVERIFY(runConfig->id() != Core::Id(Constants::TestRunConfigurationId));
            else
                testRunConfig = qobject_cast<Internal::TestRunConfiguration*>(runConfig);
        }
        QVERIFY(testRunConfig != NULL);
        QVERIFY(testRunConfig->id() == Core::Id(Constants::TestRunConfigurationId));

        Utils::Environment env = target->activeBuildConfiguration()->environment();
        ProjectExplorer::ToolChain *toolChain = ProjectExplorer::ToolChainKitInformation::toolChain(target->kit());
        QCOMPARE(testRunConfig->executable(), toolChain->makeCommand(env));
        QCOMPARE(testRunConfig->commandLineArguments(), QLatin1String("check"));
    }
}

void TestRunConfigurationFactoryTest::testOpenProjectWithoutTests_data(void)
{
    QTest::addColumn<QString>("projectPath");

    QTest::newRow("NoSubTestOne") << TESTS_DIR "/NoSubTestOne/NoSubTestOne.pro";
    QTest::newRow("NoSubTestTwo") << TESTS_DIR "/NoSubTestTwo/NoSubTestTwo.pro";
}

void TestRunConfigurationFactoryTest::testOpenProjectWithoutTests(void)
{
    QFETCH(QString, projectPath);

    QVERIFY(openQMakeProject(projectPath, &mProject));
    QCOMPARE(mProject->projectFilePath().toString(), projectPath);

    foreach (ProjectExplorer::Target* target, mProject->targets()) {
        foreach (ProjectExplorer::RunConfiguration* runConfig, target->runConfigurations())
            QVERIFY(runConfig->id() != Core::Id(Constants::TestRunConfigurationId));
     }
}

} // Test
} // QTestLibPlugin
