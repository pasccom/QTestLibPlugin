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

#include "../../testproxymodel.h"

#include "../../plaintextqtestlibparserfactory.h"
#include "../../xmlqtestlibparserfactory.h"
#include "../../testmodelfactory.h"

#include "../common/qtestlibmodeltester.h"

#include <utils/outputformat.h>
#include <extensionsystem/pluginmanager.h>
#include <projectexplorer/localapplicationrunconfiguration.h>
#include <projectexplorer/localapplicationruncontrol.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/target.h>
#include <qmakeprojectmanager/qmakeprojectmanager.h>
#include <qmakeprojectmanager/qmakeproject.h>

#include <QtTest>

class TestProxyModelTest : public QObject
{
    Q_OBJECT
    HAS_SUB_TEST_FUNCTIONS
public:
    TestProxyModelTest(void);
    inline ~TestProxyModelTest(void) {qDeleteAll(mManager->allObjects()); delete mManager;}
private Q_SLOTS:
    void cleanup(void);

    inline void oneClassTXT_data(void) {data();}
    void oneClassTXT(void);
    inline void allMessagesTXT_data(void) {data();}
    void allMessagesTXT(void);
    inline void multipleClassesTXT_data(void) {data();}
    void multipleClassesTXT(void);
    inline void signalsTestTXT_data(void) {data();}
    void signalsTestTXT(void);
    inline void limitsTXT_data(void) {data();}
    void limitsTXT(void);

    inline void oneClassXML_data(void) {data();}
    void oneClassXML(void);
    inline void allMessagesXML_data(void) {data();}
    void allMessagesXML(void);
    inline void multipleClassesXML_data(void) {data();}
    void multipleClassesXML(void);
    inline void signalsTestXML_data(void) {data();}
    void signalsTestXML(void);
    inline void limitsXML_data(void) {data();}
    void limitsXML(void);
private slots:
    inline void saveModel(QAbstractItemModel* model) {mModel = model;}
private:
    void data(void);
    void setupProxyAndTester(const QString& format, QTestLibModelTester::Verbosity verbosity = QTestLibModelTester::Normal);
    void runTest(const QString& testName, const QString& format, QTestLibModelTester::Verbosity verbosity = QTestLibModelTester::Normal, Utils::OutputFormat outputFormat = Utils::OutputFormat::StdOutFormat, Utils::OutputFormat errorFormat = Utils::OutputFormat::StdErrFormat);
    void executeTest(const QString& test, const QString& format, QTestLibModelTester::Verbosity verbosity, Utils::OutputFormat outputFormat, Utils::OutputFormat errorFormat);

    QAbstractItemModel *mModel;
    QTestLibPlugin::Internal::TestProxyModel *mProxy;
    QTestLibModelTester *mTester;

    ExtensionSystem::PluginManager *mManager;
};

Q_DECLARE_METATYPE(Utils::OutputFormat)

TestProxyModelTest::TestProxyModelTest(void) :
    mModel(NULL), mProxy(NULL), mTester(NULL)
{
    qsrand(QDateTime::currentMSecsSinceEpoch());
    QmakeProjectManager::QmakeProjectManager::initialize();

    mManager = new ExtensionSystem::PluginManager();
    QTestLibPlugin::Internal::AbstractTestParserFactory *plainTextFactory = new QTestLibPlugin::Internal::PlainTextQTestLibParserFactory(this);
    ExtensionSystem::PluginManager::addObject(plainTextFactory);
    QTestLibPlugin::Internal::AbstractTestParserFactory *xmlFactory = new QTestLibPlugin::Internal::XMLQTestLibParserFactory(this);
    ExtensionSystem::PluginManager::addObject(xmlFactory);
}

