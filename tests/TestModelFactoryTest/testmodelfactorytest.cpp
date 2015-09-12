#include "../../testmodelfactory.h"
#include "../../qtestlibmodel.h"

#include "../qttestsubfunction.h"

#include "utils/outputformat.h"
#include "projectexplorer/runconfiguration.h"

#include <QtXml>
#include <QtTest>

class TestModelFactoryTest : public QObject
{
    Q_OBJECT
    HAS_SUB_TEST_FUNCTIONS
public:
    inline TestModelFactoryTest(void) :
        mModel(NULL) {}
    typedef enum {
        Silent = -1,
        Normal,
        Verbose1,
        Verbose2,
        VerboseSignal,
    } Verbosity;
private Q_SLOTS:
    void cleanup(void);

    inline void oneClass_data(void) {data();}
    void oneClass(void);
    inline void allMessages_data(void) {data();}
    void allMessages(void);
    inline void multipleClasses_data(void) {data();}
    void multipleClasses(void);
    inline void signalsTest_data(void) {data();}
    void signalsTest(void);
private slots:
    inline void saveModel(QAbstractItemModel* model) {mModel = model;}
private:
    void data(void);
    void runTest(const QString& testName, Verbosity verbosity = Normal, Utils::OutputFormat outputFormat = Utils::OutputFormat::StdOutFormat, Utils::OutputFormat errorFormat = Utils::OutputFormat::StdErrFormat);

    void checkResults(QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results, const QDomElement& expected, Verbosity verbosity);
    void isOutput(const QDomElement& element, Verbosity verbose, bool *ret);

    void parseRoot(const QAbstractItemModel* model, const QDomElement& element, Verbosity verbosity);
    void parseClass(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element, Verbosity verbosity);
    void parseFunction(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element, Verbosity verbosity);
    void parseRow(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element, Verbosity verbosity);
    void parseMessage(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element);

    void executeTest(const QString& test, Verbosity verbosity, Utils::OutputFormat outputFormat, Utils::OutputFormat errorFormat);

    QAbstractItemModel *mModel;
};

Q_DECLARE_METATYPE(TestModelFactoryTest::Verbosity)
Q_DECLARE_METATYPE(Utils::OutputFormat)

void TestModelFactoryTest::data(void)
{
    QTest::addColumn<Verbosity>("verbosity");
    QTest::addColumn<Utils::OutputFormat>("outputFormat");
    QTest::addColumn<Utils::OutputFormat>("errorFormat");

    QTest::newRow("Normal") << Normal << Utils::OutputFormat::StdOutFormat << Utils::OutputFormat::StdErrFormat;
    QTest::newRow("Silent") << Silent << Utils::OutputFormat::StdOutFormat << Utils::OutputFormat::StdErrFormat;
    QTest::newRow("Verbose1") << Verbose1 << Utils::OutputFormat::StdOutFormat << Utils::OutputFormat::StdErrFormat;
    QTest::newRow("Verbose2") << Verbose2 << Utils::OutputFormat::StdOutFormat << Utils::OutputFormat::StdErrFormat;
    QTest::newRow("VerobseSignal") << VerboseSignal << Utils::OutputFormat::StdOutFormat << Utils::OutputFormat::StdErrFormat;

    QTest::newRow("Normal SameLine") << Normal << Utils::OutputFormat::StdOutFormatSameLine << Utils::OutputFormat::StdErrFormatSameLine;
    QTest::newRow("Silent SameLine") << Silent << Utils::OutputFormat::StdOutFormatSameLine << Utils::OutputFormat::StdErrFormatSameLine;
    QTest::newRow("Verbose1 SameLine") << Verbose1 << Utils::OutputFormat::StdOutFormatSameLine << Utils::OutputFormat::StdErrFormatSameLine;
    QTest::newRow("Verbose2 SameLine") << Verbose2 << Utils::OutputFormat::StdOutFormatSameLine << Utils::OutputFormat::StdErrFormatSameLine;
    QTest::newRow("VerobseSignal SameLine") << VerboseSignal << Utils::OutputFormat::StdOutFormatSameLine << Utils::OutputFormat::StdErrFormatSameLine;

    QTest::newRow("Normal OutSameLine") << Normal << Utils::OutputFormat::StdOutFormatSameLine << Utils::OutputFormat::StdErrFormat;
    QTest::newRow("Silent OutSameLine") << Silent << Utils::OutputFormat::StdOutFormatSameLine << Utils::OutputFormat::StdErrFormat;
    QTest::newRow("Verbose1 OutSameLine") << Verbose1 << Utils::OutputFormat::StdOutFormatSameLine << Utils::OutputFormat::StdErrFormat;
    QTest::newRow("Verbose2 OutSameLine") << Verbose2 << Utils::OutputFormat::StdOutFormatSameLine << Utils::OutputFormat::StdErrFormat;
    QTest::newRow("VerobseSignal OutSameLine") << VerboseSignal << Utils::OutputFormat::StdOutFormatSameLine << Utils::OutputFormat::StdErrFormat;

    QTest::newRow("Normal ErrSameLine") << Normal << Utils::OutputFormat::StdOutFormat << Utils::OutputFormat::StdErrFormatSameLine;
    QTest::newRow("Silent ErrSameLine") << Silent << Utils::OutputFormat::StdOutFormat << Utils::OutputFormat::StdErrFormatSameLine;
    QTest::newRow("Verbose1 ErrSameLine") << Verbose1 << Utils::OutputFormat::StdOutFormat << Utils::OutputFormat::StdErrFormatSameLine;
    QTest::newRow("Verbose2 ErrSameLine") << Verbose2 << Utils::OutputFormat::StdOutFormat << Utils::OutputFormat::StdErrFormatSameLine;
    QTest::newRow("VerobseSignal ErrSameLine") << VerboseSignal << Utils::OutputFormat::StdOutFormat << Utils::OutputFormat::StdErrFormatSameLine;
}

