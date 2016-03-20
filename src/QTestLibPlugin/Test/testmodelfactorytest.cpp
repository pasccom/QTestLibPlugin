/* Copyright 2015 Pascal COMBES <pascom@orange.fr>
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

#include "testmodelfactorytest.h"
#include "testhelper.h"

#include <common/qtestlibmodeltester.h>

#include <testmodelfactory.h>
#include <qtestlibpluginconstants.h>
#include <testrunconfiguration.h>

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/session.h>
#include <projectexplorer/project.h>
#include <projectexplorer/target.h>
#include <projectexplorer/runconfiguration.h>

#include <qmakeprojectmanager/desktopqmakerunconfiguration.h>

#include <extensionsystem/pluginmanager.h>

#include <utils/hostosinfo.h>

#include <QtTest>

namespace QTestLibPlugin {
namespace Test {

/*
 * NOTE This is extracted from QtCreator sources <qmakeprojectmanager/desktopqmakerunconfiguration.cpp>
 * I hoope this wont change in next releases otherwise these tests will be broken.
 */
#define QMAKE_RUNCONFIG_PREFIX "Qt4ProjectManager.Qt4RunConfiguration"
const QString CommandLineArgumentsKey = QLatin1String(QMAKE_RUNCONFIG_PREFIX ".CommandLineArguments");
const QString WorkingDirectoryKey = QLatin1String(QMAKE_RUNCONFIG_PREFIX ".UserWorkingDirectory");

void TestModelFactoryTest::initTestCase(void)
{
    QStringList projectPathes;

    // NOTE _data() function is not available for initTestCase()
    projectPathes << QLatin1String(TESTS_DIR "/OneClassTest");
    projectPathes << QLatin1String(TESTS_DIR "/AllMessagesTest");
    projectPathes << QLatin1String(TESTS_DIR "/MultipleClassesTest");
    projectPathes << QLatin1String(TESTS_DIR "/SignalsTest");
    projectPathes << QLatin1String(TESTS_DIR "/LimitsTest");
    projectPathes << QLatin1String(TESTS_DIR "/OneSubTest");
    projectPathes << QLatin1String(TESTS_DIR "/TwoSubTests");
    projectPathes << QLatin1String(TESTS_DIR "/NoSubTestOne");
    projectPathes << QLatin1String(TESTS_DIR "/NoSubTestTwo");

    foreach (QString projectPath, projectPathes)
        QVERIFY(removeProjectUserFiles(projectPath));
}

void TestModelFactoryTest::init(void)
{
    mProject = NULL;
    mFoundModel = NULL;
    mPopulatedModel = NULL;
    mFoundCount = 0;
    mPopulatedCount = 0;
    mOrderOk = false;
}

void TestModelFactoryTest::cleanup(void)
{
    if (mProject != NULL)
        ProjectExplorer::SessionManager::removeProject(mProject);
    if ((mFoundModel != NULL) && (mFoundModel != mPopulatedModel))
        delete mFoundModel;
    if (mPopulatedModel != NULL)
        delete mPopulatedModel;
}

void TestModelFactoryTest::data(void)
{
    QList<Internal::QTestLibArgsParser::TestVerbosity> verbosities;
    verbosities << Internal::QTestLibArgsParser::NormalVerbosity;
    verbosities << Internal::QTestLibArgsParser::Silent;
    verbosities << Internal::QTestLibArgsParser::Verbose1;
    verbosities << Internal::QTestLibArgsParser::Verbose2;
    verbosities << Internal::QTestLibArgsParser::VerboseSignal;

    QList<Internal::QTestLibArgsParser::TestOutputFormat> formats;
    formats << Internal::QTestLibArgsParser::TxtFormat;
    formats << Internal::QTestLibArgsParser::XmlFormat;
    formats << Internal::QTestLibArgsParser::LightXmlFormat;
    formats << Internal::QTestLibArgsParser::XUnitXmlFormat;

    QTest::addColumn<Internal::QTestLibArgsParser::TestOutputFormat>("format");
    QTest::addColumn<Internal::QTestLibArgsParser::TestVerbosity>("verbosity");

    foreach (Internal::QTestLibArgsParser::TestOutputFormat format, formats) {
        foreach (Internal::QTestLibArgsParser::TestVerbosity verbosity, verbosities) {
            Internal::QTestLibArgsParser args;
            args.setVerbosity(verbosity);
            args.setOutputFormat(format);
            QTest::newRow(qPrintable(args.toString())) << format << verbosity;
        }
    }
}

