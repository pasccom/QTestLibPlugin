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

#include <testmodelfactory.h>

#include <plaintextqtestlibparserfactory.h>
#include <xmlqtestlibparserfactory.h>
#include <lightxmlqtestlibparserfactory.h>
#include <xunitxmlqtestlibparserfactory.h>

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

class TestModelFactoryTest : public QObject
{
    Q_OBJECT
    HAS_SUB_TEST_FUNCTIONS
public:
    TestModelFactoryTest(void);
    inline ~TestModelFactoryTest(void) {qDeleteAll(mManager->allObjects()); delete mManager;}
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

    inline void oneClassLightXML_data(void) {data();}
    void oneClassLightXML(void);
    inline void allMessagesLightXML_data(void) {data();}
    void allMessagesLightXML(void);
    inline void multipleClassesLightXML_data(void) {data();}
    void multipleClassesLightXML(void);
    inline void signalsTestLightXML_data(void) {data();}
    void signalsTestLightXML(void);
    inline void limitsLightXML_data(void) {data();}
    void limitsLightXML(void);

    inline void oneClassXUnitXML_data(void) {data();}
    void oneClassXUnitXML(void);
    inline void allMessagesXUnitXML_data(void) {data();}
    void allMessagesXUnitXML(void);
    inline void multipleClassesXUnitXML_data(void) {data();}
    void multipleClassesXUnitXML(void);
    inline void signalsTestXUnitXML_data(void) {data();}
    void signalsTestXUnitXML(void);
    inline void limitsXUnitXML_data(void) {data();}
    void limitsXUnitXML(void);
private slots:
    inline void saveModel(QAbstractItemModel* model) {mModel = model;}
private:
    void data(void);
    void runTest(const QString& testName, const QString& format, QTestLibModelTester::Verbosity verbosity = QTestLibModelTester::Normal, Utils::OutputFormat outputFormat = Utils::OutputFormat::StdOutFormat, Utils::OutputFormat errorFormat = Utils::OutputFormat::StdErrFormat);
    void checkTest(const QString& testName, Utils::OutputFormat outputFormat = Utils::OutputFormat::StdOutFormat, Utils::OutputFormat errorFormat = Utils::OutputFormat::StdErrFormat);

    void executeTest(const QString& test, Utils::OutputFormat outputFormat, Utils::OutputFormat errorFormat);

    QAbstractItemModel *mModel;
    QString mParserFormat;
    QTestLibModelTester::Verbosity mVerbosity;

    ExtensionSystem::PluginManager *mManager;
};

Q_DECLARE_METATYPE(Utils::OutputFormat)

TestModelFactoryTest::TestModelFactoryTest(void) :
    mModel(NULL)
{
    qsrand(QDateTime::currentMSecsSinceEpoch());
    QmakeProjectManager::QmakeProjectManager::initialize();

    mManager = new ExtensionSystem::PluginManager();
    QTestLibPlugin::Internal::AbstractTestParserFactory *plainTextFactory = new QTestLibPlugin::Internal::PlainTextQTestLibParserFactory(this);
    ExtensionSystem::PluginManager::addObject(plainTextFactory);
    QTestLibPlugin::Internal::AbstractTestParserFactory *xmlFactory = new QTestLibPlugin::Internal::XMLQTestLibParserFactory(this);
    ExtensionSystem::PluginManager::addObject(xmlFactory);
    QTestLibPlugin::Internal::AbstractTestParserFactory *lightXmlFactory = new QTestLibPlugin::Internal::LightXMLQTestLibParserFactory(this);
    ExtensionSystem::PluginManager::addObject(lightXmlFactory);
    QTestLibPlugin::Internal::AbstractTestParserFactory *xUnitXmlFactory = new QTestLibPlugin::Internal::XUnitXMLQTestLibParserFactory(this);
    ExtensionSystem::PluginManager::addObject(xUnitXmlFactory);
}