void TestModelFactoryTest::oneClass(void)
{
    QFETCH(Verbosity, verbosity);
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    runTest("OneClassTest", verbosity, outputFormat, errorFormat);
}

void TestModelFactoryTest::allMessages(void)
{
    QFETCH(Verbosity, verbosity);
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    runTest("AllMessagesTest", verbosity, outputFormat, errorFormat);
}

void TestModelFactoryTest::multipleClasses(void)
{
    QFETCH(Verbosity, verbosity);

    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);
    runTest("MultipleClassesTest", verbosity, outputFormat, errorFormat);
}

void TestModelFactoryTest::signalsTest(void)
{
    QFETCH(Verbosity, verbosity);
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    runTest("SignalsTest", verbosity, outputFormat, errorFormat);
}

void TestModelFactoryTest::cleanup(void)
{
    if (mModel)
        delete mModel;
    mModel = NULL;
}

void TestModelFactoryTest::runTest(const QString& testName, Verbosity verbosity, Utils::OutputFormat outputFormat, Utils::OutputFormat errorFormat)
{
    /* Executes the test and feeds the parser with the result */
    SUB_TEST_FUNCTION(executeTest(testName, verbosity, outputFormat, errorFormat));
    QVERIFY2(mModel != NULL, "Model should not be NULL");

    /* Load the XML expected result */
    QDomDocument dom(testName);
    QFile domFile(TESTS_DIR "/" + testName + "/" + testName.toLower() + ".xml");
    QVERIFY(domFile.open(QIODevice::ReadOnly));
    QString error;
    int line;
    int column;
    QVERIFY2(dom.setContent(&domFile, false, &error, &line, &column), qPrintable(QString("%1 at %2:%3").arg(error).arg(line).arg(column)));
    QVERIFY(QString::compare(dom.documentElement().tagName(), "qtestliboutput", Qt::CaseSensitive) == 0);

    /* Test each index recursively */
    bool isElementOutput = false;
    QDomElement root = dom.documentElement().firstChildElement("root");
    QDomElement rootClass = dom.documentElement().firstChildElement("class");
    QVERIFY2(root.isNull() ^ rootClass.isNull(), "The document element should have either root or class child");
    if (!root.isNull()) {
        SUB_TEST_FUNCTION(isOutput(root, verbosity, &isElementOutput));
        if (isElementOutput) {
            QVERIFY2(root.nextSiblingElement("root").isNull(), "The document element should have only one root child");
            SUB_TEST_FUNCTION(parseRoot(mModel, root, verbosity));
        }
    }
    else if (!rootClass.isNull()) {
        SUB_TEST_FUNCTION(isOutput(rootClass, verbosity, &isElementOutput));
        if (isElementOutput) {
            QVERIFY2(rootClass.nextSiblingElement("class").isNull(), "The document element should have only one class child");
            SUB_TEST_FUNCTION(parseClass(mModel, QModelIndex(), rootClass, verbosity));
        }
    }
}


