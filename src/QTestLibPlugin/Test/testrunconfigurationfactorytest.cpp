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

#include "testrunconfigurationfactorytest.h"
#include "testhelper.h"

#include "../testrunconfiguration.h"
#include "../qtestlibpluginconstants.h"

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/target.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/toolchainkitaspect.h>
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/buildinfo.h>
#include <projectexplorer/toolchain.h>
#include <projectexplorer/runcontrol.h>

#include <qmakeprojectmanager/qmakeproject.h>

#include <utils/processinterface.h>

#include <QtTest>

namespace QTestLibPlugin {
namespace Test {

TestRunConfigurationFactoryTest::TestRunConfigurationFactoryTest(void):
    QObject(NULL), mProject(NULL)
{
}

void TestRunConfigurationFactoryTest::initTestCase(void)
{
    Utils::FilePaths projectPathes;

    // NOTE _data() function is not available for initTestCase()
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "/OneSubTest");
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "/TwoSubTests");
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "/MakefileTest");
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "/MakefileSpaceTest");
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "/NoSubTestOne");
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "/NoSubTestTwo");

    foreach (Utils::FilePath projectPath, projectPathes)
        QVERIFY(removeProjectUserFiles(projectPath));

    // NOTE First time ProjectExplorer::ProjectExplorerPlugin::openProject()
    // immediately calls ProjectExplorer::Target::ParsingFinished() and
    // consequently, openQMakeProject() does not work
    openQMakeProject(Utils::FilePath::fromString(TESTS_DIR "OneClassTest/OneClassTest.pro"), &mProject);
    QVERIFY(closeProject(mProject));
}

void TestRunConfigurationFactoryTest::init(void)
{
    mProject = NULL;
}

void TestRunConfigurationFactoryTest::cleanup(void)
{
    if (mProject != nullptr)
        QVERIFY(closeProject(mProject));
}

void TestRunConfigurationFactoryTest::testOpenProjectWithTests_data(void)
{
    QTest::addColumn<Utils::FilePath>("projectPath");
    QTest::addColumn<QString>("makefile");

    QTest::newRow("OneSubTest") << Utils::FilePath::fromString(TESTS_DIR "OneSubTest/OneSubTest.pro") << TESTS_DIR "OneSubTest/Makefile";
    QTest::newRow("TwoSubTests") << Utils::FilePath::fromString(TESTS_DIR "TwoSubTests/TwoSubTests.pro") << TESTS_DIR "TwoSubTests/Makefile";
    QTest::newRow("MakefileTest") << Utils::FilePath::fromString(TESTS_DIR "MakefileTest/MakefileTest.pro") << TESTS_DIR "MakefileTest/MyMakefile";
    QTest::newRow("MakefileSpaceTest") << Utils::FilePath::fromString(TESTS_DIR "MakefileSpaceTest/MakefileSpaceTest.pro") << TESTS_DIR "MakefileSpaceTest/My Makefile";
}

void TestRunConfigurationFactoryTest::testOpenProjectWithTests(void)
{
    QFETCH(Utils::FilePath, projectPath);
    QFETCH(QString, makefile);

    QVERIFY(openQMakeProject(projectPath, &mProject));
    QCOMPARE(mProject->projectFilePath(), projectPath);

    foreach (ProjectExplorer::Target* target, mProject->targets()) {
        Internal::TestRunConfiguration* testRunConfig = NULL;
        foreach (ProjectExplorer::RunConfiguration* runConfig, target->runConfigurations()) {
            if (testRunConfig != NULL)
                QVERIFY(runConfig->id() != Utils::Id(Constants::TestRunConfigurationId));
            else
                testRunConfig = qobject_cast<Internal::TestRunConfiguration*>(runConfig);
        }
        QVERIFY(testRunConfig != NULL);
        QVERIFY(testRunConfig->id() == Utils::Id(Constants::TestRunConfigurationId));

        Utils::Environment env = target->activeBuildConfiguration()->environment();
        ProjectExplorer::Toolchain *toolChain = ProjectExplorer::ToolchainKitAspect::toolchain(target->kit(), ProjectExplorer::Constants::CXX_LANGUAGE_ID);

        Utils::ProcessRunData runnable = testRunConfig->runnable();
        QCOMPARE(runnable.command.executable(), toolChain->makeCommand(env));
        QVERIFY(runnable.command.splitArguments().size() >= 3);
        QCOMPARE(runnable.command.splitArguments().at(0), QLatin1String("-f"));
        QCOMPARE(runnable.command.splitArguments().at(1), makefile);
        QCOMPARE(runnable.command.splitArguments().at(2), QLatin1String("check"));
    }
}

void TestRunConfigurationFactoryTest::testOpenProjectWithoutTests_data(void)
{
    QTest::addColumn<Utils::FilePath>("projectPath");

    QTest::newRow("NoSubTestOne") << Utils::FilePath::fromString(TESTS_DIR "NoSubTestOne/NoSubTestOne.pro");
    QTest::newRow("NoSubTestTwo") << Utils::FilePath::fromString(TESTS_DIR "NoSubTestTwo/NoSubTestTwo.pro");
}

void TestRunConfigurationFactoryTest::testOpenProjectWithoutTests(void)
{
    QFETCH(Utils::FilePath, projectPath);

    QVERIFY(openQMakeProject(projectPath, &mProject));
    QCOMPARE(mProject->projectFilePath(), projectPath);

    foreach (ProjectExplorer::Target* target, mProject->targets()) {
        foreach (ProjectExplorer::RunConfiguration* runConfig, target->runConfigurations())
            QVERIFY(runConfig->id() != Utils::Id(Constants::TestRunConfigurationId));
     }
}

} // Test
} // QTestLibPlugin
