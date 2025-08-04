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

#include "forceplaintextqtestlibparserfactorytest.h"
#include "testhelper.h"

#include "../plaintextqtestlibparserfactory.h"
#include "../baseforceparserfactory.h"
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

#include <QtWidgets>
#include <QtTest>

namespace QTestLibPlugin {
namespace Test {

void ForcePlainTextQTestLibParserFactoryTest::initTestCase(void)
{
    Utils::FilePaths projectPathes;

    // NOTE _data() function is not available for initTestCase()
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "/qt5/OneClassTest");
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "/qt5/AllMessagesTest");
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "/qt5/MultipleClassesTest");
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "/qt5/SignalsTest");
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "/qt5/LimitsTest");
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "/qt5/OneSubTest");
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "/qt5/TwoSubTests");
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "/qt5/NoSubTestOne");
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "/qt5/NoSubTestTwo");

    for (Utils::FilePath projectPath : projectPathes)
        QVERIFY(removeProjectUserFiles(projectPath));

    // NOTE First time ProjectExplorer::ProjectExplorerPlugin::openProject()
    // immediately calls ProjectExplorer::Target::ParsingFinished() and
    // consequently, openQMakeProject() does not work
    openQMakeProject(Utils::FilePath::fromString(TESTS_DIR "/qt5/OneClassTest/OneClassTest.pro"), &mProject);
    QVERIFY(closeProject(mProject));
}

void ForcePlainTextQTestLibParserFactoryTest::init(void)
{
    mProject = NULL;
}

void ForcePlainTextQTestLibParserFactoryTest::cleanup(void)
{
    if (mProject != nullptr)
        QVERIFY(closeProject(mProject));
}

void ForcePlainTextQTestLibParserFactoryTest::dataTest(void)
{
    QStringList verbosities;
    verbosities << QString();
    verbosities << QLatin1String("-silent");
    verbosities << QLatin1String("-v1");
    verbosities << QLatin1String("-v2");
    verbosities << QLatin1String("-vs");

    QStringList formats;
    formats << QString();
    formats << QLatin1String("txt");
    formats << QLatin1String("csv");
    formats << QLatin1String("xml");
    formats << QLatin1String("lightxml");
    formats << QLatin1String("xunitxml");

    QTest::addColumn<QString>("qtVersion");
    QTest::addColumn<QStringList>("cmdArgs");

    foreach (QString format, formats) {
        foreach (QString verbosity, verbosities) {
            QStringList cmdArgs;

            if (!format.isNull()) {
                cmdArgs << QLatin1Char('-') + format;
                cmdArgs << QLatin1String("-o") << QLatin1String("-,") + format;
            }
            if (!verbosity.isNull())
                cmdArgs << verbosity;

            QTest::newRow(qPrintable("qt5 " + cmdArgs.join(' '))) << "qt5" << cmdArgs;
        }
    }
}

void ForcePlainTextQTestLibParserFactoryTest::dataMakeCheck(void)
{
    QList<Internal::QTestLibArgsParser::TestVerbosity> verbosities;
    verbosities << Internal::QTestLibArgsParser::NormalVerbosity;
    verbosities << Internal::QTestLibArgsParser::Silent;
    verbosities << Internal::QTestLibArgsParser::Verbose1;
    verbosities << Internal::QTestLibArgsParser::Verbose2;
    verbosities << Internal::QTestLibArgsParser::VerboseSignal;

    QList<Internal::QTestLibArgsParser::TestOutputFormat> formats;
    formats << Internal::QTestLibArgsParser::TxtFormat;
    formats << Internal::QTestLibArgsParser::CsvFormat;
    formats << Internal::QTestLibArgsParser::XmlFormat;
    formats << Internal::QTestLibArgsParser::LightXmlFormat;
    formats << Internal::QTestLibArgsParser::XUnitXmlFormat;

    QTest::addColumn<QString>("qtVersion");
    QTest::addColumn<Internal::QTestLibArgsParser::TestOutputFormat>("format");
    QTest::addColumn<Internal::QTestLibArgsParser::TestVerbosity>("verbosity");

    foreach (Internal::QTestLibArgsParser::TestOutputFormat format, formats) {
        foreach (Internal::QTestLibArgsParser::TestVerbosity verbosity, verbosities) {
            Internal::QTestLibArgsParser args;
            args.setVerbosity(verbosity);
            args.setOutputFormat(format);
            QTest::newRow(qPrintable("qt5 " + args.toString())) << "qt5" << format << verbosity;
        }
    }
}

void ForcePlainTextQTestLibParserFactoryTest::testOneClass(void)
{
    QFETCH(QString, qtVersion);
    QFETCH(QStringList, cmdArgs);

    runTest(qtVersion, "OneClassTest", cmdArgs);
}

