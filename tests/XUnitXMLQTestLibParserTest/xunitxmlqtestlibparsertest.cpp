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

#include "xunitxmlqtestlibparserfactoryfake.h"

#include "../common/qtestlibmodeltester.h"

#include <projectexplorer/runconfiguration.h>

#include <utils/hostosinfo.h>

#include <QtTest>

typedef QPair<QString, QString> EnvironmentVariable;

class XUnitXMLQTestLibParserTest : public QObject
{
    Q_OBJECT
public:
    inline XUnitXMLQTestLibParserTest(void) {qsrand(QDateTime::currentMSecsSinceEpoch());}
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
    QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> executeTest(QTestLibPlugin::Internal::AbstractTestParser *parser, ProjectExplorer::Runnable runnable, const QLinkedList<EnvironmentVariable> &addToEnv = QLinkedList<EnvironmentVariable>());
};

void XUnitXMLQTestLibParserTest::data(void)
{
    QTest::addColumn<QTestLibModelTester::Verbosity>("verbosity");

    QTest::newRow("Normal") << QTestLibModelTester::Normal;
    QTest::newRow("Silent") << QTestLibModelTester::Silent;
    QTest::newRow("Verbose1") << QTestLibModelTester::Verbose1;
    QTest::newRow("Verbose2") << QTestLibModelTester::Verbose2;
    QTest::newRow("VerboseS") << QTestLibModelTester::VerboseSignal;
}

void XUnitXMLQTestLibParserTest::oneClass(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runTest("OneClassTest", verbosity);
}

void XUnitXMLQTestLibParserTest::allMessages(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runTest("AllMessagesTest", verbosity);
}

void XUnitXMLQTestLibParserTest::multipleClasses(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runTest("MultipleClassesTest", verbosity);
}

void XUnitXMLQTestLibParserTest::signalsTest(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runTest("SignalsTest", verbosity);
}

void XUnitXMLQTestLibParserTest::limits(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runTest("LimitsTest", verbosity);
}

void XUnitXMLQTestLibParserTest::oneSubTest(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runMakeCheck("OneSubTest", verbosity);
}

void XUnitXMLQTestLibParserTest::twoSubTests(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runMakeCheck("TwoSubTests", verbosity);
}

QStringList XUnitXMLQTestLibParserTest::commandLineArguments(QTestLibModelTester::Verbosity verbosity)
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
    if (qrand() % 2 == 1)
        cmdArgs << "-xunitxml";
    else
        cmdArgs << "-o -,xunitxml";

    return cmdArgs;
}

void XUnitXMLQTestLibParserTest::runTest(const QString& testName, QTestLibModelTester::Verbosity verbosity)
{
    // Creation of Runnable
    ProjectExplorer::Runnable runnable;
    runnable.workingDirectory = TESTS_DIR "/" + testName + "/";
    runnable.executable = Utils::HostOsInfo::withExecutableSuffix(TESTS_DIR "/" + testName + "/debug/" + testName);
    runnable.commandLineArguments = commandLineArguments(verbosity).join(' ');

    // Creation of parser
    QTestLibPlugin::Internal::XUnitXMLQTestLibParserFactory<Fake> factory;
    QTestLibPlugin::Internal::AbstractTestParser* parser = factory.getParserInstance(nullptr);
    QVERIFY2(parser, "Factory should return a valid parser");

    QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results = executeTest(parser, runnable);
    QAbstractItemModel *model = parser->getModel();

    checkTest(model, results, testName, qMax(QTestLibModelTester::Normal, verbosity)); // NOTE When running in XUnitXML silent is equal to normal

    delete model;
    delete parser;
}

void XUnitXMLQTestLibParserTest::runMakeCheck(const QString& testName, QTestLibModelTester::Verbosity verbosity)
{
    // Creation of Runnable
    ProjectExplorer::Runnable runnable;
    runnable.workingDirectory = TESTS_DIR "/" + testName + "/";
    runnable.executable = MAKE_EXECUATBLE;
    runnable.commandLineArguments = "-s check";
    QLinkedList<EnvironmentVariable> addToEnv;
    addToEnv << EnvironmentVariable("TESTARGS", commandLineArguments(verbosity).join(' '));

    // Creation of parser
    QTestLibPlugin::Internal::XUnitXMLQTestLibParserFactory<Fake> factory;
    QTestLibPlugin::Internal::AbstractTestParser* parser = factory.getParserInstance(nullptr);
    QVERIFY2(parser, "Factory should return a valid parser");

    QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results = executeTest(parser, runnable, addToEnv);
    QAbstractItemModel *model = parser->getModel();

    checkTest(model, results, testName, qMax(QTestLibModelTester::Normal, verbosity)); // NOTE When running in XUnitXML silent is equal to normal

    delete model;
    delete parser;
}

void XUnitXMLQTestLibParserTest::checkTest(const QAbstractItemModel *model, QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results, const QString& testName, QTestLibModelTester::Verbosity verbosity)
{
    QTestLibModelTester tester(model, verbosity, "xunitxml");
    tester.setResultsFile(TESTS_DIR "/" + testName + "/" + testName.toLower() + ".xml");
    QVERIFY2(tester.checkResults(results), qPrintable(tester.error()));
    QVERIFY2(tester.checkIndex(QModelIndex()), qPrintable(tester.error()));
}

QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> XUnitXMLQTestLibParserTest::executeTest(QTestLibPlugin::Internal::AbstractTestParser *parser, ProjectExplorer::Runnable runnable, const QLinkedList<EnvironmentVariable> &addToEnv)
{
    QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results;

    QProcessEnvironment env;
    foreach(EnvironmentVariable var, addToEnv)
        env.insert(var.first, var.second);
    QProcess testProc(this);
    testProc.setWorkingDirectory(runnable.workingDirectory);
    testProc.setProcessEnvironment(env);
    testProc.start(runnable.executable + " " + runnable.commandLineArguments, QIODevice::ReadOnly);

    if (!testProc.waitForFinished(30000)) {
        qCritical() << "Test timed out";
        return results;
    }

    testProc.setReadChannel(QProcess::StandardOutput);
    while (!testProc.atEnd()) {
        QString line = QString::fromLocal8Bit(testProc.readLine());
        while (line.endsWith('\n') || line.endsWith('\r'))
            line.chop(1);
        if (line.startsWith("Makefile"))
            continue;
        if (line.startsWith("make[") || line.startsWith("mingw32-make["))
            continue;
        //qDebug() << "stdout:" << line;
        results << parser->parseStdoutLine(nullptr, line);
    }

    testProc.setReadChannel(QProcess::StandardError);
    while (!testProc.atEnd()) {
        QString line = QString::fromLocal8Bit(testProc.readLine());
        while (line.endsWith('\n') || line.endsWith('\r'))
            line.chop(1);
        if (line.isEmpty())
            continue;
        // Lines added by windows
        if (QString::compare(line, "This application has requested the Runtime to terminate it in an unusual way.") == 0)
            continue;
        if (QString::compare(line, "Please contact the application's support team for more information.") == 0)
            continue;
        if (line.startsWith("make:") || line.startsWith("mingw32-make:"))
            continue;
        if (line.startsWith("make[") || line.startsWith("mingw32-make["))
            continue;
        //qDebug() << "stderr:" << line;
        results << parser->parseStderrLine(nullptr, line);
    }

    return results;
}

QTEST_APPLESS_MAIN(XUnitXMLQTestLibParserTest)

#include "xunitxmlqtestlibparsertest.moc"