void TestProxyModelTest::data(void)
{
    QTest::addColumn< QVector<bool> >("filter");

    QStringList rows;
    // all true
    rows << "11111111111111111";
    // 1 false
    rows << "01111111111111111";
    rows << "10111111111111111";
    rows << "11011111111111111";
    rows << "11101111111111111";
    rows << "11110111111111111";
    rows << "11111011111111111";
    rows << "11111101111111111";
    rows << "11111110111111111";
    rows << "11111111011111111";
    rows << "11111111101111111";
    rows << "11111111110111111";
    rows << "11111111111011111";
    rows << "11111111111101111";
    rows << "11111111111110111";
    rows << "11111111111111011";
    rows << "11111111111111101";
    rows << "11111111111111110";
    // 2 false
    rows << "11111111101011111";
    rows << "11111111111010111";
    rows << "11111111111001111";
    rows << "11111111110011111";
    rows << "11101111111011111";
    // 2 true
    rows << "00000000000000011";
    rows << "00000000000000101";
    rows << "00000000100000001";
    rows << "00000001000000001";
    rows << "00000010000000001";
    // 1 true
    rows << "10000000000000000";
    rows << "01000000000000000";
    rows << "00100000000000000";
    rows << "00010000000000000";
    rows << "00001000000000000";
    rows << "00000100000000000";
    rows << "00000010000000000";
    rows << "00000001000000000";
    rows << "00000000100000000";
    rows << "00000000010000000";
    rows << "00000000001000000";
    rows << "00000000000100000";
    rows << "00000000000010000";
    rows << "00000000000001000";
    rows << "00000000000000100";
    rows << "00000000000000010";
    rows << "00000000000000001";
    // all false
    rows << "00000000000000000";

    foreach (QString row, rows) {
        QVector<bool> filter;
        filter.resize(row.length());
        for (int i = 0; i < row.length(); i++)
            filter[i] = (row.at(i) == '1');
        QTest::newRow(qPrintable(row)) << filter;
    }
}

void TestProxyModelTest::setupProxyAndTester(const QString& format, QTestLibModelTester::Verbosity verbosity)
{
    QFETCH(QVector<bool>, filter);

    qDebug() << filter;

    mProxy = new QTestLibPlugin::Internal::TestProxyModel(this);
    mTester = new QTestLibModelTester(mProxy, verbosity, format);

    int t = QTestLibPlugin::Internal::QTestLibModel::FirstMessageType;
    while (++t < QTestLibPlugin::Internal::QTestLibModel::LastMessageType) {
        if (filter.at(t)) {
            mProxy->enableMessageType((QTestLibPlugin::Internal::QTestLibModel::MessageType) t);
            mTester->enableMessageType((QTestLibPlugin::Internal::QTestLibModel::MessageType) t);
        } else {
            mProxy->disableMessageType((QTestLibPlugin::Internal::QTestLibModel::MessageType) t);
            mTester->disableMessageType((QTestLibPlugin::Internal::QTestLibModel::MessageType) t);
        }
    }
}


void TestProxyModelTest::oneClassTXT(void)
{
    setupProxyAndTester("txt", QTestLibModelTester::VerboseSignal);
    runTest("OneClassTest", "txt", QTestLibModelTester::VerboseSignal);
}

void TestProxyModelTest::allMessagesTXT(void)
{
    setupProxyAndTester("txt", QTestLibModelTester::VerboseSignal);
    runTest("AllMessagesTest", "txt", QTestLibModelTester::VerboseSignal);
}

void TestProxyModelTest::multipleClassesTXT(void)
{
    setupProxyAndTester("txt", QTestLibModelTester::VerboseSignal);
    runTest("MultipleClassesTest", "txt", QTestLibModelTester::VerboseSignal);
}

void TestProxyModelTest::signalsTestTXT(void)
{
    setupProxyAndTester("txt", QTestLibModelTester::VerboseSignal);
    runTest("SignalsTest", "txt", QTestLibModelTester::VerboseSignal);
}


void TestProxyModelTest::limitsTXT(void)
{
    setupProxyAndTester("txt", QTestLibModelTester::VerboseSignal);
    runTest("LimitsTest", "txt", QTestLibModelTester::VerboseSignal);
}

void TestProxyModelTest::oneClassXML(void)
{
    setupProxyAndTester("xml", QTestLibModelTester::VerboseSignal);
    runTest("OneClassTest", "xml", QTestLibModelTester::VerboseSignal);
}

void TestProxyModelTest::allMessagesXML(void)
{
    setupProxyAndTester("xml", QTestLibModelTester::VerboseSignal);
    runTest("AllMessagesTest", "xml", QTestLibModelTester::VerboseSignal);
}

void TestProxyModelTest::multipleClassesXML(void)
{
    setupProxyAndTester("xml", QTestLibModelTester::VerboseSignal);
    runTest("MultipleClassesTest", "xml", QTestLibModelTester::VerboseSignal);
}

