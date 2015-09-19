#include "../../testsuitemodel.h"
#include "../../qtestlibmodel.h"

#include "../qttestsubfunction.h"

#include "utils/outputformat.h"
#include "projectexplorer/runconfiguration.h"

#include <QtXml>
#include <QtTest>

class TestSuiteModelTest : public QObject
{
    Q_OBJECT
    HAS_SUB_TEST_FUNCTIONS
public:
    TestSuiteModelTest(void);
    typedef enum {
        Silent = -1,
        Normal,
        Verbose1,
        Verbose2,
        VerboseSignal,
    } Verbosity;
private Q_SLOTS:
    void zeroRemoveBad(void);
    void zeroClear(void);

    inline void appendOne_data(void) {data();}
    void appendOne(void);
    inline void appendOneRemoveOne_data(void) {data();}
    void appendOneRemoveOne(void);
    inline void removeBadAppendOne_data(void) {data();}
    void removeBadAppendOne(void);
    inline void appendOneRemoveBad_data(void) {data();}
    void appendOneRemoveBad(void);
    inline void appendOneClear_data(void) {data();}
    void appendOneClear(void);

    inline void appendTwo_data(void) {data();}
    void appendTwo(void);
    inline void appendTwoRemoveFirst_data(void) {data();}
    void appendTwoRemoveFirst(void);
    inline void appendTwoRemoveSecond_data(void) {data();}
    void appendTwoRemoveSecond(void);
    inline void appendTwoRemoveBad_data(void) {data();}
    void appendTwoRemoveBad(void);
    inline void appendOneRemoveAppendOne_data(void) {data();}
    void appendOneRemoveAppendOne(void);
    inline void appendOneRemoveBadAppendOne_data(void) {data();}
    void appendOneRemoveBadAppendOne(void);
    inline void appendTwoClear_data(void) {data();}
    void appendTwoClear(void);
    inline void appendOneClearAppendOne_data(void) {data();}
    void appendOneClearAppendOne(void);
private:
    void data(void);

    void isOutput(const QDomElement& element, Verbosity verbose, bool *ret);

    void parseSuiteRoot(const QAbstractItemModel* model, const QList< QPair<QString, Verbosity> >& tests);
    void parseRoot(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element, Verbosity verbosity);
    void parseClass(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element, Verbosity verbosity);
    void parseFunction(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element, Verbosity verbosity);
    void parseRow(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element, Verbosity verbosity);
    void parseMessage(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element);

    void checkSignalArguments(const QString& signal, const QList<QVariant>& args, int first, int last);
    void appendTest(QTestLibPlugin::Internal::TestSuiteModel *model, const QString& test, Verbosity verbosity, Utils::OutputFormat outputFormat, Utils::OutputFormat errorFormat);
    void removeTestAt(QTestLibPlugin::Internal::TestSuiteModel *model, int i);
    void clearTests(QTestLibPlugin::Internal::TestSuiteModel *model);

    QStringList mTests;
};

Q_DECLARE_METATYPE(TestSuiteModelTest::Verbosity)
Q_DECLARE_METATYPE(Utils::OutputFormat)

TestSuiteModelTest::TestSuiteModelTest(void)
{
    qsrand(QDateTime::currentMSecsSinceEpoch());

    mTests.clear();
    mTests << "OneClassTest";
    mTests << "AllMessagesTest";
    mTests << "MultipleClassesTest";
    mTests << "SignalsTest";
}

void TestSuiteModelTest::data(void)
{
    QTest::addColumn<Utils::OutputFormat>("outputFormat");
    QTest::addColumn<Utils::OutputFormat>("errorFormat");

    QTest::newRow("Normal") << Utils::OutputFormat::StdOutFormat << Utils::OutputFormat::StdErrFormat;
    QTest::newRow("SameLine") << Utils::OutputFormat::StdOutFormatSameLine << Utils::OutputFormat::StdErrFormatSameLine;
    QTest::newRow("OutSameLine") << Utils::OutputFormat::StdOutFormatSameLine << Utils::OutputFormat::StdErrFormat;
    QTest::newRow("ErrSameLine") << Utils::OutputFormat::StdOutFormat << Utils::OutputFormat::StdErrFormatSameLine;
}

