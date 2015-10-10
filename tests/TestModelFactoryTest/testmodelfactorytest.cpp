#include "../../testmodelfactory.h"

#include "../common/qtestlibmodeltester.h"

#include "utils/outputformat.h"
#include "projectexplorer/runconfiguration.h"

#include <QtTest>

class TestModelFactoryTest : public QObject
{
    Q_OBJECT
    HAS_SUB_TEST_FUNCTIONS
public:
    inline TestModelFactoryTest(void) :
        mModel(NULL) {}
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
    void runTest(const QString& testName, const QString& format, QTestLibModelTester::Verbosity verbosity = QTestLibModelTester::Normal, Utils::OutputFormat outputFormat = Utils::OutputFormat::StdOutFormat, Utils::OutputFormat errorFormat = Utils::OutputFormat::StdErrFormat);
    void checkTest(const QString& testName, Utils::OutputFormat outputFormat = Utils::OutputFormat::StdOutFormat, Utils::OutputFormat errorFormat = Utils::OutputFormat::StdErrFormat);

    void executeTest(const QString& test, Utils::OutputFormat outputFormat, Utils::OutputFormat errorFormat);

    QAbstractItemModel *mModel;
    QString mParserFormat;
    QTestLibModelTester::Verbosity mVerbosity;
};

Q_DECLARE_METATYPE(Utils::OutputFormat)

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

    QStringList cmdArgs;
    cmdArgs << "-o" << QString("-,%1").arg(mParserFormat);
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

    ProjectExplorer::RunControl *runControl = new ProjectExplorer::RunControl(TESTS_DIR "/" + test + "/debug/" + test.toLower(), cmdArgs, test, this);
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