void TestProxyModelTest::signalsTestXML(void)
{
    setupProxyAndTester("xml", QTestLibModelTester::VerboseSignal);
    runTest("SignalsTest", "xml", QTestLibModelTester::VerboseSignal);
}

void TestProxyModelTest::limitsXML(void)
{
    setupProxyAndTester("xml", QTestLibModelTester::VerboseSignal);
    runTest("LimitsTest", "xml", QTestLibModelTester::VerboseSignal);
}

void TestProxyModelTest::cleanup(void)
{
    if (mModel)
        delete mModel;
    mModel = NULL;
    if (mProxy)
        delete mProxy;
    mProxy = NULL;
    if (mTester)
        delete mTester;
    mTester = NULL;
}

void TestProxyModelTest::runTest(const QString& testName, const QString& format, QTestLibModelTester::Verbosity verbosity, Utils::OutputFormat outputFormat, Utils::OutputFormat errorFormat)
{
    /* Executes the test and feeds the parser with the result */
    SUB_TEST_FUNCTION(executeTest(testName, format, verbosity, outputFormat, errorFormat));
    QVERIFY2(mModel != NULL, "Model should not be NULL");
    mProxy->setSourceModel(mModel);
    QVERIFY2(mTester->checkIndex(QModelIndex(), testName), qPrintable(mTester->error()));
}

void TestProxyModelTest::executeTest(const QString& test, const QString& format, QTestLibModelTester::Verbosity verbosity, Utils::OutputFormat outputFormat, Utils::OutputFormat errorFormat)
{
    BEGIN_SUB_TEST_FUNCTION

    QString cmdArgs;
    int argsVersion = qrand() % 3;
    if ((format != "txt") && (argsVersion == 0))
        argsVersion = 2;
    if (argsVersion == 1)
        cmdArgs.append(QString("-%1 ").arg(format));
    else if (argsVersion == 2)
        cmdArgs.append(QString("-o -,%1 ").arg(format));

    switch(verbosity) {
    case QTestLibModelTester::Silent:
        cmdArgs.append("-silent");
        break;
    case QTestLibModelTester::Normal:
        break;
    case QTestLibModelTester::Verbose1:
        cmdArgs.append("-v1");
        break;
    case QTestLibModelTester::Verbose2:
        cmdArgs.append("-v2");
        break;
    case QTestLibModelTester::VerboseSignal:
        cmdArgs.append("-vs");
        break;
    default:
        qWarning() << "Sentinel value VerbosityCountMinusOne must not be used as verbosity.";
        break;
    }
    cmdArgs.trimmed();

    // Creation of RunConfiguration
    QmakeProjectManager::QmakeProject project(TESTS_DIR "/" + test + "/" + test + ".pro", this);
    ProjectExplorer::Kit kit;
    ProjectExplorer::Target target(&project, &kit);
    ProjectExplorer::LocalApplicationRunConfiguration runConfig(&target);
    runConfig.setCommandLineArguments(cmdArgs);
    runConfig.setExecutable(TESTS_DIR "/" + test + "/debug/" + test.toLower());
    runConfig.setWorkingDirectory(TESTS_DIR "/" + test);

    ProjectExplorer::RunControl *runControl = new ProjectExplorer::LocalApplicationRunControl(&runConfig, this);
    runControl->setFormats(outputFormat, errorFormat);
    QTestLibPlugin::Internal::TestModelFactory* factory = new QTestLibPlugin::Internal::TestModelFactory(runControl, this);
    connect(factory, SIGNAL(modelPopulated(QAbstractItemModel*)), this, SLOT(saveModel(QAbstractItemModel*)));
    QSignalSpy modelFoundSpy(factory, SIGNAL(modelFound(QAbstractItemModel*)));
    QSignalSpy modelPopulatedSpy(factory, SIGNAL(modelPopulated(QAbstractItemModel*)));
    runControl->start();

    QVERIFY2(modelPopulatedSpy.wait(30000), "modelPopulated signal was not emitted within 30s");
    QVERIFY2(modelFoundSpy.count() == 1, "modelFound signal was not emitted before modelPopulated signal");

    END_SUB_TEST_FUNCTION
}

QTEST_MAIN(TestProxyModelTest)

#include "testproxymodeltest.moc"