void TestModelFactoryTest::checkResults(QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results, const QDomElement& expected, Verbosity verbosity)
{
    BEGIN_SUB_TEST_FUNCTION

    QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult>::const_iterator currentResult = results.constBegin();
    QDomElement currentExpected = expected.firstChildElement();

    while ((currentResult != results.constEnd()) && (!currentExpected.isNull())) {
        bool isPrinted = false;
        SUB_TEST_FUNCTION(isOutput(currentExpected, verbosity, &isPrinted));
        if (isPrinted) {
            switch (*currentResult) {
            case QTestLibPlugin::Internal::TestModelFactory::Unsure:
                QVERIFY(QString::compare(currentExpected.tagName(), "unsure", Qt::CaseInsensitive) == 0);
                break;
            case QTestLibPlugin::Internal::TestModelFactory::MagicFound:
                QVERIFY(QString::compare(currentExpected.tagName(), "magicfound", Qt::CaseInsensitive) == 0);
                break;
            case QTestLibPlugin::Internal::TestModelFactory::MagicNotFound:
                QVERIFY(QString::compare(currentExpected.tagName(), "magicnotfound", Qt::CaseInsensitive) == 0);
                break;
            }
            currentResult++;
        }
        currentExpected = currentExpected.nextSiblingElement();
    }

    while (!currentExpected.isNull()) {
        bool isPrinted = false;
        SUB_TEST_FUNCTION(isOutput(currentExpected, verbosity, &isPrinted));
        QVERIFY2(!isPrinted, "Number of printed results mismatch");
        currentExpected = currentExpected.nextSiblingElement();
    }

    QVERIFY2((currentResult == results.constEnd()) && (currentExpected.isNull()), "Number of results mismatch");

    END_SUB_TEST_FUNCTION
}

void TestModelFactoryTest::isOutput(const QDomElement& element, Verbosity verbose, bool *ret)
{
    BEGIN_SUB_TEST_FUNCTION

    const QString resultLevels = "silent  "
                                 "normal  "
                                 "verbose1"
                                 "verbose2"
                                 "verboses";
    QString resultLevelString = element.attribute("level", "normal");
    int resultLevel = resultLevels.indexOf(resultLevelString, 0, Qt::CaseSensitive);
    QVERIFY2(resultLevel != -1, qPrintable(QString("Unknown level: %1").arg(resultLevelString)));
    resultLevel = resultLevel/8 - 1;

    int verboseLevel = (int) verbose;

    QVERIFY2(ret != NULL, "THe ret pointer should not be NULL");

    if (resultLevel == 3) {
        *ret = (verbose == VerboseSignal);
    } else {
        if (verbose == VerboseSignal)
            verboseLevel = (int) Normal;
        *ret = (verboseLevel >= resultLevel);
    }

    END_SUB_TEST_FUNCTION
}

void TestModelFactoryTest::parseRoot(const QAbstractItemModel* model, const QDomElement& element, Verbosity verbosity)
{
    BEGIN_SUB_TEST_FUNCTION

    QVERIFY2(model->data(QModelIndex(), QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).type() == QVariant::String, "Result string role for root index should be a string");
    QVERIFY2(QString::compare(model->data(QModelIndex(), QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).toString(), element.attribute("type", QString::null), Qt::CaseInsensitive) == 0, "Result string for root do not match");

    QDomElement classElement = element.firstChildElement("class");
    int i = 0;
    while (!classElement.isNull()) {
        bool isElementOutput = false;
        SUB_TEST_FUNCTION(isOutput(classElement, verbosity, &isElementOutput));
        if (isElementOutput) {
            QVERIFY2(model->index(i, 0, QModelIndex()).isValid(), qPrintable(QString("Children %1 of root element is not valid").arg(i)));
            SUB_TEST_FUNCTION(parseClass(model, model->index(i, 0, QModelIndex()), classElement, verbosity));
            i++;
        }
        classElement = classElement.nextSiblingElement("class");
    }

    QVERIFY2(model->rowCount(QModelIndex()) == i, qPrintable(QString("The model index should have %1 rows").arg(i)));
    QVERIFY2(model->columnCount(QModelIndex()) == 1, "The model index should have 1 column.");

    END_SUB_TEST_FUNCTION
}

