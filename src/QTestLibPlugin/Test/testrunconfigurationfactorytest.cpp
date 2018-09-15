/* Copyright 2016 Pascal COMBES <pascom@orange.fr>
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
    projectPathes << QLatin1String(TESTS_DIR "/MakefileTest");
    projectPathes << QLatin1String(TESTS_DIR "/MakefileSpaceTest");
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
    closeProject(mProject);
}

void TestRunConfigurationFactoryTest::testOpenProjectWithTests_data(void)
{
    QTest::addColumn<QString>("projectPath");
    QTest::addColumn<QString>("makefile");

    QTest::newRow("OneSubTest") << TESTS_DIR "/OneSubTest/OneSubTest.pro" << TESTS_DIR "/OneSubTest/Makefile";
    QTest::newRow("TwoSubTests") << TESTS_DIR "/TwoSubTests/TwoSubTests.pro" << TESTS_DIR "/TwoSubTests/Makefile";
    QTest::newRow("MakefileTest") << TESTS_DIR "/MakefileTest/MakefileTest.pro" << TESTS_DIR "/MakefileTest/MyMakefile";
    QTest::newRow("MakefileSpaceTest") << TESTS_DIR "/MakefileSpaceTest/MakefileSpaceTest.pro" << "\"" TESTS_DIR "/MakefileSpaceTest/My Makefile\"";
}

void TestRunConfigurationFactoryTest::testOpenProjectWithTests(void)
{
    QFETCH(QString, projectPath);
    QFETCH(QString, makefile);

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
        ProjectExplorer::ToolChain *toolChain = ProjectExplorer::ToolChainKitInformation::toolChain(target->kit(), ProjectExplorer::Constants::CXX_LANGUAGE_ID);

        ProjectExplorer::Runnable runnable = testRunConfig->runnable();
        QCOMPARE(runnable.executable, toolChain->makeCommand(env));
        QCOMPARE(runnable.commandLineArguments, QString(QLatin1String("-f %1 check")).arg(makefile));
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
