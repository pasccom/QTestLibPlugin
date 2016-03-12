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

#include <testproxymodel.h>

#include "../common/qtestlibmodeltester.h"

#include <projectexplorer/localapplicationrunconfigurationfake.h>
#include <projectexplorer/localapplicationruncontrolfake.h>
#include <projectexplorer/target.h>

#include <utils/hostosinfo.h>

#include <QtTest>

class TestProxyModelTest : public QObject
{
    Q_OBJECT
    HAS_SUB_TEST_FUNCTIONS
public:
    TestProxyModelTest(void);
    ~TestProxyModelTest(void);
private Q_SLOTS:
    void initTestCase(void);

    inline void enableDisableToogleMessageType_data(void) {data();}
    void enableDisableToogleMessageType(void);
    inline void modelProxy_data(void) {data();}
    void modelProxy(void);
private:
    void data(void);
    void addRow(const QString& row);
    QString commandLineArguments(void);

    QTestLibPlugin::Internal::TestProxyModel *mProxy;
    QString mTestName;
    QTestLibModelTester::Verbosity mVerbosity;
};

TestProxyModelTest::TestProxyModelTest(void) :
    mProxy(NULL)
{
    qsrand(QDateTime::currentMSecsSinceEpoch());
    mTestName = "AllMessagesTest";
    mVerbosity = QTestLibModelTester::Verbose2;
}

TestProxyModelTest::~TestProxyModelTest(void)
{
    if (mProxy) {
        delete mProxy->sourceModel();
        delete mProxy;
    }
}

QString TestProxyModelTest::commandLineArguments(void)
{
    switch(mVerbosity) {
    case QTestLibModelTester::Silent:
        return "-silent";
    case QTestLibModelTester::Normal:
        break;
    case QTestLibModelTester::Verbose1:
        return "-v1";
    case QTestLibModelTester::Verbose2:
        return "-v2";
    case QTestLibModelTester::VerboseSignal:
        return "-vs";
    default:
        qWarning() << "Sentinel value VerbosityCountMinusOne must not be used as verbosity.";
        break;
    }

    return QString::null;
}

void TestProxyModelTest::initTestCase(void)
{
    // Creation of RunConfiguration
    ProjectExplorer::Target target(NULL, NULL);
    ProjectExplorer::LocalApplicationRunConfigurationFake runConfig(&target);
    runConfig.setDisplayName(mTestName);
    runConfig.setWorkingDirectory(TESTS_DIR "/" + mTestName + "/");
    runConfig.setExecutable(Utils::HostOsInfo::withExecutableSuffix(TESTS_DIR "/" + mTestName + "/debug/" + mTestName));
    runConfig.setCommandLineArguments(commandLineArguments());

    // Creation of parser
    QTestLibPlugin::Internal::PlainTextQTestLibParserFactory factory(this);
    QVERIFY2(factory.canParse(&runConfig), "Factory should parse this test");
    QTestLibPlugin::Internal::AbstractTestParser* parser = factory.getParserInstance(&runConfig);
    QVERIFY2(parser, "Factory should return a valid parser");

    // Execute test
    QProcess testProc(this);
    testProc.setWorkingDirectory(runConfig.workingDirectory());
    testProc.start(runConfig.executable() + " " + runConfig.commandLineArguments(), QIODevice::ReadOnly);
    if (!testProc.waitForFinished(30000)) {
        testProc.terminate();
        QVERIFY(testProc.waitForFinished());
        QSKIP("This computer is too slow for this test");
    }

    ProjectExplorer::RunControl *runControl = new ProjectExplorer::LocalApplicationRunControlFake(&runConfig);

    // Populate model with stdout
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
        parser->parseStdoutLine(runControl, line);
    }

    // Populate model with stderr
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
        parser->parseStderrLine(runControl, line);
    }

    // Get and test model
    QAbstractItemModel *model = parser->getModel();
    mProxy = new QTestLibPlugin::Internal::TestProxyModel(this);
    mProxy->setSourceModel(model);

    QTestLibModelTester tester(model, mVerbosity, "txt");
    QVERIFY2(tester.checkIndex(QModelIndex(), mTestName), qPrintable(tester.error()));
}