void TestSuiteModelTest::zeroRemoveBad(void)
{
    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< QPair<QString, Verbosity> > testRuns;

    // Try to remove non existing test
    SUB_TEST_FUNCTION(removeTestAt(&model, 0));
    //testRuns.removeAt(0);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::zeroClear(void)
{
    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< QPair<QString, Verbosity> > testRuns;

    // Clear test suite
    SUB_TEST_FUNCTION(clearTests(&model));
    testRuns.clear();
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::appendOne(void)
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QString testRun;
    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< QPair<QString, Verbosity> > testRuns;

    // Append one test
    testRun = mTests.at(qrand() % mTests.size());
    qDebug() << testRun;
    SUB_TEST_FUNCTION(appendTest(&model, testRun, Normal, outputFormat, errorFormat));
    testRuns.append(QPair<QString, Verbosity>(testRun, Normal));
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::appendOneRemoveOne()
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QString testRun;
    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< QPair<QString, Verbosity> > testRuns;

    // Append one test
    testRun = mTests.at(qrand() % mTests.size());
    qDebug() << testRun;
    SUB_TEST_FUNCTION(appendTest(&model, testRun, Normal, outputFormat, errorFormat));
    testRuns.append(QPair<QString, Verbosity>(testRun, Normal));
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Remove the appended test
    SUB_TEST_FUNCTION(removeTestAt(&model, 0));
    testRuns.removeAt(0);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::removeBadAppendOne(void)
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QString testRun;
    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< QPair<QString, Verbosity> > testRuns;

    // Try to remove non existing test
    SUB_TEST_FUNCTION(removeTestAt(&model, 0));
    //testRuns.removeAt(0);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Append one test
    testRun = mTests.at(qrand() % mTests.size());
    qDebug() << testRun;
    SUB_TEST_FUNCTION(appendTest(&model, testRun, Normal, outputFormat, errorFormat));
    testRuns.append(QPair<QString, Verbosity>(testRun, Normal));
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::appendOneRemoveBad()
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QString testRun;
    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< QPair<QString, Verbosity> > testRuns;

    // Append one test
    testRun = mTests.at(qrand() % mTests.size());
    qDebug() << testRun;
    SUB_TEST_FUNCTION(appendTest(&model, testRun, Normal, outputFormat, errorFormat));
    testRuns.append(QPair<QString, Verbosity>(testRun, Normal));
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Try to remove non existing test
    SUB_TEST_FUNCTION(removeTestAt(&model, 1));
    //testRuns.removeAt(1);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::appendOneClear()
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QString testRun;
    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< QPair<QString, Verbosity> > testRuns;

    // Append one test
    testRun = mTests.at(qrand() % mTests.size());
    qDebug() << testRun;
    SUB_TEST_FUNCTION(appendTest(&model, testRun, Normal, outputFormat, errorFormat));
    testRuns.append(QPair<QString, Verbosity>(testRun, Normal));
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Clear test suite
    SUB_TEST_FUNCTION(clearTests(&model));
    testRuns.clear();
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::appendTwo(void)
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QString testRun;
    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< QPair<QString, Verbosity> > testRuns;

    // Append one test
    testRun = mTests.at(qrand() % mTests.size());
    qDebug() << testRun;
    SUB_TEST_FUNCTION(appendTest(&model, testRun, Normal, outputFormat, errorFormat));
    testRuns.append(QPair<QString, Verbosity>(testRun, Normal));
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Append one test
    testRun = mTests.at(qrand() % mTests.size());
    qDebug() << testRun;
    SUB_TEST_FUNCTION(appendTest(&model, testRun, Normal, outputFormat, errorFormat));
    testRuns.append(QPair<QString, Verbosity>(testRun, Normal));
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::appendTwoRemoveFirst(void)
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QString testRun;
    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< QPair<QString, Verbosity> > testRuns;

    // Append one test
    testRun = mTests.at(qrand() % mTests.size());
    qDebug() << testRun;
    SUB_TEST_FUNCTION(appendTest(&model, testRun, Normal, outputFormat, errorFormat));
    testRuns.append(QPair<QString, Verbosity>(testRun, Normal));
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Append one test
    testRun = mTests.at(qrand() % mTests.size());
    qDebug() << testRun;
    SUB_TEST_FUNCTION(appendTest(&model, testRun, Normal, outputFormat, errorFormat));
    testRuns.append(QPair<QString, Verbosity>(testRun, Normal));
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Remove first test
    SUB_TEST_FUNCTION(removeTestAt(&model, 0));
    testRuns.removeAt(0);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::appendTwoRemoveSecond(void)
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QString testRun;
    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< QPair<QString, Verbosity> > testRuns;

    // Append one test
    testRun = mTests.at(qrand() % mTests.size());
    qDebug() << testRun;
    SUB_TEST_FUNCTION(appendTest(&model, testRun, Normal, outputFormat, errorFormat));
    testRuns.append(QPair<QString, Verbosity>(testRun, Normal));
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Append one test
    testRun = mTests.at(qrand() % mTests.size());
    qDebug() << testRun;
    SUB_TEST_FUNCTION(appendTest(&model, testRun, Normal, outputFormat, errorFormat));
    testRuns.append(QPair<QString, Verbosity>(testRun, Normal));
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Remove second test
    SUB_TEST_FUNCTION(removeTestAt(&model, 1));
    testRuns.removeAt(1);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::appendTwoRemoveBad(void)
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QString testRun;
    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< QPair<QString, Verbosity> > testRuns;

    // Append one test
    testRun = mTests.at(qrand() % mTests.size());
    qDebug() << testRun;
    SUB_TEST_FUNCTION(appendTest(&model, testRun, Normal, outputFormat, errorFormat));
    testRuns.append(QPair<QString, Verbosity>(testRun, Normal));
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Append one test
    testRun = mTests.at(qrand() % mTests.size());
    qDebug() << testRun;
    SUB_TEST_FUNCTION(appendTest(&model, testRun, Normal, outputFormat, errorFormat));
    testRuns.append(QPair<QString, Verbosity>(testRun, Normal));
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Try to remove non existing test
    SUB_TEST_FUNCTION(removeTestAt(&model, 2));
    //testRuns.removeAt(2);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::appendOneRemoveAppendOne(void)
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QString testRun;
    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< QPair<QString, Verbosity> > testRuns;

    // Append one test
    testRun = mTests.at(qrand() % mTests.size());
    qDebug() << testRun;
    SUB_TEST_FUNCTION(appendTest(&model, testRun, Normal, outputFormat, errorFormat));
    testRuns.append(QPair<QString, Verbosity>(testRun, Normal));
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Remove first test
    SUB_TEST_FUNCTION(removeTestAt(&model, 0));
    testRuns.removeAt(0);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Append one test
    testRun = mTests.at(qrand() % mTests.size());
    qDebug() << testRun;
    SUB_TEST_FUNCTION(appendTest(&model, testRun, Normal, outputFormat, errorFormat));
    testRuns.append(QPair<QString, Verbosity>(testRun, Normal));
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

}

void TestSuiteModelTest::appendOneRemoveBadAppendOne(void)
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QString testRun;
    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< QPair<QString, Verbosity> > testRuns;

    // Append one test
    testRun = mTests.at(qrand() % mTests.size());
    qDebug() << testRun;
    SUB_TEST_FUNCTION(appendTest(&model, testRun, Normal, outputFormat, errorFormat));
    testRuns.append(QPair<QString, Verbosity>(testRun, Normal));
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Try to remove non existing test
    SUB_TEST_FUNCTION(removeTestAt(&model, 1));
    //testRuns.removeAt(1);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Append one test
    testRun = mTests.at(qrand() % mTests.size());
    qDebug() << testRun;
    SUB_TEST_FUNCTION(appendTest(&model, testRun, Normal, outputFormat, errorFormat));
    testRuns.append(QPair<QString, Verbosity>(testRun, Normal));
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::appendTwoClear(void)
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QString testRun;
    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< QPair<QString, Verbosity> > testRuns;

    // Append one test
    testRun = mTests.at(qrand() % mTests.size());
    qDebug() << testRun;
    SUB_TEST_FUNCTION(appendTest(&model, testRun, Normal, outputFormat, errorFormat));
    testRuns.append(QPair<QString, Verbosity>(testRun, Normal));
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Append one test
    testRun = mTests.at(qrand() % mTests.size());
    qDebug() << testRun;
    SUB_TEST_FUNCTION(appendTest(&model, testRun, Normal, outputFormat, errorFormat));
    testRuns.append(QPair<QString, Verbosity>(testRun, Normal));
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Clear test suite
    SUB_TEST_FUNCTION(clearTests(&model));
    testRuns.clear();
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::appendOneClearAppendOne(void)
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QString testRun;
    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< QPair<QString, Verbosity> > testRuns;

    // Append one test
    testRun = mTests.at(qrand() % mTests.size());
    qDebug() << testRun;
    SUB_TEST_FUNCTION(appendTest(&model, testRun, Normal, outputFormat, errorFormat));
    testRuns.append(QPair<QString, Verbosity>(testRun, Normal));
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Clear test suite
    SUB_TEST_FUNCTION(clearTests(&model));
    testRuns.clear();
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Append one test
    testRun = mTests.at(qrand() % mTests.size());
    qDebug() << testRun;
    SUB_TEST_FUNCTION(appendTest(&model, testRun, Normal, outputFormat, errorFormat));
    testRuns.append(QPair<QString, Verbosity>(testRun, Normal));
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::isOutput(const QDomElement& element, Verbosity verbose, bool *ret)
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

void TestSuiteModelTest::parseSuiteRoot(const QAbstractItemModel* model, const QList<QPair<QString, Verbosity> > &tests)
{
    BEGIN_SUB_TEST_FUNCTION

    QVERIFY(model->rowCount(QModelIndex()) == tests.size());
    QVERIFY(model->columnCount(QModelIndex()) == 1);

    int i = 0;
    QList< QPair<QString, Verbosity> >::const_iterator it = tests.constBegin();
    for (; it!= tests.constEnd(); it++, i++) {
        QVERIFY2(model->index(i, 0, QModelIndex()).isValid(), qPrintable(QString("Children %1 of root element is not valid").arg(i)));

        /* Load the XML expected result */
        QDomDocument dom((*it).first);
        QFile domFile(TESTS_DIR "/" + (*it).first + "/" + (*it).first.toLower() + ".xml");
        QVERIFY(domFile.open(QIODevice::ReadOnly));
        QString error;
        int line = 0;
        int column = 0;
        QVERIFY2(dom.setContent(&domFile, false, &error, &line, &column), qPrintable(QString("%1 at %2:%3").arg(error).arg(line).arg(column)));
        QVERIFY(QString::compare(dom.documentElement().tagName(), "qtestliboutput", Qt::CaseSensitive) == 0);

        /* Test each index recursively */
        bool isElementOutput = false;
        QDomElement root = dom.documentElement().firstChildElement("root");
        QDomElement rootClass = dom.documentElement().firstChildElement("class");
        QVERIFY2(root.isNull() ^ rootClass.isNull(), "The document element should have either root or class child");
        if (!root.isNull()) {
            SUB_TEST_FUNCTION(isOutput(root, (*it).second, &isElementOutput));
            if (isElementOutput) {
                QVERIFY2(root.nextSiblingElement("root").isNull(), "The document element should have only one root child");
                SUB_TEST_FUNCTION(parseRoot(model, model->index(i, 0, QModelIndex()), root, (*it).second));
            }
        }
        else if (!rootClass.isNull()) {
            SUB_TEST_FUNCTION(isOutput(rootClass, (*it).second, &isElementOutput));
            if (isElementOutput) {
                QVERIFY2(rootClass.nextSiblingElement("class").isNull(), "The document element should have only one class child");
                SUB_TEST_FUNCTION(parseClass(model, model->index(i, 0, QModelIndex()), rootClass, (*it).second));
            }
        }
    }

    END_SUB_TEST_FUNCTION
}

void TestSuiteModelTest::parseRoot(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element, Verbosity verbosity)
{
    BEGIN_SUB_TEST_FUNCTION

    QVERIFY2(model->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).type() == QVariant::String, "Result string role for root index should be a string");
    QVERIFY2(QString::compare(model->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).toString(), element.attribute("type", QString::null), Qt::CaseInsensitive) == 0, "Result string for root do not match");

    QDomElement classElement = element.firstChildElement("class");
    int i = 0;
    while (!classElement.isNull()) {
        bool isElementOutput = false;
        SUB_TEST_FUNCTION(isOutput(classElement, verbosity, &isElementOutput));
        if (isElementOutput) {
            QVERIFY2(model->index(i, 0, index).isValid(), qPrintable(QString("Children %1 of sub model root element is not valid").arg(i)));
            SUB_TEST_FUNCTION(parseClass(model, model->index(i, 0, index), classElement, verbosity));
            i++;
        }
        classElement = classElement.nextSiblingElement("class");
    }

    QVERIFY2(model->rowCount(index) == i, qPrintable(QString("The model index should have %1 rows").arg(i)));
    QVERIFY2(model->columnCount(index) == 1, "The model index should have 1 column.");

    END_SUB_TEST_FUNCTION
}

