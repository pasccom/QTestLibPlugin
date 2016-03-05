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

#include "plaintextqtestlibparserfactoryfake.h"

#include "../common/qtestlibmodeltester.h"

#include <projectexplorer/localapplicationrunconfigurationfake.h>
#include <projectexplorer/localapplicationruncontrolfake.h>
#include <projectexplorer/target.h>

#include <utils/hostosinfo.h>

#include <QtTest>

class PlainTextQTestLibParserTest : public QObject
{
    Q_OBJECT
public:
    inline PlainTextQTestLibParserTest(void) {qsrand(QDateTime::currentMSecsSinceEpoch());}
private Q_SLOTS:
    inline void oneClass_data(void) {data();}
    void oneClass(void);
    inline void allMessages_data(void) {data();}
    void allMessages(void);
    inline void multipleClasses_data(void) {data();}
    void multipleClasses(void);
    inline void signalsTest_data(void) {data();}
    void signalsTest(void);
    inline void limits_data(void) {data();}
    void limits(void);
    inline void oneSubTest_data(void) {data();}
    void oneSubTest(void);
    inline void twoSubTests_data(void) {data();}
    void twoSubTests(void);
private:
    void data(void);
    QStringList commandLineArguments(QTestLibModelTester::Verbosity verbosity);
    void runTest(const QString& testName, QTestLibModelTester::Verbosity verbosity = QTestLibModelTester::Normal);
    void runMakeCheck(const QString& testName, QTestLibModelTester::Verbosity verbosity = QTestLibModelTester::Normal);
    void checkTest(const QAbstractItemModel *model, QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results, const QString& testName, QTestLibModelTester::Verbosity verbosity);
    QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> executeTest(QTestLibPlugin::Internal::AbstractTestParser *parser, ProjectExplorer::LocalApplicationRunConfiguration* runConfig);
};

void PlainTextQTestLibParserTest::data(void)
{
    QTest::addColumn<QTestLibModelTester::Verbosity>("verbosity");

    QTest::newRow("Normal") << QTestLibModelTester::Normal;
    QTest::newRow("Silent") << QTestLibModelTester::Silent;
    QTest::newRow("Verbose1") << QTestLibModelTester::Verbose1;
    QTest::newRow("Verbose2") << QTestLibModelTester::Verbose2;
    QTest::newRow("VerboseS") << QTestLibModelTester::VerboseSignal;
}

void PlainTextQTestLibParserTest::oneClass(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runTest("OneClassTest", verbosity);
}

void PlainTextQTestLibParserTest::allMessages(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runTest("AllMessagesTest", verbosity);
}

void PlainTextQTestLibParserTest::multipleClasses(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runTest("MultipleClassesTest", verbosity);
}

void PlainTextQTestLibParserTest::signalsTest(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runTest("SignalsTest", verbosity);
}

void PlainTextQTestLibParserTest::limits(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runTest("LimitsTest", verbosity);
}

void PlainTextQTestLibParserTest::oneSubTest(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runMakeCheck("OneSubTest", verbosity);
}

void PlainTextQTestLibParserTest::twoSubTests(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runMakeCheck("TwoSubTests", verbosity);
}

QStringList PlainTextQTestLibParserTest::commandLineArguments(QTestLibModelTester::Verbosity verbosity)
{
    QStringList cmdArgs;
    switch(verbosity) {
    case QTestLibModelTester::Silent:
        cmdArgs << "-silent";
        break;
    case QTestLibModelTester::Normal:
        break;
    case QTestLibModelTester::Verbose1:
        cmdArgs << "-v1";
        break;
    case QTestLibModelTester::Verbose2:
        cmdArgs << "-v2";
        break;
    case QTestLibModelTester::VerboseSignal:
        cmdArgs << "-vs";
        break;
    default:
        qWarning() << "Sentinel value VerbosityCountMinusOne must not be used as verbosity.";
        break;
    }
    int argsVersion = qrand() % 3;
    if (argsVersion == 1)
        cmdArgs << "-txt";
    else if (argsVersion == 2)
        cmdArgs << "-o -,txt";

    return cmdArgs;
}

