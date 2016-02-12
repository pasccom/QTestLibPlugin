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

#include <xmlqtestlibparserfactory.h>

#include <projectexplorer/localapplicationrunconfiguration.h>
#include <projectexplorer/localapplicationruncontrol.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/project.h>
#include <qmakeprojectmanager/qmakeproject.h>
#include <qmakeprojectmanager/qmakeprojectmanager.h>

#include "../common/qtestlibmodeltester.h"

#include <QtTest>

class XMLQTestLibParserTest : public QObject
{
    Q_OBJECT
public:
    inline XMLQTestLibParserTest(void) {qsrand(QDateTime::currentMSecsSinceEpoch()); QmakeProjectManager::QmakeProjectManager::initialize();}
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
private:
    void data(void);
    void runTest(const QString& testName, QTestLibModelTester::Verbosity verbosity = QTestLibModelTester::Normal);
    void checkTest(const QAbstractItemModel *model, QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results, const QString& testName);
    QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> executeTest(QTestLibPlugin::Internal::AbstractTestParser *parser, ProjectExplorer::LocalApplicationRunConfiguration *runConfig);

    QTestLibModelTester::Verbosity mVerbosity;
};

void XMLQTestLibParserTest::data(void)
{
    QTest::addColumn<QTestLibModelTester::Verbosity>("verbosity");

    QTest::newRow("Normal") << QTestLibModelTester::Normal;
    QTest::newRow("Silent") << QTestLibModelTester::Silent;
    QTest::newRow("Verbose1") << QTestLibModelTester::Verbose1;
    QTest::newRow("Verbose2") << QTestLibModelTester::Verbose2;
    QTest::newRow("VerboseS") << QTestLibModelTester::VerboseSignal;
}

void XMLQTestLibParserTest::oneClass(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runTest("OneClassTest", verbosity);
}

void XMLQTestLibParserTest::allMessages(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runTest("AllMessagesTest", verbosity);
}

void XMLQTestLibParserTest::multipleClasses(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runTest("MultipleClassesTest", verbosity);
}

void XMLQTestLibParserTest::signalsTest(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runTest("SignalsTest", verbosity);
}

void XMLQTestLibParserTest::limits(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    runTest("LimitsTest", verbosity);
}


void XMLQTestLibParserTest::runTest(const QString& testName, QTestLibModelTester::Verbosity verbosity)
{
    // Creation of RunConfiguration
    QmakeProjectManager::QmakeProject project(TESTS_DIR "/" + testName + "/" + testName + ".pro", this);
    ProjectExplorer::Kit kit;
    ProjectExplorer::Target target(&project, &kit);
    ProjectExplorer::LocalApplicationRunConfiguration runConfig(&target);
    runConfig.setExecutable(testName + ".exe");
    if (qrand() % 2 == 1)
        runConfig.setCommandLineArguments("-xml");
    else
        runConfig.setCommandLineArguments("-o -,xml");

    // Creation of parser
    QTestLibPlugin::Internal::XMLQTestLibParserFactory factory(this);
    QVERIFY2(factory.canParse(&runConfig), "Factory should parse this test");
    QTestLibPlugin::Internal::AbstractTestParser* parser = factory.getParserInstance(&runConfig);
    QVERIFY2(parser, "Factory should return a valid parser");

    mVerbosity = verbosity;

    QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results = executeTest(parser, &runConfig);
    QAbstractItemModel *model = parser->getModel();

    mVerbosity = qMax(QTestLibModelTester::Normal, verbosity); // NOTE When running in XML silent is equal to normal
    checkTest(model, results, testName);

    mVerbosity = QTestLibModelTester::Normal;

    delete model;
    delete parser;
}

void XMLQTestLibParserTest::checkTest(const QAbstractItemModel *model, QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results, const QString& testName)
{
    QTestLibModelTester tester(model, (QTestLibModelTester::Verbosity) mVerbosity, "xml");
    QVERIFY2(tester.checkResults(results, testName), qPrintable(tester.error()));
    QVERIFY2(tester.checkIndex(QModelIndex(), testName), qPrintable(tester.error()));
}

QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> XMLQTestLibParserTest::executeTest(QTestLibPlugin::Internal::AbstractTestParser *parser, ProjectExplorer::LocalApplicationRunConfiguration *runConfig)
{
    QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results;
    QStringList cmdArgs;
    cmdArgs << "-o" << QString("-,xml");
    switch(mVerbosity) {
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

    QProcess testProc(this);
    testProc.setWorkingDirectory(TESTS_DIR "/" + runConfig->displayName() + "/");
    testProc.start(TESTS_DIR "/" + runConfig->displayName() + "/debug/" + runConfig->displayName(), cmdArgs, QIODevice::ReadOnly);

    if (!testProc.waitForFinished(30000)) {
        qCritical() << "Test timed out";
        return results;
    }

    ProjectExplorer::RunControl *runControl = new ProjectExplorer::LocalApplicationRunControl(runConfig);

    testProc.setReadChannel(QProcess::StandardOutput);
    while (!testProc.atEnd()) {
        QString line = testProc.readLine();
        while (line.endsWith('\n') || line.endsWith('\r'))
            line.chop(1);
        //qDebug() << line;
        results << parser->parseStdoutLine(runControl, line);
    }

    testProc.setReadChannel(QProcess::StandardError);
    while (!testProc.atEnd()) {
        QString line = testProc.readLine();
        while (line.endsWith('\n') || line.endsWith('\r'))
            line.chop(1);
        //qDebug() << line;
        if (line.isEmpty())
            continue;
        // Lines added by windows
        if (QString::compare(line, "This application has requested the Runtime to terminate it in an unusual way.") == 0)
            continue;
        if (QString::compare(line, "Please contact the application's support team for more information.") == 0)
            continue;
        results << parser->parseStderrLine(runControl, line);
    }

    return results;
}

QTEST_APPLESS_MAIN(XMLQTestLibParserTest)

#include "xmlqtestlibparsertest.moc"