void TestSuiteModelTest::parseClass(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element, Verbosity verbosity)
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

void TestSuiteModelTest::parseFunction(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element, Verbosity verbosity)
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

void TestSuiteModelTest::parseRow(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element, Verbosity verbosity)
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

void TestSuiteModelTest::parseMessage(const QAbstractItemModel* model, const QModelIndex& index, const QDomElement& element)
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

void TestSuiteModelTest::appendTest(QTestLibPlugin::Internal::TestSuiteModel *model, const QString& test, Verbosity verbosity, Utils::OutputFormat outputFormat, Utils::OutputFormat errorFormat)
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
    model->appendTestRun(runControl);
    QSignalSpy rowsAboutToBeInsertedSpy(model, SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)));
    QSignalSpy rowsInsertedSpy(model, SIGNAL(rowsInserted(QModelIndex, int, int)));
    runControl->start();

    QVERIFY2(rowsInsertedSpy.wait(30000), "RowsInserted signal was not emitted within 30s");
    QVERIFY2(rowsAboutToBeInsertedSpy.count() == 1, "RowsAboutToBeInserted was not emitted before rowsInserted sinal");

    SUB_TEST_FUNCTION(checkSignalArguments("rowsAboutToBeInserted()", rowsAboutToBeInsertedSpy.takeFirst(), model->rowCount() - 1, model->rowCount() - 1));
    SUB_TEST_FUNCTION(checkSignalArguments("rowsInserted()", rowsInsertedSpy.takeFirst(), model->rowCount() - 1, model->rowCount() - 1));

    END_SUB_TEST_FUNCTION
}

