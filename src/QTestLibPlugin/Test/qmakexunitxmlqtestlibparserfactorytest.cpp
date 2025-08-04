/* Copyright 2015-2020 Pascal COMBES <pascom@orange.fr>
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

#include "qmakexunitxmlqtestlibparserfactorytest.h"
#include "testhelper.h"

#include "../xunitxmlqtestlibparserfactory.h"
#include "../baseqmakeqtestlibparserfactory.h"
#include "../qtestlibpluginconstants.h"
#include "../testrunconfiguration.h"
#include "../testextraaspect.h"

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/project.h>
#include <projectexplorer/target.h>
#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/runconfigurationaspects.h>
#include <projectexplorer/runcontrol.h>

#include <extensionsystem/pluginmanager.h>

#include <utils/hostosinfo.h>
#include <utils/processinterface.h>

#include <QtTest>

namespace QTestLibPlugin {
namespace Test {

void QMakeXUnitXMLQTestLibParserFactoryTest::initTestCase(void)
{
    Utils::FilePaths projectPathes;

    // NOTE _data() function is not available for initTestCase()
    projectPathes << Utils::FilePath(TESTS_DIR "/qt5/OneClassTest");
    projectPathes << Utils::FilePath(TESTS_DIR "/qt5/AllMessagesTest");
    projectPathes << Utils::FilePath(TESTS_DIR "/qt5/MultipleClassesTest");
    projectPathes << Utils::FilePath(TESTS_DIR "/qt5/SignalsTest");
    projectPathes << Utils::FilePath(TESTS_DIR "/qt5/LimitsTest");
    projectPathes << Utils::FilePath(TESTS_DIR "/qt5/OneSubTest");
    projectPathes << Utils::FilePath(TESTS_DIR "/qt5/TwoSubTests");
    projectPathes << Utils::FilePath(TESTS_DIR "/qt5/NoSubTestOne");
    projectPathes << Utils::FilePath(TESTS_DIR "/qt5/NoSubTestTwo");

    foreach (Utils::FilePath projectPath, projectPathes)
        QVERIFY(removeProjectUserFiles(projectPath));

    // NOTE First time ProjectExplorer::ProjectExplorerPlugin::openProject()
    // immediately calls ProjectExplorer::Target::ParsingFinished() and
    // consequently, openQMakeProject() does not work
    openQMakeProject(Utils::FilePath::fromString(TESTS_DIR "/qt5/OneClassTest/OneClassTest.pro"), &mProject);
    QVERIFY(closeProject(mProject));
}

void QMakeXUnitXMLQTestLibParserFactoryTest::init(void)
{
    mProject = NULL;
}

void QMakeXUnitXMLQTestLibParserFactoryTest::cleanup(void)
{
    if (mProject != nullptr)
        QVERIFY(closeProject(mProject));
}


void QMakeXUnitXMLQTestLibParserFactoryTest::dataTest(void)
{
    QList< QPair<QString, bool> > verbosities;
    verbosities << qMakePair(QString(), true);
    verbosities << qMakePair(QLatin1String("-silent"), true);
    verbosities << qMakePair(QLatin1String("-v1"), true);
    verbosities << qMakePair(QLatin1String("-v2"), true);
    verbosities << qMakePair(QLatin1String("-vs"), true);

    QList< QPair<QString, bool> > formats;
    formats << qMakePair(QString(), false);
    formats << qMakePair(QLatin1String("txt"), false);
    formats << qMakePair(QLatin1String("csv"), false);
    formats << qMakePair(QLatin1String("xml"), false);
    formats << qMakePair(QLatin1String("lightxml"), false);
    formats << qMakePair(QLatin1String("xunitxml"), true);

    QTest::addColumn<QString>("qtVersion");
    QTest::addColumn<QStringList>("cmdArgs");
    QTest::addColumn<bool>("result");

    typedef QPair<QString, bool> QPairStringbool;
    foreach (QPairStringbool formatPair, formats) {
        foreach (QPairStringbool verbosityPair, verbosities) {
            QStringList cmdArgs;

            if (!formatPair.first.isNull()) {
                cmdArgs << QLatin1Char('-') + formatPair.first;
                cmdArgs << QLatin1String("-o") << QLatin1String("-,") + formatPair.first;
            }
            if (!verbosityPair.first.isNull())
                cmdArgs << verbosityPair.first;

            QTest::newRow(qPrintable("qt5 " + cmdArgs.join(' '))) << "qt5" << cmdArgs << (formatPair.second && verbosityPair.second);
        }
    }
}

void QMakeXUnitXMLQTestLibParserFactoryTest::dataMakeCheck(void)
{
    QList< QPair<Internal::QTestLibArgsParser::TestVerbosity, bool> > verbosities;
    verbosities << qMakePair(Internal::QTestLibArgsParser::NormalVerbosity, true);
    verbosities << qMakePair(Internal::QTestLibArgsParser::Silent, true);
    verbosities << qMakePair(Internal::QTestLibArgsParser::Verbose1, true);
    verbosities << qMakePair(Internal::QTestLibArgsParser::Verbose2, true);
    verbosities << qMakePair(Internal::QTestLibArgsParser::VerboseSignal, true);

    QList< QPair<Internal::QTestLibArgsParser::TestOutputFormat, bool> > formats;
    formats << qMakePair(Internal::QTestLibArgsParser::TxtFormat, false);
    formats << qMakePair(Internal::QTestLibArgsParser::CsvFormat, false);
    formats << qMakePair(Internal::QTestLibArgsParser::XmlFormat, false);
    formats << qMakePair(Internal::QTestLibArgsParser::LightXmlFormat, false);
    formats << qMakePair(Internal::QTestLibArgsParser::XUnitXmlFormat, true);

    QTest::addColumn<QString>("qtVersion");
    QTest::addColumn<Internal::QTestLibArgsParser::TestOutputFormat>("format");
    QTest::addColumn<Internal::QTestLibArgsParser::TestVerbosity>("verbosity");
    QTest::addColumn<bool>("result");

    typedef QPair<Internal::QTestLibArgsParser::TestVerbosity, bool> QPairVerbositybool;
    typedef QPair<Internal::QTestLibArgsParser::TestOutputFormat, bool> QPairFormatbool;
    foreach (QPairFormatbool formatPair, formats) {
        foreach (QPairVerbositybool verbosityPair, verbosities) {
            Internal::QTestLibArgsParser args;
            args.setVerbosity(verbosityPair.first);
            args.setOutputFormat(formatPair.first);
            QTest::newRow(qPrintable("qt5 " + args.toString())) << "qt5" << formatPair.first << verbosityPair.first << (formatPair.second && verbosityPair.second);
        }
    }
}

void QMakeXUnitXMLQTestLibParserFactoryTest::testOneClass(void)
{
    QFETCH(QString, qtVersion);
    QFETCH(QStringList, cmdArgs);
    QFETCH(bool, result);

    runTest(qtVersion, "OneClassTest", cmdArgs, result);
}

void QMakeXUnitXMLQTestLibParserFactoryTest::testAllMessages(void)
{
    QFETCH(QString, qtVersion);
    QFETCH(QStringList, cmdArgs);
    QFETCH(bool, result);

    runTest(qtVersion, "AllMessagesTest", cmdArgs, result);
}

void QMakeXUnitXMLQTestLibParserFactoryTest::testMultipleClasses(void)
{
    QFETCH(QString, qtVersion);
    QFETCH(QStringList, cmdArgs);
    QFETCH(bool, result);

    runTest(qtVersion, "MultipleClassesTest", cmdArgs, result);
}

void QMakeXUnitXMLQTestLibParserFactoryTest::testSignalsTest(void)
{
    QFETCH(QString, qtVersion);
    QFETCH(QStringList, cmdArgs);
    QFETCH(bool, result);

    runTest(qtVersion, "SignalsTest", cmdArgs, result);
}

void QMakeXUnitXMLQTestLibParserFactoryTest::testLimits(void)
{
    QFETCH(QString, qtVersion);
    QFETCH(QStringList, cmdArgs);
    QFETCH(bool, result);

    runTest(qtVersion, "LimitsTest", cmdArgs, result);
}

void QMakeXUnitXMLQTestLibParserFactoryTest::testOneSubTest(void)
{
    QFETCH(QString, qtVersion);
    QFETCH(Internal::QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(Internal::QTestLibArgsParser::TestVerbosity, verbosity);
    QFETCH(bool, result);

    runMakeCheck(qtVersion, "OneSubTest", format, verbosity, result);
}

void QMakeXUnitXMLQTestLibParserFactoryTest::testTwoSubTests(void)
{
    QFETCH(QString, qtVersion);
    QFETCH(Internal::QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(Internal::QTestLibArgsParser::TestVerbosity, verbosity);
    QFETCH(bool, result);

    runMakeCheck(qtVersion, "TwoSubTests", format, verbosity, result);
}

void QMakeXUnitXMLQTestLibParserFactoryTest::runTest(const QString& qtVersion, const QString& testName, const QStringList& cmdArgs, bool result)
{
    QVERIFY(openQMakeProject(Utils::FilePath::fromString(TESTS_DIR "/" + qtVersion + "/" + testName + "/" + testName + ".pro"), &mProject));

    // Retrieve RunConfiguration:
    ProjectExplorer::RunConfiguration* testRunConfig = NULL;
    foreach (ProjectExplorer::RunConfiguration* runConfig, mProject->activeTarget()->runConfigurations()) {
        QFileInfo exeFileInfo = runConfig->runnable().command.executable().toFileInfo();
        qDebug() << exeFileInfo.absoluteFilePath();
        QVERIFY(exeFileInfo.exists());
        if (QString::compare(exeFileInfo.baseName(), testName, Qt::CaseSensitive) != 0)
            continue;
        testRunConfig = runConfig;
        break;
    }
    QVERIFY(testRunConfig != NULL);

    // Change the argument aspect:
    ProjectExplorer::ArgumentsAspect* argsAspect = testRunConfig->aspect<ProjectExplorer::ArgumentsAspect>();
    QVERIFY(argsAspect != nullptr);
    argsAspect->setArguments(cmdArgs.join(QLatin1Char(' ')));

    // Check the output parser factory:
    ProjectExplorer::RunControl testRunControl(ProjectExplorer::Constants::NORMAL_RUN_MODE);
    testRunControl.copyDataFromRunConfiguration(testRunConfig);
    testFactory(&testRunControl, result);
}

void QMakeXUnitXMLQTestLibParserFactoryTest::runMakeCheck(const QString& qtVersion, const QString& testName, Internal::QTestLibArgsParser::TestOutputFormat format, Internal::QTestLibArgsParser::TestVerbosity verbosity, bool result)
{
    QVERIFY(openQMakeProject(Utils::FilePath::fromString(TESTS_DIR "/" + qtVersion + "/" + testName + "/" + testName + ".pro"), &mProject));

    // Retrieve RunConfiguration:
    ProjectExplorer::RunConfiguration* testRunConfig = NULL;
    foreach (ProjectExplorer::RunConfiguration* runConfig, mProject->activeTarget()->runConfigurations()) {
        if (runConfig->id() != Utils::Id(Constants::TestRunConfigurationId))
            continue;
        testRunConfig = runConfig;
        break;
    }
    QVERIFY(testRunConfig != NULL);

    // Change the text extra aspect:
    QTestLibPlugin::Internal::TestExtraAspect* testAspect = testRunConfig->aspect<QTestLibPlugin::Internal::TestExtraAspect>();
    QVERIFY(testAspect != nullptr);
    testAspect->setOutputFormat(format);
    testAspect->setVerbosity(verbosity);

    // Compare arguments to expected value:
    Utils::ProcessRunData modifiedRunnable = testRunConfig->runnable();
    Internal::QTestLibArgsParser testArgsParser;
    testArgsParser.setOutputFormat(format);
    testArgsParser.setVerbosity(verbosity);
    QString expectedCmdArgs(QLatin1String("-f " TESTS_DIR) + qtVersion + "/" + testName + QLatin1String("/Makefile check"));
    if (!testArgsParser.toString().isEmpty())
        expectedCmdArgs.append(QString(QLatin1String(" TESTARGS=\"%1\"")).arg(testArgsParser.toString()));
    QCOMPARE(modifiedRunnable.command.arguments(), expectedCmdArgs);

    // Check the output parser factory:
    ProjectExplorer::RunControl testRunControl(ProjectExplorer::Constants::NORMAL_RUN_MODE);
    testRunControl.copyDataFromRunConfiguration(testRunConfig);
    testFactory(&testRunControl, result);
}

void QMakeXUnitXMLQTestLibParserFactoryTest::testFactory(ProjectExplorer::RunControl* testRunControl, bool result)
{
    // Retrieve factory:
    QLinkedList<QTestLibPlugin::Internal::AbstractTestParserFactory*> parserFactories = QTestLibPlugin::Internal::TestModelFactory::parserFactories(Utils::Id(QTestLibPlugin::Constants::XUnitXmlQTestLibParserFactoryId).withSuffix(QTestLibPlugin::Constants::BaseQMakeQTestLibParserFactoryId));
    QCOMPARE(parserFactories.size(), 1);
    QTestLibPlugin::Internal::AbstractTestParserFactory* parserFactory = parserFactories.first();
    QVERIFY(parserFactory != nullptr);

    QCOMPARE(parserFactory->canParse(testRunControl), result);
    QTestLibPlugin::Internal::AbstractTestParser* parser = parserFactory->getParserInstance(testRunControl);
    QCOMPARE(parser != NULL, result);
    delete parser;
}


} // Test
} // QTestLibPlugin
