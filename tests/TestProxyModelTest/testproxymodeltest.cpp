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

#include "xmlqtestlibparserfactoryfake.h"

#include <testproxymodel.h>

#include "../common/qtestlibmodeltester.h"

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

    inline void oneClass_data(void) {data();}
    void oneClass(void);
    inline void allMessages_data(void) {data();}
    void allMessages(void);
    inline void multipleclasses_data(void) {data();}
    void multipleclasses(void);
    inline void signalsTest_data(void) {data();}
    void signalsTest(void);
    inline void limits_data(void) {data();}
    void limits(void);

private:
    void data(void);
    void addRow(const QString& row);
    QString commandLineArguments(void);
    void createModel(const QString& testName);
    void testProxy(const QString& testName, QVector<bool> filter);

    QTestLibPlugin::Internal::TestProxyModel *mProxy;
    QMap<QString, QAbstractItemModel*> mModelMap;
    QTestLibModelTester::Verbosity mVerbosity;
};

TestProxyModelTest::TestProxyModelTest(void) :
    mProxy(NULL)
{
    qsrand(QDateTime::currentMSecsSinceEpoch());
    mVerbosity = QTestLibModelTester::Verbose2;
}

TestProxyModelTest::~TestProxyModelTest(void)
{
    for(QMap<QString, QAbstractItemModel*>::iterator it = mModelMap.begin(); it != mModelMap.end();) {
        delete it.value();
        it = mModelMap.erase(it);
    }

    if (mProxy != NULL)
        delete mProxy;
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

void TestProxyModelTest::initTestCase(void)
{
    // NOTE initTestCase cannot have a _data() function
    QStringList testNames;
    testNames << "OneClassTest";
    testNames << "AllMessagesTest";
    testNames << "MultipleClassesTest";
    testNames << "SignalsTest";
    testNames << "LimitsTest";

    foreach (QString testName, testNames) {
        createModel(testName);
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

void TestProxyModelTest::oneClass(void)
{
    QFETCH(QVector<bool>, filter);

    testProxy("OneClassTest", filter);
}

void TestProxyModelTest::allMessages(void)
{
    QFETCH(QVector<bool>, filter);

    testProxy("AllMessagesTest", filter);
}

void TestProxyModelTest::multipleclasses(void)
{
    QFETCH(QVector<bool>, filter);

    testProxy("MultipleClassesTest", filter);
}

void TestProxyModelTest::signalsTest(void)
{
    QFETCH(QVector<bool>, filter);

    testProxy("SignalsTest", filter);
}

void TestProxyModelTest::limits(void)
{
    QFETCH(QVector<bool>, filter);

    testProxy("LimitsTest", filter);
}

QString TestProxyModelTest::commandLineArguments(void)
{
    switch(mVerbosity) {
    case QTestLibModelTester::Silent:
        return "-xml -silent";
    case QTestLibModelTester::Normal:
        break;
    case QTestLibModelTester::Verbose1:
        return "-xml -v1";
    case QTestLibModelTester::Verbose2:
        return "-xml -v2";
    case QTestLibModelTester::VerboseSignal:
        return "-xml -vs";
    default:
        qWarning() << "Sentinel value VerbosityCountMinusOne must not be used as verbosity.";
        break;
    }

    return "-xml";
}

void TestProxyModelTest::createModel(const QString& testName)
{
    BEGIN_SUB_TEST_FUNCTION

    // Creation of parser
    QTestLibPlugin::Internal::XMLQTestLibParserFactory factory(this);
    QTestLibPlugin::Internal::AbstractTestParser* parser = factory.getParserInstance(nullptr);
    QVERIFY2(parser, "Factory should return a valid parser");

    // Execute test
    QProcess testProc(this);
    testProc.setWorkingDirectory(TESTS_DIR "/" + testName + "/");
    testProc.start(Utils::HostOsInfo::withExecutableSuffix(TESTS_DIR "/" + testName + "/debug/" + testName) + " " + commandLineArguments(), QIODevice::ReadOnly);
    if (!testProc.waitForFinished(30000)) {
        testProc.terminate();
        QVERIFY(testProc.waitForFinished());
        QSKIP("This computer is too slow for this test");
    }

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
        parser->parseStdoutLine(nullptr, line);
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
        parser->parseStderrLine(nullptr, line);
    }

    // Get and test model
    QAbstractItemModel *model = parser->getModel();
    QVERIFY(model != NULL);
    QVERIFY(!mModelMap.contains(testName));
    mModelMap.insert(testName, model);
    mProxy = new QTestLibPlugin::Internal::TestProxyModel(this);
    mProxy->setSourceModel(model);

    QTestLibModelTester tester(model, mVerbosity, "xml");
    tester.setResultsFile(TESTS_DIR "/" + testName + "/" + testName.toLower() + ".xml");
    QVERIFY2(tester.checkIndex(QModelIndex()), qPrintable(tester.error()));

    END_SUB_TEST_FUNCTION
}

void TestProxyModelTest::testProxy(const QString& testName, QVector<bool> filter)
{
    QAbstractItemModel* model = mModelMap.value(testName, NULL);
    QVERIFY(model != NULL);
    mProxy->setSourceModel(model);
    QTestLibModelTester tester(mProxy, mVerbosity, "xml");
    tester.setResultsFile(TESTS_DIR "/" + testName + "/" + testName.toLower() + ".xml");

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

    QVERIFY2(tester.checkIndex(QModelIndex()), qPrintable(tester.error()));
}

QTEST_MAIN(TestProxyModelTest)

#include "testproxymodeltest.moc"