void PlainTextQTestLibParserTest::runTest(const QString& testName, QTestLibModelTester::Verbosity verbosity)
{
    // Creation of RunConfiguration
    ProjectExplorer::Target target(NULL, NULL);
    ProjectExplorer::LocalApplicationRunConfigurationFake runConfig(&target);
    runConfig.setDisplayName(testName);
    runConfig.setWorkingDirectory(TESTS_DIR "/" + testName + "/");
    runConfig.setExecutable(Utils::HostOsInfo::withExecutableSuffix(TESTS_DIR "/" + testName + "/debug/" + testName));
    runConfig.setCommandLineArguments(commandLineArguments(verbosity).join(' '));

    // Creation of parser
    QTestLibPlugin::Internal::PlainTextQTestLibParserFactory factory(this);
    QVERIFY2(factory.canParse(&runConfig), "Factory should parse this test");
    QTestLibPlugin::Internal::AbstractTestParser* parser = factory.getParserInstance(&runConfig);
    QVERIFY2(parser, "Factory should return a valid parser");

    QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results = executeTest(parser, &runConfig);
    QAbstractItemModel *model = parser->getModel();

    checkTest(model, results, testName, verbosity);

    delete model;
    delete parser;
}

void PlainTextQTestLibParserTest::runMakeCheck(const QString& testName, QTestLibModelTester::Verbosity verbosity)
{
    // Creation of RunConfiguration
    ProjectExplorer::Target target(NULL, NULL);
    ProjectExplorer::LocalApplicationRunConfigurationFake runConfig(&target);
    runConfig.setDisplayName(testName);
    runConfig.setWorkingDirectory(TESTS_DIR "/" + testName + "/");
    runConfig.setExecutable(MAKE_EXECUATBLE);
    runConfig.setCommandLineArguments(QString("-s check TESTARGS=\"%1\"").arg(commandLineArguments(verbosity).join(' ')));

    // Creation of parser
    QTestLibPlugin::Internal::PlainTextQTestLibParserFactory factory(this);
    QVERIFY2(factory.canParse(&runConfig), "Factory should parse this test");
    QTestLibPlugin::Internal::AbstractTestParser* parser = factory.getParserInstance(&runConfig);
    QVERIFY2(parser, "Factory should return a valid parser");

    QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results = executeTest(parser, &runConfig);
    QAbstractItemModel *model = parser->getModel();

    checkTest(model, results, testName, verbosity);

    delete model;
    delete parser;
}

void PlainTextQTestLibParserTest::checkTest(const QAbstractItemModel *model, QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results, const QString& testName, QTestLibModelTester::Verbosity verbosity)
{
    QTestLibModelTester tester(model, verbosity, "txt");
    QVERIFY2(tester.checkResults(results, testName), qPrintable(tester.error()));
    QVERIFY2(tester.checkIndex(QModelIndex(), testName), qPrintable(tester.error()));
}

QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> PlainTextQTestLibParserTest::executeTest(QTestLibPlugin::Internal::AbstractTestParser *parser, ProjectExplorer::LocalApplicationRunConfiguration* runConfig)
{
    QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results;
    QProcess testProc(this);
    testProc.setWorkingDirectory(runConfig->workingDirectory());
    testProc.start(runConfig->executable() + " " + runConfig->commandLineArguments(), QIODevice::ReadOnly);

    if (!testProc.waitForFinished(30000)) {
        qCritical() << "Test timed out";
        return results;
    }

    ProjectExplorer::RunControl *runControl = new ProjectExplorer::LocalApplicationRunControlFake(runConfig);

    testProc.setReadChannel(QProcess::StandardOutput);
    while (!testProc.atEnd()) {
        QString line = testProc.readLine();
        while (line.endsWith('\n') || line.endsWith('\r'))
            line.chop(1);
        // Lines added by make (even in silent mode)
        if (line.startsWith("Makefile:"))
            continue;
        if (line.startsWith("make["))
            continue;
        //qDebug() << "stdout:" << line;
        results << parser->parseStdoutLine(runControl, line);
    }

    testProc.setReadChannel(QProcess::StandardError);
    while (!testProc.atEnd()) {
        QString line = testProc.readLine();
        while (line.endsWith('\n') || line.endsWith('\r'))
            line.chop(1);
        if (line.isEmpty())
            continue;
        // Lines added by windows
        if (QString::compare(line, "This application has requested the Runtime to terminate it in an unusual way.") == 0)
            continue;
        if (QString::compare(line, "Please contact the application's support team for more information.") == 0)
            continue;
        // Lines added by make (even in silent mode)
        if (line.startsWith("make:"))
            continue;
        if (line.startsWith("make["))
            continue;
        //qDebug() << "stderr:" << line;
        results << parser->parseStderrLine(runControl, line);
    }

    return results;
}

QTEST_APPLESS_MAIN(PlainTextQTestLibParserTest)

#include "plaintextqtestlibparsertest.moc"