void TestModelFactoryTest::parseClass(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element, Verbosity verbosity)
{
    BEGIN_SUB_TEST_FUNCTION

    QVERIFY2(model->data(index, Qt::DisplayRole).type() == QVariant::String, "Display role for class index should be a string");
    QVERIFY2(QString::compare(model->data(index, Qt::DisplayRole).toString(), element.attribute("name", QString::null) , Qt::CaseSensitive) == 0, "Class name do not match");
    QVERIFY2(model->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).type() == QVariant::String, "Result string role for class index should be a string");
    QVERIFY2(QString::compare(model->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).toString(), element.attribute("type", QString::null) , Qt::CaseInsensitive) == 0, "Result string for class do not match");

    QDomElement functionElement = element.firstChildElement("function");
    int i = 0;
    while (!functionElement.isNull()) {
        bool isElementOutput = false;
        SUB_TEST_FUNCTION(isOutput(functionElement, verbosity, &isElementOutput));
        if (isElementOutput) {
            QVERIFY2(model->index(i, 0, index).isValid(), qPrintable(QString("Children %1 of class element is not valid").arg(i)));
            SUB_TEST_FUNCTION(parseFunction(model, model->index(i, 0, index), functionElement, verbosity));
            i++;
        }
        functionElement = functionElement.nextSiblingElement("function");
    }

    QVERIFY2(model->rowCount(index) == i, qPrintable(QString("The model index should have %1 rows").arg(i)));
    QVERIFY2(model->columnCount(index) == 1, "The model index should have 1 column.");

    END_SUB_TEST_FUNCTION
}

void TestModelFactoryTest::parseFunction(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element, Verbosity verbosity)
{
    BEGIN_SUB_TEST_FUNCTION

    QVERIFY2(model->data(index, Qt::DisplayRole).type() == QVariant::String, "Display role for function index should be a string");
    QVERIFY2(QString::compare(model->data(index, Qt::DisplayRole).toString(), element.attribute("name", QString::null) , Qt::CaseSensitive) == 0, "Function name do not match");
    QVERIFY2(model->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).type() == QVariant::String, "Result string role for function index should be a string");
    /*qDebug() << model->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).toString()
             << element.attribute("type", QString::null);*/
    QVERIFY2(QString::compare(model->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).toString(), element.attribute("type", QString::null) , Qt::CaseInsensitive) == 0, "Result string for function do not match");

    int i = 0;
    QDomElement childElement = element.firstChildElement();
    while(!childElement.isNull()) {
        QVERIFY2((QString::compare(childElement.tagName(), "row", Qt::CaseSensitive) == 0)
              || (QString::compare(childElement.tagName(), "message", Qt::CaseSensitive) == 0),
                 "A function child should be a row or a message.");
        bool isElementOutput = false;
        SUB_TEST_FUNCTION(isOutput(childElement, verbosity, &isElementOutput));
        if (isElementOutput) {
            if (QString::compare(childElement.tagName(), "row", Qt::CaseInsensitive) == 0) {
                QVERIFY2(model->index(i, 0, index).isValid(), qPrintable(QString("Row children %1 of function element is not valid").arg(i)));
                SUB_TEST_FUNCTION(parseRow(model, model->index(i, 0, index), childElement, verbosity));
                i++;
            }
            if (QString::compare(childElement.tagName(), "message", Qt::CaseInsensitive) == 0) {
                QVERIFY2(model->index(i, 0, index).isValid(), qPrintable(QString("Message children %1 of function element is not valid").arg(i)));
                SUB_TEST_FUNCTION(parseMessage(model, model->index(i, 0, index), childElement));
                i++;
            }
        }
        childElement = childElement.nextSiblingElement();
    }

    QVERIFY2(model->rowCount(index) == i, qPrintable(QString("The model index should have %1 rows").arg(i)));
    QVERIFY2(model->columnCount(index) == 1, "The model index should have 1 column.");

    END_SUB_TEST_FUNCTION
}