void TestSuiteModelTest::removeTestAt(QTestLibPlugin::Internal::TestSuiteModel *model, int i)
{
    BEGIN_SUB_TEST_FUNCTION

    QSignalSpy rowsAboutToBeRemovedSpy(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)));
    QSignalSpy rowsRemovedSpy(model, SIGNAL(rowsRemoved(QModelIndex, int, int)));

    int emissions = (int) (i >= 0) && (i < model->rowCount());
    model->removeTestRun(i);

    QVERIFY2(rowsAboutToBeRemovedSpy.count() == emissions, "RowsAboutToBeRemoved was not emitted");
    QVERIFY2(rowsRemovedSpy.count() == emissions, "RowsRemoved signal was not emitted");

    if (emissions == 1) {
        SUB_TEST_FUNCTION(checkSignalArguments("rowsAboutToBeRemoved()", rowsAboutToBeRemovedSpy.takeFirst(), i, i));
        SUB_TEST_FUNCTION(checkSignalArguments("rowsRemoved()", rowsRemovedSpy.takeFirst(), i, i));
    }

    END_SUB_TEST_FUNCTION
}

void TestSuiteModelTest::clearTests(QTestLibPlugin::Internal::TestSuiteModel *model)
{
    BEGIN_SUB_TEST_FUNCTION

    QSignalSpy modelAboutToBeResetSpy(model, SIGNAL(modelAboutToBeReset()));
    QSignalSpy modelResetSpy(model, SIGNAL(modelReset()));

    model->clear();

    QVERIFY2(modelAboutToBeResetSpy.count() == 1, "RowsAboutToBeRemoved was not emitted");
    QVERIFY2(modelResetSpy.count() == 1, "RowsRemoved signal was not emitted");

    END_SUB_TEST_FUNCTION
}