void TestModelFactoryTest::testOneClass(void)
{
    QFETCH(Internal::QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(Internal::QTestLibArgsParser::TestVerbosity, verbosity);

    runTest("OneClassTest", format, verbosity);
}

void TestModelFactoryTest::testAllMessages(void)
{
    QFETCH(Internal::QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(Internal::QTestLibArgsParser::TestVerbosity, verbosity);

    runTest("AllMessagesTest", format, verbosity);
}

void TestModelFactoryTest::testMultipleClasses(void)
{
    QFETCH(Internal::QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(Internal::QTestLibArgsParser::TestVerbosity, verbosity);

    runTest("MultipleClassesTest", format, verbosity);
}

void TestModelFactoryTest::testSignalsTest(void)
{
    QFETCH(Internal::QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(Internal::QTestLibArgsParser::TestVerbosity, verbosity);

    runTest("SignalsTest", format, verbosity);
}

void TestModelFactoryTest::testLimits(void)
{
    QFETCH(Internal::QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(Internal::QTestLibArgsParser::TestVerbosity, verbosity);

    runTest("LimitsTest", format, verbosity);
}

void TestModelFactoryTest::testOneSubTest(void)
{
    QFETCH(Internal::QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(Internal::QTestLibArgsParser::TestVerbosity, verbosity);

    runMakeCheck("OneSubTest", format, verbosity);
}

void TestModelFactoryTest::testTwoSubTests(void)
{
    QFETCH(Internal::QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(Internal::QTestLibArgsParser::TestVerbosity, verbosity);

    runMakeCheck("TwoSubTests", format, verbosity);
}

QStringList TestModelFactoryTest::commandLineArguments(Internal::QTestLibArgsParser::TestOutputFormat format, Internal::QTestLibArgsParser::TestVerbosity verbosity)
{
    QStringList cmdArgs;
    int argType = qrand() % 6;

    switch (format) {
    case Internal::QTestLibArgsParser::TxtFormat:
        switch (argType % 3) {
        case 0:
            break;
        case 1:
            cmdArgs << "-txt";
        case 2:
            cmdArgs << "-o" << "-,txt";
            break;
        default:
            Q_ASSERT(false); // IMPOSSIBLE
        }
        break;
    case Internal::QTestLibArgsParser::CsvFormat:
        if (argType % 2 == 0)
            cmdArgs << "-csv";
        else
            cmdArgs << "-o" << "-,csv";
        break;
    case Internal::QTestLibArgsParser::XmlFormat:
        if (argType % 2 == 0)
            cmdArgs << "-xml";
        else
            cmdArgs << "-o" << "-,xml";
        break;
    case Internal::QTestLibArgsParser::LightXmlFormat:
        if (argType % 2 == 0)
            cmdArgs << "-lightxml";
        else
            cmdArgs << "-o" << "-,lightxml";
        break;
    case Internal::QTestLibArgsParser::XUnitXmlFormat:
        if (argType % 2 == 0)
            cmdArgs << "-xunitxml";
        else
            cmdArgs << "-o" << "-,xunitxml";
        break;
    default:
        qWarning() << "Unknonw test output format";
        break;
    }

    switch (verbosity) {
    case Internal::QTestLibArgsParser::NormalVerbosity:
        break;
    case Internal::QTestLibArgsParser::Silent:
        cmdArgs << "-silent";
        break;
    case Internal::QTestLibArgsParser::Verbose1:
        cmdArgs << "-v1";
        break;
    case Internal::QTestLibArgsParser::Verbose2:
        cmdArgs << "-v2";
        break;
    case Internal::QTestLibArgsParser::VerboseSignal:
        cmdArgs << "-vs";
        break;
    default:
        qWarning() << "Unknonw test verbosity level";
        break;
    }

    qDebug() << "Command line arguments:" << cmdArgs;
    return cmdArgs;
}

QString TestModelFactoryTest::formatToString(Internal::QTestLibArgsParser::TestOutputFormat format)
{
    switch (format) {
    case Internal::QTestLibArgsParser::TxtFormat:
        return QLatin1String("txt");
    case Internal::QTestLibArgsParser::CsvFormat:
        return QLatin1String("csv");
    case Internal::QTestLibArgsParser::XmlFormat:
        return QLatin1String("xml");
    case Internal::QTestLibArgsParser::LightXmlFormat:
        return QLatin1String("lightxml");
    case Internal::QTestLibArgsParser::XUnitXmlFormat:
        return QLatin1String("xunitxml");
    default:
        qWarning() << "Unknonw test output format";
        break;
    }

    return QString::null;
}


void TestModelFactoryTest::runTest(const QString& testName, Internal::QTestLibArgsParser::TestOutputFormat format, Internal::QTestLibArgsParser::TestVerbosity verbosity)
{
    QVERIFY(openQMakeProject(TESTS_DIR "/" + testName + "/" + testName + ".pro", &mProject));

    // Retrieve RunConfiguration:
    ProjectExplorer::LocalApplicationRunConfiguration* testRunConfig = NULL;
    foreach (ProjectExplorer::RunConfiguration* runConfig, mProject->activeTarget()->runConfigurations()) {
        ProjectExplorer::LocalApplicationRunConfiguration* localRunConfig = qobject_cast<ProjectExplorer::LocalApplicationRunConfiguration*>(runConfig);
        if (localRunConfig == NULL)
            continue;
        QFileInfo exeFileInfo(localRunConfig->executable());
        qDebug() << exeFileInfo.absoluteFilePath();
        QVERIFY(exeFileInfo.exists());
        if (QString::compare(exeFileInfo.baseName(), testName, Qt::CaseSensitive) == 0)
            testRunConfig = localRunConfig;
        break;
    }
    QVERIFY(testRunConfig != NULL);

    // Change the run configuration map:
    QStringList cmdArgs = commandLineArguments(format, verbosity);
    QVariantMap map = testRunConfig->toMap();
    map.remove(CommandLineArgumentsKey);
    map.insert(CommandLineArgumentsKey, cmdArgs.join(QLatin1Char(' ')));
    map.remove(WorkingDirectoryKey);
    map.insert(WorkingDirectoryKey, TESTS_DIR "/" + testName + "/");

    // Restore a modified run configuration from the modified map:
    ProjectExplorer::IRunConfigurationFactory* runConfigFactory = ProjectExplorer::IRunConfigurationFactory::find(mProject->activeTarget(), map);
    QVERIFY(runConfigFactory != NULL);
    QVERIFY(runConfigFactory->canRestore(mProject->activeTarget(), map));
    ProjectExplorer::RunConfiguration* modifiedRunConfig = runConfigFactory->restore(mProject->activeTarget(), map);
    QVERIFY(modifiedRunConfig != NULL);
    testRunConfig = qobject_cast<ProjectExplorer::LocalApplicationRunConfiguration*>(modifiedRunConfig);
    QVERIFY(testRunConfig != NULL);
    QCOMPARE(testRunConfig->commandLineArguments(), cmdArgs.join(QLatin1Char(' ')));
    QCOMPARE(testRunConfig->workingDirectory(), TESTS_DIR "/" + testName);

    runRunConfiguration(testRunConfig, testName, format, verbosity);
}

void TestModelFactoryTest::runMakeCheck(const QString& testName, Internal::QTestLibArgsParser::TestOutputFormat format, Internal::QTestLibArgsParser::TestVerbosity verbosity)
{
    QVERIFY(openQMakeProject(TESTS_DIR "/" + testName + "/" + testName + ".pro", &mProject));

    // Retrieve RunConfiguration:
    ProjectExplorer::LocalApplicationRunConfiguration* testRunConfig = NULL;
    foreach (ProjectExplorer::RunConfiguration* runConfig, mProject->activeTarget()->runConfigurations()) {
        if (qobject_cast<Internal::TestRunConfiguration*>(runConfig) == NULL)
            continue;
        testRunConfig = qobject_cast<ProjectExplorer::LocalApplicationRunConfiguration*>(runConfig);
        break;
    }
    QVERIFY(testRunConfig != NULL);

    // Change the run configuration map:
    QVariantMap map = testRunConfig->toMap();
    map.remove(Constants::FormatKey);
    map.remove(Constants::VerbosityKey);
    map.insert(Constants::FormatKey, (int) format);
    map.insert(Constants::VerbosityKey, (int) verbosity);

    // Restore a modified run configuration from the modified map:
    ProjectExplorer::IRunConfigurationFactory* runConfigFactory = ProjectExplorer::IRunConfigurationFactory::find(mProject->activeTarget(), map);
    QVERIFY(runConfigFactory != NULL);
    QVERIFY(runConfigFactory->canRestore(mProject->activeTarget(), map));
    ProjectExplorer::RunConfiguration* modifiedRunConfig  = runConfigFactory->restore(mProject->activeTarget(), map);
    QVERIFY(modifiedRunConfig != NULL);
    testRunConfig = qobject_cast<ProjectExplorer::LocalApplicationRunConfiguration*>(modifiedRunConfig);
    QVERIFY(testRunConfig != NULL);

    // Compare arguments to expected value:
    Internal::QTestLibArgsParser testArgsParser;
    testArgsParser.setOutputFormat(format);
    testArgsParser.setVerbosity(verbosity);QString expectedCmdArgs(QLatin1String("-f " TESTS_DIR "/") + testName + QLatin1String("/Makefile check"));
    if (!testArgsParser.toString().isEmpty())
        expectedCmdArgs.append(QString(QLatin1String(" TESTARGS=\"%1\"")).arg(testArgsParser.toString()));
    QCOMPARE(testRunConfig->commandLineArguments(), expectedCmdArgs);
    QCOMPARE(testRunConfig->displayName(), QLatin1String("make check"));
    testRunConfig->setDisplayName(testName);

    runRunConfiguration(testRunConfig, testName, format, verbosity);
}

void TestModelFactoryTest::runRunConfiguration(ProjectExplorer::RunConfiguration *runConfig, const QString& testName, Internal::QTestLibArgsParser::TestOutputFormat format, Internal::QTestLibArgsParser::TestVerbosity verbosity)
{
    // Find a run control factory
    ProjectExplorer::IRunControlFactory* runControlFactory = NULL;
    foreach (runControlFactory, ExtensionSystem::PluginManager::getObjects<ProjectExplorer::IRunControlFactory>()) {
        if (runControlFactory->canRun(runConfig, ProjectExplorer::Constants::NORMAL_RUN_MODE))
            break;
    }
    QVERIFY(runControlFactory != NULL);
    QVERIFY(runControlFactory->canRun(runConfig, ProjectExplorer::Constants::NORMAL_RUN_MODE));

    // Create a run control
    QString errMsg;
    ProjectExplorer::RunControl* runControl = runControlFactory->create(runConfig, ProjectExplorer::Constants::NORMAL_RUN_MODE, &errMsg);
    QCOMPARE(errMsg, QString());
    QVERIFY(runControl != NULL);
    QCOMPARE(runControl->runConfiguration(), runConfig);

    // Create test model factory
    Internal::TestModelFactory* modelFactory = new Internal::TestModelFactory(runControl, this);

    // Signal spies
    QSignalSpy runControlFinishedSpy(runControl, SIGNAL(finished()));
    connect(modelFactory, &Internal::TestModelFactory::modelFound,
            this, [this] (QAbstractItemModel *model) {
        mFoundModel = model;
        mFoundCount++;
    });
    connect(modelFactory, &Internal::TestModelFactory::modelPopulated,
            this, [this] (QAbstractItemModel *model) {
        mOrderOk = (mFoundModel !=  NULL);
        if (mPopulatedModel != NULL)
            delete mPopulatedModel;
        mPopulatedModel = model;
        mPopulatedCount++;
    });

    // Run the test
    runControl->start();
    if (!runControlFinishedSpy.wait()) {
        runControl->stop();
        QVERIFY((runControlFinishedSpy.count() == 1) || runControlFinishedSpy.wait());
        QSKIP("Computer is too slow for this test.");
    }

    // Retrieve information:
    QVERIFY(mFoundModel == mPopulatedModel);
    QCOMPARE(mFoundCount, 1);
    QCOMPARE(mPopulatedCount, 1);
    QVERIFY(mOrderOk);

    // Test the model:
    QString formatString = formatToString(format);
    if (format != Internal::QTestLibArgsParser::TxtFormat)
        verbosity = qMax(verbosity, Internal::QTestLibArgsParser::NormalVerbosity);
    QTestLibModelTester tester(mPopulatedModel, (QTestLibModelTester::Verbosity) verbosity, formatString);
    QVERIFY2(tester.checkIndex(QModelIndex(), testName), qPrintable(tester.error()));
}

} // Test
} // QTestLibPlugin