void TestModelFactoryTest::data(void)
{
    QTest::addColumn<QTestLibModelTester::Verbosity>("verbosity");
    QTest::addColumn<Utils::OutputFormat>("outputFormat");
    QTest::addColumn<Utils::OutputFormat>("errorFormat");

    QTest::newRow("Normal") << QTestLibModelTester::Normal << Utils::OutputFormat::StdOutFormat << Utils::OutputFormat::StdErrFormat;
    QTest::newRow("Silent") << QTestLibModelTester::Silent << Utils::OutputFormat::StdOutFormat << Utils::OutputFormat::StdErrFormat;
    QTest::newRow("Verbose1") << QTestLibModelTester::Verbose1 << Utils::OutputFormat::StdOutFormat << Utils::OutputFormat::StdErrFormat;
    QTest::newRow("Verbose2") << QTestLibModelTester::Verbose2 << Utils::OutputFormat::StdOutFormat << Utils::OutputFormat::StdErrFormat;
    QTest::newRow("VerobseSignal") << QTestLibModelTester::VerboseSignal << Utils::OutputFormat::StdOutFormat << Utils::OutputFormat::StdErrFormat;

    QTest::newRow("Normal SameLine") << QTestLibModelTester::Normal << Utils::OutputFormat::StdOutFormatSameLine << Utils::OutputFormat::StdErrFormatSameLine;
    QTest::newRow("Silent SameLine") << QTestLibModelTester::Silent << Utils::OutputFormat::StdOutFormatSameLine << Utils::OutputFormat::StdErrFormatSameLine;
    QTest::newRow("Verbose1 SameLine") << QTestLibModelTester::Verbose1 << Utils::OutputFormat::StdOutFormatSameLine << Utils::OutputFormat::StdErrFormatSameLine;
    QTest::newRow("Verbose2 SameLine") << QTestLibModelTester::Verbose2 << Utils::OutputFormat::StdOutFormatSameLine << Utils::OutputFormat::StdErrFormatSameLine;
    QTest::newRow("VerobseSignal SameLine") << QTestLibModelTester::VerboseSignal << Utils::OutputFormat::StdOutFormatSameLine << Utils::OutputFormat::StdErrFormatSameLine;

    QTest::newRow("Normal OutSameLine") << QTestLibModelTester::Normal << Utils::OutputFormat::StdOutFormatSameLine << Utils::OutputFormat::StdErrFormat;
    QTest::newRow("Silent OutSameLine") << QTestLibModelTester::Silent << Utils::OutputFormat::StdOutFormatSameLine << Utils::OutputFormat::StdErrFormat;
    QTest::newRow("Verbose1 OutSameLine") << QTestLibModelTester::Verbose1 << Utils::OutputFormat::StdOutFormatSameLine << Utils::OutputFormat::StdErrFormat;
    QTest::newRow("Verbose2 OutSameLine") << QTestLibModelTester::Verbose2 << Utils::OutputFormat::StdOutFormatSameLine << Utils::OutputFormat::StdErrFormat;
    QTest::newRow("VerobseSignal OutSameLine") << QTestLibModelTester::VerboseSignal << Utils::OutputFormat::StdOutFormatSameLine << Utils::OutputFormat::StdErrFormat;

    QTest::newRow("Normal ErrSameLine") << QTestLibModelTester::Normal << Utils::OutputFormat::StdOutFormat << Utils::OutputFormat::StdErrFormatSameLine;
    QTest::newRow("Silent ErrSameLine") << QTestLibModelTester::Silent << Utils::OutputFormat::StdOutFormat << Utils::OutputFormat::StdErrFormatSameLine;
    QTest::newRow("Verbose1 ErrSameLine") << QTestLibModelTester::Verbose1 << Utils::OutputFormat::StdOutFormat << Utils::OutputFormat::StdErrFormatSameLine;
    QTest::newRow("Verbose2 ErrSameLine") << QTestLibModelTester::Verbose2 << Utils::OutputFormat::StdOutFormat << Utils::OutputFormat::StdErrFormatSameLine;
    QTest::newRow("VerobseSignal ErrSameLine") << QTestLibModelTester::VerboseSignal << Utils::OutputFormat::StdOutFormat << Utils::OutputFormat::StdErrFormatSameLine;
}

void TestModelFactoryTest::oneClassTXT(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    runTest("OneClassTest", "txt", verbosity, outputFormat, errorFormat);
}

void TestModelFactoryTest::allMessagesTXT(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    runTest("AllMessagesTest", "txt", verbosity, outputFormat, errorFormat);
}

void TestModelFactoryTest::multipleClassesTXT(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);

    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);
    runTest("MultipleClassesTest", "txt", verbosity, outputFormat, errorFormat);
}

void TestModelFactoryTest::signalsTestTXT(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    runTest("SignalsTest", "txt", verbosity, outputFormat, errorFormat);
}


void TestModelFactoryTest::limitsTXT(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    runTest("LimitsTest", "txt", verbosity, outputFormat, errorFormat);
}

void TestModelFactoryTest::oneClassXML(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    runTest("OneClassTest", "xml", verbosity, outputFormat, errorFormat);
}

void TestModelFactoryTest::allMessagesXML(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    runTest("AllMessagesTest", "xml", verbosity, outputFormat, errorFormat);
}

void TestModelFactoryTest::multipleClassesXML(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    runTest("MultipleClassesTest", "xml", verbosity, outputFormat, errorFormat);
}