void TestProxyModelTest::addRow(const QString& row)
{
    QVector<bool> filter;
    filter.resize(row.length());
    for (int i = 0; i < row.length(); i++)
        filter[i] = (row.at(i) == '1');
    QTest::newRow(qPrintable(row)) << filter;
}

void TestProxyModelTest::data(void)
{
    QTest::addColumn< QVector<bool> >("filter");

    QString rowTrue = "11111111111111111";
    QString rowFalse = "00000000000000000";

    addRow(rowTrue);
    for (int i1 = 0; i1 < rowTrue.length(); i1++) {
        for (int i2 = i1; i2 < rowTrue.length(); i2++) {
            QString row = rowTrue;
            row.replace(i1, 1, '0');
            row.replace(i2, 1, '0');
            addRow(row);
        }
    }

    addRow(rowFalse);
    for (int i1 = 0; i1 < rowTrue.length(); i1++) {
        for (int i2 = i1; i2 < rowTrue.length(); i2++) {
            QString row = rowFalse;
            row.replace(i1, 1, '1');
            row.replace(i2, 1, '1');
            addRow(row);
        }
    }
}

void TestProxyModelTest::enableDisableToogleMessageType(void)
{
    QFETCH(QVector<bool>, filter);

    for (int i = 0; i < filter.length(); i++) {
        if (filter.at(i))
            mProxy->enableMessageType((QTestLibPlugin::Internal::QTestLibModel::MessageType) i);
        else
            mProxy->disableMessageType((QTestLibPlugin::Internal::QTestLibModel::MessageType) i);
    }

    for (int i = 0; i < filter.length(); i++)
        QCOMPARE(mProxy->isMessageTypeEnabled((QTestLibPlugin::Internal::QTestLibModel::MessageType) i), filter.at(i));

    for (int i = 0; i < filter.length(); i++)
        mProxy->toogleMessageType((QTestLibPlugin::Internal::QTestLibModel::MessageType) i);

    for (int i = 0; i < filter.length(); i++)
        QCOMPARE(mProxy->isMessageTypeEnabled((QTestLibPlugin::Internal::QTestLibModel::MessageType) i), !filter.at(i));

    for (int i = 0; i < filter.length(); i++)
        mProxy->toogleMessageType((QTestLibPlugin::Internal::QTestLibModel::MessageType) i);

    for (int i = 0; i < filter.length(); i++)
        QCOMPARE(mProxy->isMessageTypeEnabled((QTestLibPlugin::Internal::QTestLibModel::MessageType) i), filter.at(i));
}

void TestProxyModelTest::modelProxy(void)
{
    QFETCH(QVector<bool>, filter);

    QTestLibModelTester tester(mProxy, mVerbosity, "txt");
    for (int i = 0; i < filter.length(); i++) {
        if (filter.at(i)) {
            mProxy->enableMessageType((QTestLibPlugin::Internal::QTestLibModel::MessageType) i);
            tester.enableMessageType((QTestLibPlugin::Internal::QTestLibModel::MessageType) i);
        } else {
            mProxy->disableMessageType((QTestLibPlugin::Internal::QTestLibModel::MessageType) i);
            tester.disableMessageType((QTestLibPlugin::Internal::QTestLibModel::MessageType) i);
        }
    }

    for (int i = 0; i < filter.length(); i++)
        QCOMPARE(mProxy->isMessageTypeEnabled((QTestLibPlugin::Internal::QTestLibModel::MessageType) i), filter.at(i));

    QVERIFY2(tester.checkIndex(QModelIndex(), mTestName), qPrintable(tester.error()));
}

QTEST_MAIN(TestProxyModelTest)

#include "testproxymodeltest.moc"
