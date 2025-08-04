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

#include "lightxmlqtestlibparserfactoryfake.h"
#include <lightxmlqtestlibparser.h>

#include <qtestlibmodeltester.h>

#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/runcontrol.h>

#include <utils/hostosinfo.h>
#include <utils/processinterface.h>

#include <QtTest>

typedef QPair<QString, QString> EnvironmentVariable;

class LightXMLQTestLibParserTest : public QObject
{
    Q_OBJECT
public:
    inline LightXMLQTestLibParserTest(void) {mRandom = QRandomGenerator::global();}
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
    void runTest(const QString& qtVersion, const QString& testName, QTestLibModelTester::Verbosity verbosity = QTestLibModelTester::Normal);
    void runMakeCheck(const QString& qtVersion, const QString& testName, QTestLibModelTester::Verbosity verbosity = QTestLibModelTester::Normal);
    void checkTest(const QAbstractItemModel *model, QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results, const QString& qtVersion, const QString& testName, QTestLibModelTester::Verbosity verbosity);
    QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> executeTest(QTestLibPlugin::Internal::AbstractTestParser* parser, const Utils::ProcessRunData& runnable, const QLinkedList<EnvironmentVariable>& addToEnv = QLinkedList<EnvironmentVariable>());

    QRandomGenerator* mRandom;
};

void LightXMLQTestLibParserTest::data(void)
{
    QTest::addColumn<QString>("qtVersion");
    QTest::addColumn<QTestLibModelTester::Verbosity>("verbosity");

    QTest::newRow("Qt5 Normal") << "qt5" << QTestLibModelTester::Normal;
    QTest::newRow("Qt5 Silent") << "qt5" << QTestLibModelTester::Silent;
    QTest::newRow("Qt5 Verbose1") << "qt5" << QTestLibModelTester::Verbose1;
    QTest::newRow("Qt5 Verbose2") << "qt5" << QTestLibModelTester::Verbose2;
    QTest::newRow("Qt5 VerboseS") << "qt5" << QTestLibModelTester::VerboseSignal;
}

void LightXMLQTestLibParserTest::oneClass(void)
{
    QFETCH(QString, qtVersion);
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runTest(qtVersion, "OneClassTest", verbosity);
}

void LightXMLQTestLibParserTest::allMessages(void)
{
    QFETCH(QString, qtVersion);
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runTest(qtVersion, "AllMessagesTest", verbosity);
}

void LightXMLQTestLibParserTest::multipleClasses(void)
{
    QFETCH(QString, qtVersion);
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runTest(qtVersion, "MultipleClassesTest", verbosity);
}

void LightXMLQTestLibParserTest::signalsTest(void)
{
    QFETCH(QString, qtVersion);
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runTest(qtVersion, "SignalsTest", verbosity);
}

void LightXMLQTestLibParserTest::limits(void)
{
    QFETCH(QString, qtVersion);
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runTest(qtVersion, "LimitsTest", verbosity);
}

void LightXMLQTestLibParserTest::oneSubTest(void)
{
    QFETCH(QString, qtVersion);
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runMakeCheck(qtVersion, "OneSubTest", verbosity);
}

void LightXMLQTestLibParserTest::twoSubTests(void)
{
    QFETCH(QString, qtVersion);
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runMakeCheck(qtVersion, "TwoSubTests", verbosity);
}

QStringList LightXMLQTestLibParserTest::commandLineArguments(QTestLibModelTester::Verbosity verbosity)
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
    if (mRandom->bounded(2) == 1)
        cmdArgs << "-lightxml";
    else
        cmdArgs << "-o -,lightxml";

    return cmdArgs;
}