void TestSuiteModelTest::checkSignalArguments(const QString& signal, const QList<QVariant>& args, int first, int last)
{
    BEGIN_SUB_TEST_FUNCTION

    QVERIFY2(args.at(0).type() == QVariant::ModelIndex,
             qPrintable(QString("The first argument of signal %1 should be a QModelIndex").arg(signal)));
    QVERIFY2(!args.at(0).toModelIndex().isValid(),
             qPrintable(QString("The first argument of signal %1 should be an invalid QModelIndex").arg(signal)));
    QVERIFY2(args.at(1).type() == QVariant::Int,
             qPrintable(QString("The second argument of signal %1 should be an integer").arg(signal)));
    QVERIFY2(args.at(1).toInt() == first,
             qPrintable(QString("The second argument of signal %1 should be %2").arg(signal).arg(first)));
    QVERIFY2(args.at(2).type() == QVariant::Int,
             qPrintable(QString("The third argument of signal %1 should be an integer").arg(signal)));
    QVERIFY2(args.at(2).toInt() == last,
             qPrintable(QString("The third argument of signal %1 should be %2").arg(signal).arg(last)));

    END_SUB_TEST_FUNCTION
}

QTEST_MAIN(TestSuiteModelTest)

#include "testsuitemodeltest.moc"
