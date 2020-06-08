/* Copyright 2015-2016 Pascal COMBES <pascom@orange.fr>
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
#include <testextraaspect.h>

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/session.h>
#include <projectexplorer/project.h>
#include <projectexplorer/target.h>
#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/runconfigurationaspects.h>
#include <projectexplorer/runcontrol.h>

#include <extensionsystem/pluginmanager.h>

#include <utils/hostosinfo.h>

#include <QtTest>

namespace QTestLibPlugin {
namespace Test {

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
    closeProject(mProject);
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
    QTest::addColumn<Core::Id>("runMode");

    foreach (Internal::QTestLibArgsParser::TestOutputFormat format, formats) {
        foreach (Internal::QTestLibArgsParser::TestVerbosity verbosity, verbosities) {
            Internal::QTestLibArgsParser args;
            args.setVerbosity(verbosity);
            args.setOutputFormat(format);
            QTest::newRow(qPrintable(QString("Normal %1").arg(args.toString()))) << format << verbosity << Core::Id(ProjectExplorer::Constants::NORMAL_RUN_MODE);
            QTest::newRow(qPrintable(QString("Debug %1").arg(args.toString()))) << format << verbosity << Core::Id(ProjectExplorer::Constants::DEBUG_RUN_MODE);
        }
    }
}

void TestModelFactoryTest::testOneClass(void)
{
    QFETCH(Internal::QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(Internal::QTestLibArgsParser::TestVerbosity, verbosity);
    QFETCH(Core::Id, runMode);

    if ((runMode == ProjectExplorer::Constants::DEBUG_RUN_MODE) && (verbosity == Internal::QTestLibArgsParser::VerboseSignal))
        QSKIP("Debugging with signals changes test output");
    runTest("OneClassTest", format, verbosity, runMode);
}

void TestModelFactoryTest::testAllMessages(void)
{
    QFETCH(Internal::QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(Internal::QTestLibArgsParser::TestVerbosity, verbosity);
    QFETCH(Core::Id, runMode);

    if (runMode == ProjectExplorer::Constants::DEBUG_RUN_MODE)
        QSKIP("Debugging AllMessagesTest is not supported due to OS error signal");
    runTest("AllMessagesTest", format, verbosity, runMode);
}

void TestModelFactoryTest::testMultipleClasses(void)
{
    QFETCH(Internal::QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(Internal::QTestLibArgsParser::TestVerbosity, verbosity);
    QFETCH(Core::Id, runMode);

    if ((runMode == ProjectExplorer::Constants::DEBUG_RUN_MODE) && (verbosity == Internal::QTestLibArgsParser::VerboseSignal))
        QSKIP("Debugging with signals changes test output");
    runTest("MultipleClassesTest", format, verbosity, runMode);
}

void TestModelFactoryTest::testSignalsTest(void)
{
    QFETCH(Internal::QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(Internal::QTestLibArgsParser::TestVerbosity, verbosity);
    QFETCH(Core::Id, runMode);

    if ((runMode == ProjectExplorer::Constants::DEBUG_RUN_MODE) && (verbosity == Internal::QTestLibArgsParser::VerboseSignal))
        QSKIP("Debugging with signals changes test output");
    runTest("SignalsTest", format, verbosity, runMode);
}

void TestModelFactoryTest::testLimits(void)
{
    QFETCH(Internal::QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(Internal::QTestLibArgsParser::TestVerbosity, verbosity);
    QFETCH(Core::Id, runMode);

    if ((runMode == ProjectExplorer::Constants::DEBUG_RUN_MODE) && (verbosity == Internal::QTestLibArgsParser::VerboseSignal))
        QSKIP("Debugging with signals changes test output");
    runTest("LimitsTest", format, verbosity, runMode);
}

void TestModelFactoryTest::testOneSubTest(void)
{
    QFETCH(Internal::QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(Internal::QTestLibArgsParser::TestVerbosity, verbosity);
    QFETCH(Core::Id, runMode);

    runMakeCheck("OneSubTest", format, verbosity, runMode);
}

void TestModelFactoryTest::testTwoSubTests(void)
{
    QFETCH(Internal::QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(Internal::QTestLibArgsParser::TestVerbosity, verbosity);
    QFETCH(Core::Id, runMode);

    runMakeCheck("TwoSubTests", format, verbosity, runMode);
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
            break;
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

    return QString();
}


void TestModelFactoryTest::runTest(const QString& testName, Internal::QTestLibArgsParser::TestOutputFormat format, Internal::QTestLibArgsParser::TestVerbosity verbosity, Core::Id runMode)
{
    QVERIFY(openQMakeProject(TESTS_DIR "/" + testName + "/" + testName + ".pro", &mProject));

    // Retrieve RunConfiguration:
    ProjectExplorer::RunConfiguration* testRunConfig = NULL;
    foreach (ProjectExplorer::RunConfiguration* runConfig, mProject->activeTarget()->runConfigurations()) {
        QFileInfo exeFileInfo = runConfig->runnable().executable.toFileInfo();
        qDebug() << exeFileInfo.absoluteFilePath();
        QVERIFY(exeFileInfo.exists());
        if (QString::compare(exeFileInfo.baseName(), testName, Qt::CaseSensitive) != 0)
            continue;

        testRunConfig = runConfig;
        break;
    }
    QVERIFY(testRunConfig != NULL);

    // Change the argument aspect:
    QStringList cmdArgs = commandLineArguments(format, verbosity);
    ProjectExplorer::ArgumentsAspect* argsAspect = testRunConfig->aspect<ProjectExplorer::ArgumentsAspect>();
    QVERIFY(argsAspect != nullptr);
    argsAspect->setArguments(cmdArgs.join(QLatin1Char(' ')));

    // Change the working directory aspect
    ProjectExplorer::WorkingDirectoryAspect* workingDirectoryAspect = testRunConfig->aspect<ProjectExplorer::WorkingDirectoryAspect>();
    QVERIFY(workingDirectoryAspect != nullptr);
    workingDirectoryAspect->setDefaultWorkingDirectory(Utils::FilePath::fromString(TESTS_DIR).pathAppended(testName));

    // Check the modifications were applied:
    ProjectExplorer::Runnable modifiedRunnable = testRunConfig->runnable();
    QCOMPARE(modifiedRunnable.commandLineArguments, cmdArgs.join(QLatin1Char(' ')));
    QCOMPARE(modifiedRunnable.workingDirectory, QString(TESTS_DIR "/" + testName));

    runRunConfiguration(testRunConfig, testName, format, verbosity, runMode);
}

void TestModelFactoryTest::runMakeCheck(const QString& testName, Internal::QTestLibArgsParser::TestOutputFormat format, Internal::QTestLibArgsParser::TestVerbosity verbosity, Core::Id runMode)
{
    QVERIFY(openQMakeProject(TESTS_DIR "/" + testName + "/" + testName + ".pro", &mProject));

    // Retrieve RunConfiguration:
    ProjectExplorer::RunConfiguration* testRunConfig = NULL;
    foreach (ProjectExplorer::RunConfiguration* runConfig, mProject->activeTarget()->runConfigurations()) {
        if (runConfig->id() != Core::Id(Constants::TestRunConfigurationId))
            continue;
        testRunConfig = qobject_cast<ProjectExplorer::RunConfiguration*>(runConfig);
        break;
    }
    QVERIFY(testRunConfig != NULL);
    qDebug() << testRunConfig->displayName();

    // Change the text extra aspect:
    QTestLibPlugin::Internal::TestExtraAspect* testAspect = testRunConfig->aspect<QTestLibPlugin::Internal::TestExtraAspect>();
    QVERIFY(testAspect != nullptr);
    testAspect->setOutputFormat(format);
    testAspect->setVerbosity(verbosity);

    // Compare arguments to expected value:
    ProjectExplorer::Runnable modifiedRunnable = testRunConfig->runnable();
    Internal::QTestLibArgsParser testArgsParser;
    testArgsParser.setOutputFormat(format);
    testArgsParser.setVerbosity(verbosity);QString expectedCmdArgs(QLatin1String("-f " TESTS_DIR "/") + testName + QLatin1String("/Makefile check"));
    if (!testArgsParser.toString().isEmpty())
        expectedCmdArgs.append(QString(QLatin1String(" TESTARGS=\"%1\"")).arg(testArgsParser.toString()));
    QCOMPARE(modifiedRunnable.commandLineArguments, expectedCmdArgs);

    testRunConfig->setDisplayName(testName);
    runRunConfiguration(testRunConfig, testName, format, verbosity, runMode);
}

void TestModelFactoryTest::runRunConfiguration(ProjectExplorer::RunConfiguration *runConfig, const QString& testName, Internal::QTestLibArgsParser::TestOutputFormat format, Internal::QTestLibArgsParser::TestVerbosity verbosity, Core::Id runMode)
{
    // Create a run control and a run worker:
    ProjectExplorer::RunControl* runControl = new ProjectExplorer::RunControl(runMode);
    runControl->setRunConfiguration(runConfig);
    QVERIFY2(runControl->createMainWorker(), "Could not create main worker");

    // Create test model factory
    Internal::TestModelFactory* modelFactory = new Internal::TestModelFactory(runControl, this);

    // Signal spies
    QSignalSpy runControlStartedSpy(runControl, SIGNAL(started()));
    QSignalSpy runControlStoppedSpy(runControl, SIGNAL(stopped()));
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
    ProjectExplorer::ProjectExplorerPlugin::startRunControl(runControl);
    QVERIFY((runControlStartedSpy.count() == 1) || runControlStartedSpy.wait());
    if (!runControlStoppedSpy.wait()) {
        runControl->initiateStop();
        QVERIFY((runControlStoppedSpy.count() == 1) || runControlStoppedSpy.wait());
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
    tester.setResultsFile(TESTS_DIR "/" + testName + "/" + testName.toLower() + ".xml");
    QVERIFY2(tester.checkIndex(QModelIndex()), qPrintable(tester.error()));
}

} // Test
} // QTestLibPlugin