void TestModelFactoryTest::parseRow(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element, Verbosity verbosity)
{
    BEGIN_SUB_TEST_FUNCTION

    QVERIFY2(model->data(index, Qt::DisplayRole).type() == QVariant::String, "Display role for row index should be a string");
    /*qDebug() << model->data(index, Qt::DisplayRole).toString()
             << element.attribute("title", QString::null);*/
    QVERIFY2(QString::compare(model->data(index, Qt::DisplayRole).toString(), element.attribute("title", QString::null) , Qt::CaseSensitive) == 0, "Row title do not match");
    QVERIFY2(model->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).type() == QVariant::String, "Result string role for row index should be a string");
    QVERIFY2(QString::compare(model->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).toString(), element.attribute("type", QString::null) , Qt::CaseInsensitive) == 0, "Result string for row do not match");

    QDomElement messageElement = element.firstChildElement("message");
    int i = 0;
    while (!messageElement.isNull()) {
        bool isElementOutput = false;
        SUB_TEST_FUNCTION(isOutput(messageElement, verbosity, &isElementOutput));
        if (isElementOutput) {
            QVERIFY2(model->index(i, 0, index).isValid(), qPrintable(QString("Children %1 of class element is not valid").arg(i)));
            SUB_TEST_FUNCTION(parseMessage(model, model->index(i, 0, index), messageElement));
            i++;
        }
        messageElement = messageElement.nextSiblingElement("message");
    }

    QVERIFY2(model->rowCount(index) == i, qPrintable(QString("The model index should have %1 rows").arg(i)));
    QVERIFY2(model->columnCount(index) == 1, "The model index should have 1 column.");

    END_SUB_TEST_FUNCTION
}

void TestModelFactoryTest::parseMessage(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element)
{
    BEGIN_SUB_TEST_FUNCTION

    QVERIFY2(model->data(index, Qt::DisplayRole).type() == QVariant::String, "Display role for message index should be a string");
    QVERIFY2(element.firstChild().isText(), "Child node should be a text node.");
    QVERIFY2(element.firstChild().nextSibling().isNull(), "The element should have only one child");
    /*qDebug() << model->data(index, Qt::DisplayRole).toString()
             << element.firstChild().toText().data().trimmed();*/
    if (QString::compare(element.attribute("strict", "false"), "true", Qt::CaseInsensitive) == 0)
        QVERIFY2(QString::compare(model->data(index, Qt::DisplayRole).toString(), element.firstChild().toText().data().trimmed() , Qt::CaseSensitive) == 0, "Message text do not match");
    QVERIFY2(model->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).type() == QVariant::String, "Result string role for message index should be a string");
    /*qDebug() << model->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).toString()
             << element.attribute("type", QString::null);*/
    QVERIFY2(QString::compare(model->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).toString(), element.attribute("type", QString::null) , Qt::CaseInsensitive) == 0, "Result string for message do not match");

    QVERIFY2(model->rowCount(index) == 0, "The model index should have 0 rows");
    QVERIFY2(model->columnCount(index) == 0, "The model index should have 0 column.");

    END_SUB_TEST_FUNCTION
}

void TestModelFactoryTest::executeTest(const QString& test, Verbosity verbosity, Utils::OutputFormat outputFormat, Utils::OutputFormat errorFormat)
{
    BEGIN_SUB_TEST_FUNCTION

    QStringList cmdArgs;
    cmdArgs << "-o" << "-,txt";
    switch(verbosity) {
    case Silent:
        cmdArgs << "-silent";
        break;
    case Normal:
        break;
    case Verbose1:
        cmdArgs << "-v1";
        break;
    case Verbose2:
        cmdArgs << "-v2";
        break;
    case VerboseSignal:
        cmdArgs << "-vs";
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