void LightXMLQTestLibParserTest::runTest(const QString& qtVersion, const QString& testName, QTestLibModelTester::Verbosity verbosity)
{
    // Creation of Runnable
    Utils::ProcessRunData runnable;
    runnable.workingDirectory = Utils::FilePath::fromString(TESTS_DIR "/" + qtVersion + "/" + testName + "/");
    runnable.command = Utils::CommandLine(Utils::FilePath::fromString(Utils::HostOsInfo::withExecutableSuffix(TESTS_DIR "/" + qtVersion + "/" + testName + "/debug/" + testName)));
    runnable.command.addArgs(commandLineArguments(verbosity).join(' '), Utils::CommandLine::Raw);
    qDebug() << runnable.command.arguments();

    // Creation of parser
    QTestLibPlugin::Internal::LightXMLQTestLibParserFactory<Fake> factory;
    QTestLibPlugin::Internal::AbstractTestParser* parser = factory.getParserInstance(nullptr);
    QVERIFY2(parser, "Factory should return a valid parser");
    QTestLibPlugin::Internal::LightXMLQTestLibParser* lightXMLParser = qobject_cast<QTestLibPlugin::Internal::LightXMLQTestLibParser*>(parser);
    QVERIFY(lightXMLParser != nullptr);
    lightXMLParser->setDefaultClassName(testName);

    QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results = executeTest(parser, runnable);
    QAbstractItemModel *model = parser->getModel();

    checkTest(model, results, qtVersion, testName, qMax(QTestLibModelTester::Normal, verbosity)); // NOTE When running in LightXML silent is equal to normal

    delete model;
    delete parser;
}

void LightXMLQTestLibParserTest::runMakeCheck(const QString& qtVersion, const QString& testName, QTestLibModelTester::Verbosity verbosity)
{
    // Creation of Runnable
    Utils::ProcessRunData runnable;
    runnable.workingDirectory = Utils::FilePath::fromString(TESTS_DIR "/" + qtVersion + "/" + testName + "/");
    runnable.command = Utils::CommandLine(Utils::FilePath::fromString(MAKE_EXECUATBLE), QStringList() << "-s" << "check");
    QLinkedList<EnvironmentVariable> addToEnv;
    addToEnv << EnvironmentVariable("TESTARGS", commandLineArguments(verbosity).join(' '));

    // Creation of parser
    QTestLibPlugin::Internal::LightXMLQTestLibParserFactory<Fake> factory;
    QTestLibPlugin::Internal::AbstractTestParser* parser = factory.getParserInstance(nullptr);
    QVERIFY2(parser, "Factory should return a valid parser");
    QTestLibPlugin::Internal::LightXMLQTestLibParser* lightXMLParser = qobject_cast<QTestLibPlugin::Internal::LightXMLQTestLibParser*>(parser);
    QVERIFY(lightXMLParser != nullptr);
    lightXMLParser->setDefaultClassName(testName);

    QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results = executeTest(parser, runnable, addToEnv);
    QAbstractItemModel *model = parser->getModel();

    checkTest(model, results, qtVersion, testName, qMax(QTestLibModelTester::Normal, verbosity)); // NOTE When running in LightXML silent is equal to normal

    delete model;
    delete parser;
}

void LightXMLQTestLibParserTest::checkTest(const QAbstractItemModel *model, QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results, const QString& qtVersion, const QString& testName,  QTestLibModelTester::Verbosity verbosity)
{
    QTestLibModelTester tester(model, verbosity, "lightxml");
    tester.setResultsFile(TESTS_DIR "/" + qtVersion + "/" + testName + "/" + testName.toLower() + ".xml");
    QVERIFY2(tester.checkResults(results), qPrintable(tester.error()));
    QVERIFY2(tester.checkIndex(QModelIndex()), qPrintable(tester.error()));
}

QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> LightXMLQTestLibParserTest::executeTest(QTestLibPlugin::Internal::AbstractTestParser* parser, const Utils::ProcessRunData& runnable, const QLinkedList<EnvironmentVariable>& addToEnv)
{
    QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results;

    QProcessEnvironment env;
    foreach(EnvironmentVariable var, addToEnv)
        env.insert(var.first, var.second);
    QProcess testProc(this);
    testProc.setWorkingDirectory(runnable.workingDirectory.toFSPathString());
    testProc.setProcessEnvironment(env);
    testProc.start(runnable.command.executable().toFSPathString(), runnable.command.splitArguments(), QIODevice::ReadOnly);

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
        // FIXME nothing goes in stderr the test is executed in debug mode.
        // For the moment, I will not care on stderr.
        //results << parser->parseStderrLine(nullptr, line);
    }

    return results;
}

QTEST_APPLESS_MAIN(LightXMLQTestLibParserTest)

#include "lightxmlqtestlibparsertest.moc"
