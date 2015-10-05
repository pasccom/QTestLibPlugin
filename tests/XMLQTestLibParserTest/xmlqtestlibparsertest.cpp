#include "../../xmlqtestlibparserfactory.h"
#include "../../qtestlibmodel.h"

#include <projectexplorer/runconfiguration.h>

#include "../qttestsubfunction.h"

#include <QtXml>
#include <QtTest>

class XMLQTestLibParserTest : public QObject
{
    Q_OBJECT
    HAS_SUB_TEST_FUNCTIONS
public:
    inline XMLQTestLibParserTest(void) : mParserFormat("xml") {}
    typedef enum {
        Silent = -1,
        Normal,
        Verbose1,
        Verbose2,
        VerboseSignal,
    } Verbosity;
private Q_SLOTS:
    inline void oneClass_data(void) {data();}
    void oneClass(void);
    inline void allMessages_data(void) {data();}
    void allMessages(void);
    inline void multipleClasses_data(void) {data();}
    void multipleClasses(void);
    inline void signalsTest_data(void) {data();}
    void signalsTest(void);
private:
    void data(void);
    void runTest(const QString& testName, Verbosity verbosity = Normal);
    void checkTest(const QAbstractItemModel *model, QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results, const QString& testName);

    void checkResults(QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results, const QDomElement& expected);
    void isOutput(const QDomElement& element, bool *ret);
    void isOutputFormat(const QDomElement& element, bool *ret);
    void isOutputVerbosity(const QDomElement& element, bool *ret);

    void parseRoot(const QAbstractItemModel* model, const QDomElement& element);
    void parseClass(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element);
    void parseFunction(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element);
    void parseRow(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element);
    void parseMessage(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element);

    QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> executeTest(QTestLibPlugin::Internal::AbstractTestParser *parser, const QString& test);

    QString mParserFormat;
    Verbosity mVerbosity;
};

Q_DECLARE_METATYPE(XMLQTestLibParserTest::Verbosity)

void XMLQTestLibParserTest::data(void)
{
    QTest::addColumn<Verbosity>("verbosity");

    QTest::newRow("Normal") << Normal;
    QTest::newRow("Silent") << Silent;
    QTest::newRow("Verbose1") << Verbose1;
    QTest::newRow("Verbose2") << Verbose2;
    QTest::newRow("VerboseS") << VerboseSignal;
}

void XMLQTestLibParserTest::oneClass(void)
{
    QFETCH(Verbosity, verbosity);

    runTest("OneClassTest", verbosity);
}

void XMLQTestLibParserTest::allMessages(void)
{
    QFETCH(Verbosity, verbosity);

    runTest("AllMessagesTest", verbosity);
}

void XMLQTestLibParserTest::multipleClasses(void)
{
    QFETCH(Verbosity, verbosity);

    runTest("MultipleClassesTest", verbosity);
}

void XMLQTestLibParserTest::signalsTest(void)
{
    QFETCH(Verbosity, verbosity);

    runTest("SignalsTest", verbosity);
}

void XMLQTestLibParserTest::runTest(const QString& testName, Verbosity verbosity)
{
    mVerbosity = verbosity;

    /* Executes the test and feeds the parser with the result */
    ProjectExplorer::RunConfiguration runConfig(this);
    QTestLibPlugin::Internal::XMLQTestLibParserFactory factory(this);
    // TODO: This should be done elsewhere: QVERIFY2(factory.canParse(&runConfig), "Factory should parse this test");
    QTestLibPlugin::Internal::AbstractTestParser* parser = factory.getParserInstance(this);
    QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results = executeTest(parser, testName);
    QAbstractItemModel *model = parser->getModel();

    mVerbosity = qMax(Normal, verbosity); // NOTE When running in XML silent is equal to normal
    checkTest(model, results, testName);

    delete model;
    delete parser;
    mVerbosity = Normal;
}