void ForcePlainTextQTestLibParserFactoryTest::testAllMessages(void)
{
    QFETCH(QString, qtVersion);
    QFETCH(QStringList, cmdArgs);

    runTest(qtVersion, "AllMessagesTest", cmdArgs);
}

void ForcePlainTextQTestLibParserFactoryTest::testMultipleClasses(void)
{
    QFETCH(QString, qtVersion);
    QFETCH(QStringList, cmdArgs);

    runTest(qtVersion, "MultipleClassesTest", cmdArgs);
}

void ForcePlainTextQTestLibParserFactoryTest::testSignalsTest(void)
{
    QFETCH(QString, qtVersion);
    QFETCH(QStringList, cmdArgs);

    runTest(qtVersion, "SignalsTest", cmdArgs);
}

void ForcePlainTextQTestLibParserFactoryTest::testLimits(void)
{
    QFETCH(QString, qtVersion);
    QFETCH(QStringList, cmdArgs);

    runTest(qtVersion, "LimitsTest", cmdArgs);
}

void ForcePlainTextQTestLibParserFactoryTest::testOneSubTest(void)
{
    QFETCH(QString, qtVersion);
    QFETCH(Internal::QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(Internal::QTestLibArgsParser::TestVerbosity, verbosity);

    runMakeCheck(qtVersion, "OneSubTest", format, verbosity);
}

void ForcePlainTextQTestLibParserFactoryTest::testTwoSubTests(void)
{
    QFETCH(QString, qtVersion);
    QFETCH(Internal::QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(Internal::QTestLibArgsParser::TestVerbosity, verbosity);

    runMakeCheck(qtVersion, "TwoSubTests", format, verbosity);
}

void ForcePlainTextQTestLibParserFactoryTest::runTest(const QString& qtVersion, const QString& testName, const QStringList& cmdArgs)
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
    testFactory(&testRunControl);
}

void ForcePlainTextQTestLibParserFactoryTest::runMakeCheck(const QString& qtVersion, const QString& testName, Internal::QTestLibArgsParser::TestOutputFormat format, Internal::QTestLibArgsParser::TestVerbosity verbosity)
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
    testFactory(&testRunControl);
}

void ForcePlainTextQTestLibParserFactoryTest::testFactory(ProjectExplorer::RunControl* testRunControl)
{
    // Retrieve factory:
    QLinkedList<QTestLibPlugin::Internal::AbstractTestParserFactory*> parserFactories = QTestLibPlugin::Internal::TestModelFactory::parserFactories(Utils::Id(QTestLibPlugin::Constants::PlainTextQTestLibParserFactoryId).withSuffix(QTestLibPlugin::Constants::BaseForceParserFactoryId));
    QCOMPARE(parserFactories.size(), 1);
    QTestLibPlugin::Internal::PlainTextQTestLibParserFactory<QTestLibPlugin::Internal::BaseForceParserFactory>* parserFactory = dynamic_cast<QTestLibPlugin::Internal::PlainTextQTestLibParserFactory<QTestLibPlugin::Internal::BaseForceParserFactory>*>(parserFactories.first());
    QVERIFY(parserFactory != nullptr);

    // Find output pane:
    QTestLibPlugin::Internal::TestOutputPane* outputPane = parserFactory->base().outputPane();
    QVERIFY(outputPane != NULL);

    // Find force combo box:
    QComboBox* forceComboBox = NULL;
    foreach (QWidget* widget, outputPane->toolBarWidgets()) {
        forceComboBox = qobject_cast<QComboBox *>(widget);
        if (forceComboBox != NULL)
            break;
    }
    QVERIFY(forceComboBox != NULL);

    QTestLibPlugin::Internal::AbstractTestParser* parser;
    for (int index = 0; index < forceComboBox->count(); index++) {
        forceComboBox->setCurrentIndex(index);

        // Get current forced format:
        QVariant data = forceComboBox->currentData();
        QVERIFY(data.canConvert<Internal::QTestLibArgsParser::TestOutputFormat>());
        Internal::QTestLibArgsParser::TestOutputFormat format = data.value<Internal::QTestLibArgsParser::TestOutputFormat>();
        QCOMPARE(outputPane->userForceParser(), format);

        // Check the factory:
        if ((format == Internal::QTestLibArgsParser::TxtFormat) || (format == Internal::QTestLibArgsParser::AllFormats)) {
            QVERIFY(parserFactory->canParse(testRunControl));
            parser = parserFactory->getParserInstance(testRunControl);
            bool parserCreated = (parser != NULL);
            if (parserCreated)
                delete parser;
            QVERIFY(parserCreated);
        } else {
            QVERIFY(!parserFactory->canParse(testRunControl));
            parser = parserFactory->getParserInstance(testRunControl);
            bool parserCreated = (parser != NULL);
            if (parserCreated)
                delete parser;
            QVERIFY(!parserCreated);
        }
    }
}

} // Test
} // QTestLibPlugin