void TestModelFactoryTest::signalsTestXML(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    runTest("SignalsTest", "xml", verbosity, outputFormat, errorFormat);
}

void TestModelFactoryTest::limitsXML(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    runTest("LimitsTest", "xml", verbosity, outputFormat, errorFormat);
}

void TestModelFactoryTest::oneClassLightXML(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    runTest("OneClassTest", "lightxml", verbosity, outputFormat, errorFormat);
}

void TestModelFactoryTest::allMessagesLightXML(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    runTest("AllMessagesTest", "lightxml", verbosity, outputFormat, errorFormat);
}

void TestModelFactoryTest::multipleClassesLightXML(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    runTest("MultipleClassesTest", "lightxml", verbosity, outputFormat, errorFormat);
}

void TestModelFactoryTest::signalsTestLightXML(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    runTest("SignalsTest", "lightxml", verbosity, outputFormat, errorFormat);
}

void TestModelFactoryTest::limitsLightXML(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    runTest("LimitsTest", "lightxml", verbosity, outputFormat, errorFormat);
}

void TestModelFactoryTest::oneClassXUnitXML(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    runTest("OneClassTest", "xunitxml", verbosity, outputFormat, errorFormat);
}

void TestModelFactoryTest::allMessagesXUnitXML(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    runTest("AllMessagesTest", "xunitxml", verbosity, outputFormat, errorFormat);
}

void TestModelFactoryTest::multipleClassesXUnitXML(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    runTest("MultipleClassesTest", "xunitxml", verbosity, outputFormat, errorFormat);
}

void TestModelFactoryTest::signalsTestXUnitXML(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    runTest("SignalsTest", "xunitxml", verbosity, outputFormat, errorFormat);
}

void TestModelFactoryTest::limitsXUnitXML(void)
{
    QFETCH(QTestLibModelTester::Verbosity, verbosity);
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    runTest("LimitsTest", "xunitxml", verbosity, outputFormat, errorFormat);
}

void TestModelFactoryTest::cleanup(void)
{
    if (mModel)
        delete mModel;
    mModel = NULL;
}

void TestModelFactoryTest::runTest(const QString& testName, const QString& format, QTestLibModelTester::Verbosity verbosity, Utils::OutputFormat outputFormat, Utils::OutputFormat errorFormat)
{
    mParserFormat = format;
    if (QString::compare(mParserFormat, "txt", Qt::CaseSensitive) == 0)
        mVerbosity = verbosity;
    else if (QString::compare(mParserFormat, "xml", Qt::CaseSensitive) == 0)
        mVerbosity = qMax(QTestLibModelTester::Normal, verbosity);

    checkTest(testName, outputFormat, errorFormat);

    mVerbosity = QTestLibModelTester::Normal;
    mParserFormat = QString::null;
}

void TestModelFactoryTest::checkTest(const QString& testName, Utils::OutputFormat outputFormat, Utils::OutputFormat errorFormat)
{
    /* Executes the test and feeds the parser with the result */
    SUB_TEST_FUNCTION(executeTest(testName, outputFormat, errorFormat));
    QVERIFY2(mModel != NULL, "Model should not be NULL");

    QTestLibModelTester tester(mModel, (QTestLibModelTester::Verbosity) mVerbosity, mParserFormat);
    QVERIFY2(tester.checkIndex(QModelIndex(), testName), qPrintable(tester.error()));
}

void TestModelFactoryTest::executeTest(const QString& test, Utils::OutputFormat outputFormat, Utils::OutputFormat errorFormat)
{
    BEGIN_SUB_TEST_FUNCTION

    QString cmdArgs;
    int argsVersion = qrand() % 3;
    if ((mParserFormat != "txt") && (argsVersion == 0))
        argsVersion = 2;
    if (argsVersion == 1)
        cmdArgs.append(QString("-%1 ").arg(mParserFormat));
    else if (argsVersion == 2)
        cmdArgs.append(QString("-o -,%1 ").arg(mParserFormat));

    switch(mVerbosity) {
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
    runConfig.setExecutable(TESTS_DIR "/" + test + "/debug/" + test);
    runConfig.setWorkingDirectory(TESTS_DIR "/" + test);

    ProjectExplorer::RunControl *runControl = new ProjectExplorer::LocalApplicationRunControl(&runConfig, this);
    qDebug() << "Run config display name:" << runConfig.displayName();
    qDebug() << "Run config exe:" << runConfig.executable();
    qDebug() << "Run config dir:" << runConfig.workingDirectory();
    qDebug() << "Run control display name:" << runControl->displayName();
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

QTEST_MAIN(TestModelFactoryTest)

#include "testmodelfactorytest.moc"
