#include "testrunconfigurationfactorytest.h"

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

    SUB_TEST_FUNCTION(openProject(projectPath));
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

        QtcUtils::Environment env = target->activeBuildConfiguration()->environment();
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

    SUB_TEST_FUNCTION(openProject(projectPath));
    QCOMPARE(mProject->projectFilePath().toString(), projectPath);

    foreach (ProjectExplorer::Target* target, mProject->targets()) {
        foreach (ProjectExplorer::RunConfiguration* runConfig, target->runConfigurations())
            QVERIFY(runConfig->id() != Core::Id(Constants::TestRunConfigurationId));
     }
}

void TestRunConfigurationFactoryTest::openProject(const QString& projectFilePath)
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

    END_SUB_TEST_FUNCTION
}

} // Test
} // QTestLibPlugin