void XMLQTestLibParserTest::checkTest(const QAbstractItemModel *model, QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results, const QString& testName)
{
    /* Load the XML expected result */
    QDomDocument dom(testName);
    QFile domFile(TESTS_DIR "/" + testName + "/" + testName.toLower() + ".xml");
    QVERIFY(domFile.open(QIODevice::ReadOnly));
    QString error;
    int line = 0;
    int column = 0;
    QVERIFY2(dom.setContent(&domFile, false, &error, &line, &column), qPrintable(QString("%1 at %2:%3").arg(error).arg(line).arg(column)));
    QVERIFY(QString::compare(dom.documentElement().tagName(), "qtestliboutput", Qt::CaseSensitive) == 0);

    /* Check results */
    QDomElement expectedResults = dom.documentElement().firstChildElement("results");
    while (!expectedResults.isNull()) {
        QVERIFY2(expectedResults.hasAttribute("format"), "results elements must have a format attribute.");
        if (QString::compare(mParserFormat, expectedResults.attribute("format"), Qt::CaseSensitive) == 0)
            break;
        expectedResults = expectedResults.nextSiblingElement("results");
    }
    if (!expectedResults.isNull()) {
        SUB_TEST_FUNCTION(checkResults(results, expectedResults));
    }

    /* Test each index recursively */
    bool isElementOutput = false;
    QDomElement root = dom.documentElement().firstChildElement("root");
    QDomElement rootClass = dom.documentElement().firstChildElement("class");
    QVERIFY2(root.isNull() ^ rootClass.isNull(), "The document element should have either root or class child");
    if (!root.isNull()) {
        SUB_TEST_FUNCTION(isOutput(root, &isElementOutput));
        if (isElementOutput) {
            QVERIFY2(root.nextSiblingElement("root").isNull(), "The document element should have only one root child");
            SUB_TEST_FUNCTION(parseRoot(model, root));
        }
    }
    else if (!rootClass.isNull()) {
        SUB_TEST_FUNCTION(isOutput(rootClass, &isElementOutput));
        if (isElementOutput) {
            QVERIFY2(rootClass.nextSiblingElement("class").isNull(), "The document element should have only one class child");
            SUB_TEST_FUNCTION(parseClass(model, QModelIndex(), rootClass));
        }
    }
}

void XMLQTestLibParserTest::checkResults(QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results, const QDomElement& expected)
{
    BEGIN_SUB_TEST_FUNCTION

    QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult>::const_iterator currentResult = results.constBegin();
    QDomElement currentExpected = expected.firstChildElement();

    while ((currentResult != results.constEnd()) && (!currentExpected.isNull())) {
        bool isPrinted = false;
        SUB_TEST_FUNCTION(isOutput(currentExpected, &isPrinted));
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
        SUB_TEST_FUNCTION(isOutput(currentExpected, &isPrinted));
        QVERIFY2(!isPrinted, "Number of printed results mismatch");
        currentExpected = currentExpected.nextSiblingElement();
    }

    QVERIFY2((currentResult == results.constEnd()) && (currentExpected.isNull()), "Number of results mismatch");

    END_SUB_TEST_FUNCTION
}

void XMLQTestLibParserTest::isOutput(const QDomElement& element, bool *ret)
{
    BEGIN_SUB_TEST_FUNCTION

    SUB_TEST_FUNCTION(isOutputFormat(element, ret));
    if (*ret)
        SUB_TEST_FUNCTION(isOutputVerbosity(element, ret));

    END_SUB_TEST_FUNCTION
}

void XMLQTestLibParserTest::isOutputFormat(const QDomElement& element, bool *ret)
{
    BEGIN_SUB_TEST_FUNCTION

    QString formatString = element.attribute("format", "all");

    if (QString::compare(formatString, "all", Qt::CaseSensitive) == 0) {
        *ret = true;
    } else if (formatString.startsWith('!')) {
        QStringList formatList = formatString.mid(1).split(',', QString::SkipEmptyParts);
        *ret = !formatList.contains(mParserFormat);
    } else {
        QStringList formatList = formatString.split(',', QString::SkipEmptyParts);
        *ret = formatList.contains(mParserFormat);
    }

    END_SUB_TEST_FUNCTION
}

