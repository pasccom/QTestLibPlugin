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

#include "forcexunitxmlqtestlibparserfactorytest.h"
#include "testhelper.h"

#include <xunitxmlqtestlibparserfactory.h>
#include <baseforceparserfactory.h>
#include <qtestlibpluginconstants.h>
#include <testrunconfiguration.h>
#include <testextraaspect.h>

#include <projectexplorer/session.h>
#include <projectexplorer/project.h>
#include <projectexplorer/target.h>
#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/runconfigurationaspects.h>
#include <projectexplorer/runcontrol.h>

#include <extensionsystem/pluginmanager.h>

#include <utils/hostosinfo.h>

#include <QtWidgets>
#include <QtTest>

namespace QTestLibPlugin {
namespace Test {

void ForceXUnitXMLQTestLibParserFactoryTest::initTestCase(void)
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

void ForceXUnitXMLQTestLibParserFactoryTest::init(void)
{
    mProject = NULL;
}

void ForceXUnitXMLQTestLibParserFactoryTest::cleanup(void)
{
    closeProject(mProject);
}

void ForceXUnitXMLQTestLibParserFactoryTest::dataTest(void)
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

            QTest::newRow(qPrintable(cmdArgs.join(' '))) << cmdArgs;
        }
    }
}

void ForceXUnitXMLQTestLibParserFactoryTest::dataMakeCheck(void)
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

void ForceXUnitXMLQTestLibParserFactoryTest::testOneClass(void)
{
    QFETCH(QStringList, cmdArgs);

    runTest("OneClassTest", cmdArgs);
}

void ForceXUnitXMLQTestLibParserFactoryTest::testAllMessages(void)
{
    QFETCH(QStringList, cmdArgs);

    runTest("AllMessagesTest", cmdArgs);
}

void ForceXUnitXMLQTestLibParserFactoryTest::testMultipleClasses(void)
{
    QFETCH(QStringList, cmdArgs);

    runTest("MultipleClassesTest", cmdArgs);
}

void ForceXUnitXMLQTestLibParserFactoryTest::testSignalsTest(void)
{
    QFETCH(QStringList, cmdArgs);

    runTest("SignalsTest", cmdArgs);
}

void ForceXUnitXMLQTestLibParserFactoryTest::testLimits(void)
{
    QFETCH(QStringList, cmdArgs);

    runTest("LimitsTest", cmdArgs);
}

void ForceXUnitXMLQTestLibParserFactoryTest::testOneSubTest(void)
{
    QFETCH(Internal::QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(Internal::QTestLibArgsParser::TestVerbosity, verbosity);

    runMakeCheck("OneSubTest", format, verbosity);
}

void ForceXUnitXMLQTestLibParserFactoryTest::testTwoSubTests(void)
{
    QFETCH(Internal::QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(Internal::QTestLibArgsParser::TestVerbosity, verbosity);

    runMakeCheck("TwoSubTests", format, verbosity);
}

void ForceXUnitXMLQTestLibParserFactoryTest::runTest(const QString& testName, const QStringList& cmdArgs)
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
    ProjectExplorer::ArgumentsAspect* argsAspect = testRunConfig->aspect<ProjectExplorer::ArgumentsAspect>();
    QVERIFY(argsAspect != nullptr);
    argsAspect->setArguments(cmdArgs.join(QLatin1Char(' ')));
    testFactory(testRunConfig);
}

void ForceXUnitXMLQTestLibParserFactoryTest::runMakeCheck(const QString& testName, Internal::QTestLibArgsParser::TestOutputFormat format, Internal::QTestLibArgsParser::TestVerbosity verbosity)
{
    QVERIFY(openQMakeProject(TESTS_DIR "/" + testName + "/" + testName + ".pro", &mProject));

    // Retrieve RunConfiguration:
    ProjectExplorer::RunConfiguration* testRunConfig = NULL;
    foreach (ProjectExplorer::RunConfiguration* runConfig, mProject->activeTarget()->runConfigurations()) {
        if (runConfig->id() != Core::Id(Constants::TestRunConfigurationId))
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
    ProjectExplorer::Runnable modifiedRunnable = testRunConfig->runnable();
    Internal::QTestLibArgsParser testArgsParser;
    testArgsParser.setOutputFormat(format);
    testArgsParser.setVerbosity(verbosity);
    QString expectedCmdArgs(QLatin1String("-f " TESTS_DIR "/") + testName + QLatin1String("/Makefile check"));
    if (!testArgsParser.toString().isEmpty())
        expectedCmdArgs.append(QString(QLatin1String(" TESTARGS=\"%1\"")).arg(testArgsParser.toString()));
    QCOMPARE(modifiedRunnable.commandLineArguments, expectedCmdArgs);

    testFactory(testRunConfig);
}

void ForceXUnitXMLQTestLibParserFactoryTest::testFactory(ProjectExplorer::RunConfiguration* testRunConfig)
{
    // Retrieve factory:
    QLinkedList<QTestLibPlugin::Internal::AbstractTestParserFactory*> parserFactories = QTestLibPlugin::Internal::TestModelFactory::parserFactories(Core::Id(QTestLibPlugin::Constants::XUnitXmlQTestLibParserFactoryId).withSuffix(QTestLibPlugin::Constants::BaseForceParserFactoryId));
    QCOMPARE(parserFactories.size(), 1);
    QTestLibPlugin::Internal::XUnitXMLQTestLibParserFactory<QTestLibPlugin::Internal::BaseForceParserFactory>* parserFactory = dynamic_cast<QTestLibPlugin::Internal::XUnitXMLQTestLibParserFactory<QTestLibPlugin::Internal::BaseForceParserFactory>*>(parserFactories.first());
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
        if ((format == Internal::QTestLibArgsParser::XUnitXmlFormat) || (format == Internal::QTestLibArgsParser::AllFormats)) {
            QVERIFY(parserFactory->canParse(testRunConfig));
            parser = parserFactory->getParserInstance(testRunConfig);
            bool parserCreated = (parser != NULL);
            if (parserCreated)
                delete parser;
            QVERIFY(parserCreated);
        } else {
            QVERIFY(!parserFactory->canParse(testRunConfig));
            parser = parserFactory->getParserInstance(testRunConfig);
            bool parserCreated = (parser != NULL);
            if (parserCreated)
                delete parser;
            QVERIFY(!parserCreated);
        }
    }
}

} // Test
} // QTestLibPlugin