void XMLQTestLibParserTest::isOutputVerbosity(const QDomElement& element, bool *ret)
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

    int verboseLevel = (int) mVerbosity;

    QVERIFY2(ret != NULL, "The ret pointer should not be NULL");

    if (resultLevel == 3) {
        *ret = (mVerbosity == VerboseSignal);
    } else {
        if (mVerbosity == VerboseSignal)
            verboseLevel = (int) Normal;
        *ret = (verboseLevel >= resultLevel);
    }

    END_SUB_TEST_FUNCTION
}

void XMLQTestLibParserTest::parseRoot(const QAbstractItemModel* model, const QDomElement& element)
{
    BEGIN_SUB_TEST_FUNCTION

    QVERIFY2(model->data(QModelIndex(), QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).type() == QVariant::String, "Result string role for root index should be a string");
    QVERIFY2(QString::compare(model->data(QModelIndex(), QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).toString(), element.attribute("type", QString::null), Qt::CaseInsensitive) == 0, "Result string for root do not match");

    QDomElement classElement = element.firstChildElement("class");
    int i = 0;
    while (!classElement.isNull()) {
        bool isElementOutput = false;
        SUB_TEST_FUNCTION(isOutput(classElement, &isElementOutput));
        if (isElementOutput) {
            QVERIFY2(model->index(i, 0, QModelIndex()).isValid(), qPrintable(QString("Children %1 of root element is not valid").arg(i)));
            SUB_TEST_FUNCTION(parseClass(model, model->index(i, 0, QModelIndex()), classElement));
            i++;
        }
        classElement = classElement.nextSiblingElement("class");
    }

    QVERIFY2(model->rowCount(QModelIndex()) == i, qPrintable(QString("The model index should have %1 rows").arg(i)));
    QVERIFY2(model->columnCount(QModelIndex()) == 1, "The model index should have 1 column.");

    END_SUB_TEST_FUNCTION
}

void XMLQTestLibParserTest::parseClass(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element)
{
    BEGIN_SUB_TEST_FUNCTION

    QVERIFY2(model->data(index, Qt::DisplayRole).type() == QVariant::String, "Display role for class index should be a string");
    QVERIFY2(QString::compare(model->data(index, Qt::DisplayRole).toString(), element.attribute("name", QString::null) , Qt::CaseSensitive) == 0, "Class name do not match");
    QVERIFY2(model->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).type() == QVariant::String, "Result string role for class index should be a string");
    QVERIFY2(QString::compare(model->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).toString(), element.attribute("type", QString::null) , Qt::CaseInsensitive) == 0, "Result string for class do not match");

    int i = 0;
    QDomElement childElement = element.firstChildElement();
    while (!childElement.isNull()) {
        bool isElementOutput = false;
        SUB_TEST_FUNCTION(isOutput(childElement, &isElementOutput));
        if (isElementOutput) {
            if (QString::compare(childElement.tagName(), "function", Qt::CaseInsensitive) == 0) {
                QVERIFY2(model->index(i, 0, index).isValid(), qPrintable(QString("Function children %1 of class element is not valid").arg(i)));
                SUB_TEST_FUNCTION(parseFunction(model, model->index(i, 0, index), childElement));
                i++;
            }
            if (QString::compare(childElement.tagName(), "message", Qt::CaseInsensitive) == 0) {
                QVERIFY2(model->index(i, 0, index).isValid(), qPrintable(QString("Message children %1 of class element is not valid").arg(i)));
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

void XMLQTestLibParserTest::parseFunction(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element)
{
    BEGIN_SUB_TEST_FUNCTION

    QVERIFY2(model->data(index, Qt::DisplayRole).type() == QVariant::String, "Display role for function index should be a string");
    QVERIFY2(QString::compare(model->data(index, Qt::DisplayRole).toString(), element.attribute("name", QString::null) , Qt::CaseSensitive) == 0, "Function name do not match");
    qDebug() << model->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).toString()
             << element.attribute("type", QString::null);
    QVERIFY2(model->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).type() == QVariant::String, "Result string role for function index should be a string");
    QVERIFY2(QString::compare(model->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).toString(), element.attribute("type", QString::null) , Qt::CaseInsensitive) == 0, "Result string for function do not match");

    int i = 0;
    QDomElement childElement = element.firstChildElement();
    while(!childElement.isNull()) {
        QVERIFY2((QString::compare(childElement.tagName(), "row", Qt::CaseSensitive) == 0)
              || (QString::compare(childElement.tagName(), "message", Qt::CaseSensitive) == 0),
                 "A function child should be a row or a message.");
        bool isElementOutput = false;
        SUB_TEST_FUNCTION(isOutput(childElement, &isElementOutput));
        if (isElementOutput) {
            if (QString::compare(childElement.tagName(), "row", Qt::CaseInsensitive) == 0) {
                QVERIFY2(model->index(i, 0, index).isValid(), qPrintable(QString("Row children %1 of function element is not valid").arg(i)));
                SUB_TEST_FUNCTION(parseRow(model, model->index(i, 0, index), childElement));
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

void XMLQTestLibParserTest::parseRow(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element)
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
        SUB_TEST_FUNCTION(isOutput(messageElement, &isElementOutput));
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

void XMLQTestLibParserTest::parseMessage(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element)
{
    BEGIN_SUB_TEST_FUNCTION

    QVERIFY2(model->data(index, Qt::DisplayRole).type() == QVariant::String, "Display role for message index should be a string");
    QVERIFY2(element.firstChild().isText(), "Child node should be a text node.");
    QVERIFY2(element.firstChild().nextSibling().isNull(), "The element should have only one child");
    qDebug() << model->data(index, Qt::DisplayRole).toString()
             << element.firstChild().toText().data().trimmed();
    if (QString::compare(element.attribute("strict", "false"), "true", Qt::CaseInsensitive) == 0)
        QVERIFY2(QString::compare(model->data(index, Qt::DisplayRole).toString(), element.firstChild().toText().data().trimmed() , Qt::CaseSensitive) == 0, "Message text do not match");
    QVERIFY2(model->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).type() == QVariant::String, "Result string role for message index should be a string");
    qDebug() << model->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).toString()
             << element.attribute("type", QString::null);
    QVERIFY2(QString::compare(model->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).toString(), element.attribute("type", QString::null) , Qt::CaseInsensitive) == 0, "Result string for message do not match");

    QVERIFY2(model->rowCount(index) == 0, "The model index should have 0 rows");
    QVERIFY2(model->columnCount(index) == 0, "The model index should have 0 column.");

    END_SUB_TEST_FUNCTION
}

QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> XMLQTestLibParserTest::executeTest(QTestLibPlugin::Internal::AbstractTestParser *parser, const QString& test)
{
    QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results;
    QStringList cmdArgs;
    cmdArgs << "-o" << QString("-,%1").arg(mParserFormat);
    switch(mVerbosity) {
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

    QProcess testProc(this);
    testProc.setWorkingDirectory(TESTS_DIR "/" + test + "/");
    testProc.start(TESTS_DIR "/" + test + "/debug/" + test.toLower(), cmdArgs, QIODevice::ReadOnly);

    if (!testProc.waitForFinished(30000)) {
        qCritical() << "Test timed out";
        return results;
    }

    testProc.setReadChannel(QProcess::StandardOutput);
    while (!testProc.atEnd()) {
        QString line = testProc.readLine();
        while (line.endsWith('\n') || line.endsWith('\r'))
            line.chop(1);
        //qDebug() << line;
        results << parser->parseStdoutLine(NULL, line);
    }

    testProc.setReadChannel(QProcess::StandardError);
    while (!testProc.atEnd()) {
        QString line = testProc.readLine();
        while (line.endsWith('\n') || line.endsWith('\r'))
            line.chop(1);
        //qDebug() << line;
        results << parser->parseStderrLine(NULL, line);
    }

    return results;
}

QTEST_APPLESS_MAIN(XMLQTestLibParserTest)

#include "xmlqtestlibparsertest.moc"
