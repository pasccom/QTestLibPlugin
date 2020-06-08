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

#include <qtestlibmodel.h>
#include <testsuitemodel.h>
using namespace QTestLibPlugin::Internal;

#include "../common/qtestlibmodeltester.h"
#include "../common/qttestsubfunction.h"

#include <QtXml>
#include <QtTest>

class TestSuiteModelSignalTest : public QObject
{
    Q_OBJECT
    HAS_SUB_TEST_FUNCTIONS
public:
    typedef QPair<QString, QVariantList> Signal;
    TestSuiteModelSignalTest(void);
private Q_SLOTS:
    void cleanup(void);
    inline void addTestItem_data(void) {testNames();}
    void addTestItem(void);
    inline void appendTestItemMessage_data(void) {testNames();}
    void appendTestItemMessage(void);
    inline void appendTestLocation_data(void) {testNames();}
    void appendTestLocation(void);
private slots:
    void rowsAboutToBeInserted(const QModelIndex& index, int first, int last);
    void rowsInserted(const QModelIndex& index, int first, int last);
    void rowsAboutToBeMoved(const QModelIndex& src, int first, int last, const QModelIndex& dst, int row);
    void rowsMoved(const QModelIndex& src, int first, int last, const QModelIndex& dst, int row);
    void dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
private:
    void testNames(void);

    void loadTest(QDomDocument& dom, const QString& testName);
    void loadResult(QDomDocument& dom, const QString& testName);

    void createSpies(const QAbstractItemModel* model, const QDomElement& testSignals);
    void createSpy(const QAbstractItemModel* model, const QString& signal);
    void checkSignals(const QAbstractItemModel* model, const QDomElement& testSignals, const QString& testName);
    void checkSignal(const QAbstractItemModel* model, const QVariantList& argList, const QDomElement& testSignal, const QDomElement& testResult);

    QModelIndex index(const QString& id, const QDomElement& element, const QAbstractItemModel* model, const QModelIndex& idx = QModelIndex());
    QModelIndex index(const QAbstractItemModel* model, const QStringList& list, int column, const QModelIndex& idx = QModelIndex());

    QLinkedList<QString> mConnectedSignals;
    QLinkedList<Signal> mSignals;

    TestSuiteModel *mModel;
    QModelIndex subModelIndex;
};

TestSuiteModelSignalTest::TestSuiteModelSignalTest(void)
{
    mModel = new TestSuiteModel(this);
}

void TestSuiteModelSignalTest::cleanup(void)
{
    foreach (QString signal, mConnectedSignals) {
        if (QString::compare(signal, "rowsAboutToBeInserted", Qt::CaseSensitive) == 0)
            disconnect(mModel, SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int)),
                       this, SLOT(rowsAboutToBeInserted(const QModelIndex&, int, int)));
        else if (QString::compare(signal, "rowsInserted", Qt::CaseSensitive) == 0)
            disconnect(mModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
                       this, SLOT(rowsInserted(const QModelIndex&, int, int)));
        else if (QString::compare(signal, "rowsAboutToBeMoved", Qt::CaseSensitive) == 0)
            disconnect(mModel, SIGNAL(rowsAboutToBeMoved(const QModelIndex&, int, int, const QModelIndex&, int)),
                       this, SLOT(rowsAboutToBeMoved(const QModelIndex&, int, int, const QModelIndex&, int)));
        else if (QString::compare(signal, "rowsMoved", Qt::CaseSensitive) == 0)
            disconnect(mModel, SIGNAL(rowsMoved(const QModelIndex&, int, int, const QModelIndex&, int)),
                       this, SLOT(rowsMoved(const QModelIndex&, int, int, const QModelIndex&, int)));
        else if (QString::compare(signal, "dataChanged", Qt::CaseSensitive) == 0)
            disconnect(mModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
                       this, SLOT(dataChanged(const QModelIndex&, const QModelIndex&)));
        // TODO add other signals
        else
            qCritical() << QString("Unknown signal: %1").arg(signal);
    }
}

void TestSuiteModelSignalTest::rowsAboutToBeInserted(const QModelIndex& index, int first, int last)
{
    QVariantList argsList;
    argsList.append(index);
    argsList.append(first);
    argsList.append(last);

    mSignals.append(QPair<QString, QVariantList>(__func__, argsList));
}

void TestSuiteModelSignalTest::rowsInserted(const QModelIndex& index, int first, int last)
{
    QVariantList argsList;
    argsList.append(index);
    argsList.append(first);
    argsList.append(last);

    mSignals.append(QPair<QString, QVariantList>(__func__, argsList));
}

void TestSuiteModelSignalTest::rowsAboutToBeMoved(const QModelIndex& src, int first, int last, const QModelIndex& dst, int row)
{
    QVariantList argsList;
    argsList.append(src);
    argsList.append(first);
    argsList.append(last);
    argsList.append(dst);
    argsList.append(row);

    mSignals.append(QPair<QString, QVariantList>(__func__, argsList));
}

void TestSuiteModelSignalTest::rowsMoved(const QModelIndex& src, int first, int last, const QModelIndex& dst, int row)
{
    QVariantList argsList;
    argsList.append(src);
    argsList.append(first);
    argsList.append(last);
    argsList.append(dst);
    argsList.append(row);

    mSignals.append(QPair<QString, QVariantList>(__func__, argsList));
}


void TestSuiteModelSignalTest::dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    QVariantList argsList;
    argsList.append(topLeft);
    argsList.append(bottomRight);

    mSignals.append(QPair<QString, QVariantList>(__func__, argsList));
}

void TestSuiteModelSignalTest::testNames(void)
{
    mModel->clear();

    QTest::addColumn<QString>("testName");

#ifdef TESTS_LST_EXISTS
#   include "../QTestLibModelTest/tests/tests.lst"
#endif
}

void TestSuiteModelSignalTest::addTestItem(void)
{
    QFETCH(QString, testName);

    QDomDocument dom;
    SUB_TEST_FUNCTION(loadTest(dom, testName));
    QDomElement testItems = dom.documentElement().firstChildElement("items");
    QDomElement testSignals = dom.documentElement().firstChildElement("signals");
    QVERIFY2(!testItems.isNull() && testItems.nextSiblingElement("items").isNull(), "There should be one and only one items element in root childrens");
    QVERIFY2(!testSignals.isNull() && testSignals.nextSiblingElement("signals").isNull(), "There should be one and only one signals element in root childrens");

    QTestLibModel *model = new QTestLibModel(NULL);
    mModel->appendSubModel(model);
    subModelIndex = mModel->index(mModel->rowCount() - 1, 0, QModelIndex());
    SUB_TEST_FUNCTION(createSpies(mModel, testSignals));

    QDomElement testItem = testItems.firstChildElement("item");
    while (!testItem.isNull()) {
        QVERIFY2(testItem.firstChild().isText(), "Child of item element should be text");
        QVERIFY2(testItem.firstChild().nextSibling().isNull(), "item element should have only one child");
        qDebug() << testItem.attribute("class")
                 << testItem.attribute("function")
                 << testItem.attribute("row")
                 << testItem.firstChild().toText().data();
        model->addTestItem(NULL,
                           QTestLibModel::Unknown,
                           testItem.attribute("class"),
                           testItem.attribute("function"),
                           testItem.attribute("row"),
                           testItem.firstChild().toText().data());
        testItem = testItem.nextSiblingElement("item");
    }

    foreach (Signal signal, mSignals) {
        qDebug() << signal;
    }

    QTestLibModelTester tester(mModel);
    tester.setResultsFile(TESTS_DIR "/" + testName + "/" + testName.toLower() + ".xml");
    QVERIFY2(tester.checkIndex(subModelIndex), qPrintable(tester.error()));

    SUB_TEST_FUNCTION(checkSignals(mModel, testSignals, testName));
}

void TestSuiteModelSignalTest::appendTestItemMessage(void)
{
    QFETCH(QString, testName);

    QDomDocument dom;
    SUB_TEST_FUNCTION(loadTest(dom, testName));
    QDomElement testItems = dom.documentElement().firstChildElement("items");
    QDomElement testSignals = dom.documentElement().firstChildElement("signals");
    QVERIFY2(!testItems.isNull() && testItems.nextSiblingElement("items").isNull(), "There should be one and only one items element in root childrens");
    QVERIFY2(!testSignals.isNull() && testSignals.nextSiblingElement("signals").isNull(), "There should be one and only one signals element in root childrens");

    QTestLibModel *model = new QTestLibModel(NULL);
    mModel->appendSubModel(model);
    subModelIndex = mModel->index(mModel->rowCount() - 1, 0, QModelIndex());
    QSignalSpy dataChangedSpy(mModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)));

    QDomElement testItem = testItems.firstChildElement("item");
    QString root;
    while (!testItem.isNull()) {
        QVERIFY2(testItem.firstChild().isText(), "Child of item element should be text");
        QVERIFY2(testItem.firstChild().nextSibling().isNull(), "item element should have only one child");
        qDebug() << testItem.attribute("class")
                 << testItem.attribute("function")
                 << testItem.attribute("row")
                 << testItem.firstChild().toText().data();

        if (root.isNull()) {
            if (testItem.attribute("class").isNull())
                root = "root";
            else
                root = "class:" + testItem.attribute("class");
        } else if (root.startsWith("class:", Qt::CaseSensitive)) {
            if (QString::compare(testItem.attribute("class"), root.mid(6)) != 0)
                root = "root";
        }

        model->addTestItem(NULL,
                           QTestLibModel::Unknown,
                           testItem.attribute("class"),
                           testItem.attribute("function"),
                           testItem.attribute("row"),
                           testItem.firstChild().toText().data());

        QStringList list;
        if (QString::compare(root, "root", Qt::CaseSensitive) == 0)
            list << testItem.attribute("class");
        list << testItem.attribute("function");
        list << testItem.attribute("row");

        QModelIndex currentIndex = index(mModel, list, 0, subModelIndex);
        qDebug() << currentIndex;

        dataChangedSpy.clear();
        model->appendTestItemMessage(NULL, "Appended message");
        QVERIFY2(dataChangedSpy.count() == 1, "dataChanged() signal should be emitted once.");
        QVERIFY2(dataChangedSpy.first().at(0).type() == QVariant::ModelIndex, "Invalid type for signal argument 1");
        QVERIFY2(dataChangedSpy.first().at(1).type() == QVariant::ModelIndex, "Invalid type for signal argument 2");
        qDebug() << dataChangedSpy.first().at(0).toModelIndex();
        qDebug() << dataChangedSpy.first().at(1).toModelIndex();
        QVERIFY2(dataChangedSpy.first().at(0).toModelIndex() == currentIndex, "Bad value for signal argument 1");
        QVERIFY2(dataChangedSpy.first().at(1).toModelIndex() == currentIndex, "Bad value for signal argument 2");

        testItem = testItem.nextSiblingElement("item");
    }
}

void TestSuiteModelSignalTest::appendTestLocation(void)
{
    QFETCH(QString, testName);

    QDomDocument dom;
    SUB_TEST_FUNCTION(loadTest(dom, testName));
    QDomElement testItems = dom.documentElement().firstChildElement("items");
    QDomElement testSignals = dom.documentElement().firstChildElement("signals");
    QVERIFY2(!testItems.isNull() && testItems.nextSiblingElement("items").isNull(), "There should be one and only one items element in root childrens");
    QVERIFY2(!testSignals.isNull() && testSignals.nextSiblingElement("signals").isNull(), "There should be one and only one signals element in root childrens");

    QTestLibModel *model = new QTestLibModel(NULL);
    mModel->appendSubModel(model);
    subModelIndex = mModel->index(mModel->rowCount() - 1, 0, QModelIndex());
    QSignalSpy dataChangedSpy(mModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)));

    QDomElement testItem = testItems.firstChildElement("item");
    QString root;
    while (!testItem.isNull()) {
        QVERIFY2(testItem.firstChild().isText(), "Child of item element should be text");
        QVERIFY2(testItem.firstChild().nextSibling().isNull(), "item element should have only one child");
        qDebug() << testItem.attribute("class")
                 << testItem.attribute("function")
                 << testItem.attribute("row")
                 << testItem.firstChild().toText().data();

        if (root.isNull()) {
            if (testItem.attribute("class").isNull())
                root = "root";
            else
                root = "class:" + testItem.attribute("class");
        } else if (root.startsWith("class:", Qt::CaseSensitive)) {
            if (QString::compare(testItem.attribute("class"), root.mid(6)) != 0)
                root = "root";
        }

        model->addTestItem(NULL,
                           QTestLibModel::Unknown,
                           testItem.attribute("class"),
                           testItem.attribute("function"),
                           testItem.attribute("row"),
                           testItem.firstChild().toText().data());

        QStringList list;
        if (QString::compare(root, "root", Qt::CaseSensitive) == 0)
            list << testItem.attribute("class");
        list << testItem.attribute("function");
        list << testItem.attribute("row");

        QModelIndex firstIndex = index(mModel, list, 1, subModelIndex);
        QModelIndex lastIndex = index(mModel, list, 2, subModelIndex);
        qDebug() << firstIndex;
        qDebug() << lastIndex;

        dataChangedSpy.clear();
        model->appendTestLocation(NULL, "Unknown file", 0);
        QVERIFY2(dataChangedSpy.count() == 1, "dataChanged() signal should be emitted once.");
        QVERIFY2(dataChangedSpy.first().at(0).type() == QVariant::ModelIndex, "Invalid type for signal argument 1");
        QVERIFY2(dataChangedSpy.first().at(1).type() == QVariant::ModelIndex, "Invalid type for signal argument 2");
        qDebug() << dataChangedSpy.first().at(0).toModelIndex();
        qDebug() << dataChangedSpy.first().at(1).toModelIndex();
        QVERIFY2(dataChangedSpy.first().at(0).toModelIndex() == firstIndex, "Bad value for signal argument 1");
        QVERIFY2(dataChangedSpy.first().at(1).toModelIndex() == lastIndex, "Bad value for signal argument 2");

        testItem = testItem.nextSiblingElement("item");
    }
}

void TestSuiteModelSignalTest::loadTest(QDomDocument& dom, const QString& testName)
{
    BEGIN_SUB_TEST_FUNCTION

    QFile domFile(TESTS_DIR "/" + testName + "/" + testName.toLower() + "test.xml");
    QVERIFY(domFile.open(QIODevice::ReadOnly));
    QString error;
    int line = 0;
    int column = 0;
    QVERIFY2(dom.setContent(&domFile, false, &error, &line, &column), qPrintable(QString("%1 at %2:%3").arg(error).arg(line).arg(column)));
    QVERIFY(QString::compare(dom.documentElement().tagName(), "qtestlibmodeltest", Qt::CaseSensitive) == 0);

    END_SUB_TEST_FUNCTION
}

void TestSuiteModelSignalTest::loadResult(QDomDocument& dom, const QString& testName)
{
    BEGIN_SUB_TEST_FUNCTION

    QFile domFile(TESTS_DIR "/" + testName + "/" + testName.toLower() + ".xml");
    QVERIFY(domFile.open(QIODevice::ReadOnly));
    QString error;
    int line = 0;
    int column = 0;
    QVERIFY2(dom.setContent(&domFile, false, &error, &line, &column), qPrintable(QString("%1 at %2:%3").arg(error).arg(line).arg(column)));
    QVERIFY(QString::compare(dom.documentElement().tagName(), "qtestliboutput", Qt::CaseSensitive) == 0);

    END_SUB_TEST_FUNCTION
}

void TestSuiteModelSignalTest::createSpies(const QAbstractItemModel* model, const QDomElement& testSignals)
{
    BEGIN_SUB_TEST_FUNCTION

    QDomElement testSignal = testSignals.firstChildElement("signal");
    mSignals.clear();
    mConnectedSignals.clear();
    while (!testSignal.isNull()) {
        QVERIFY2(testSignal.hasAttribute("name"), "Signal element should have name attribute");
        SUB_TEST_FUNCTION(createSpy(model, testSignal.attribute("name")));
        testSignal = testSignal.nextSiblingElement("signal");
    }

    END_SUB_TEST_FUNCTION
}

void TestSuiteModelSignalTest::createSpy(const QAbstractItemModel* model, const QString& signal)
{
    BEGIN_SUB_TEST_FUNCTION

    if (mConnectedSignals.contains(signal))
        END_SUB_TEST_FUNCTION
    mConnectedSignals.append(signal);

    if (QString::compare(signal, "rowsAboutToBeInserted", Qt::CaseSensitive) == 0)
        connect(model, SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int)),
                this, SLOT(rowsAboutToBeInserted(const QModelIndex&, int, int)));
    else if (QString::compare(signal, "rowsInserted", Qt::CaseSensitive) == 0)
        connect(model, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
                this, SLOT(rowsInserted(const QModelIndex&, int, int)));
    else if (QString::compare(signal, "rowsAboutToBeMoved", Qt::CaseSensitive) == 0)
        connect(model, SIGNAL(rowsAboutToBeMoved(const QModelIndex&, int, int, const QModelIndex&, int)),
                this, SLOT(rowsAboutToBeMoved(const QModelIndex&, int, int, const QModelIndex&, int)));
    else if (QString::compare(signal, "rowsMoved", Qt::CaseSensitive) == 0)
        connect(model, SIGNAL(rowsMoved(const QModelIndex&, int, int, const QModelIndex&, int)),
                this, SLOT(rowsMoved(const QModelIndex&, int, int, const QModelIndex&, int)));
    else if (QString::compare(signal, "dataChanged", Qt::CaseSensitive) == 0)
        connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
                this, SLOT(dataChanged(const QModelIndex&, const QModelIndex&)));
    // TODO add other signals
    else
        QVERIFY2(false, qPrintable(QString("Unknown signal: %1").arg(signal)));

    END_SUB_TEST_FUNCTION
}

void TestSuiteModelSignalTest::checkSignals(const QAbstractItemModel* model, const QDomElement& testSignals, const QString& testName)
{
    BEGIN_SUB_TEST_FUNCTION

    QDomDocument dom;
    SUB_TEST_FUNCTION(loadResult(dom, testName));
    QVERIFY2(QString::compare(dom.documentElement().tagName(), "qtestliboutput", Qt::CaseSensitive) == 0, "Test result root element should be \"qtestliboutput\"");

    QDomElement testSignal = testSignals.firstChildElement("signal");
    QLinkedList<Signal>::const_iterator it = mSignals.constBegin();
    while (!testSignal.isNull() && (it != mSignals.constEnd())) {
        QVERIFY2(testSignal.hasAttribute("name"), "Signal element should have name attribute");
        QVERIFY2(QString::compare((*it).first, testSignal.attribute("name"), Qt::CaseSensitive) == 0, "Signal name mismatch");
        SUB_TEST_FUNCTION(checkSignal(model, (*it).second, testSignal, dom.documentElement().firstChildElement()));
        testSignal = testSignal.nextSiblingElement("signal");
        it++;
    }

    if (!testSignal.isNull() || (it != mSignals.constEnd()))
        QVERIFY2(false, "Signal number mismatch");

    END_SUB_TEST_FUNCTION
}

void TestSuiteModelSignalTest::checkSignal(const QAbstractItemModel* model, const QVariantList& argList, const QDomElement& testSignal, const QDomElement& testResult)
{
    BEGIN_SUB_TEST_FUNCTION

    QDomElement signalArg = testSignal.firstChildElement("arg");
    QVariantList::const_iterator it = argList.constBegin();
    while (!signalArg.isNull() && (it != argList.constEnd())) {
        QVERIFY2(signalArg.hasAttribute("type"), "Signal argument must have a \"type\" attribute");
        QVERIFY2(signalArg.hasAttribute("value"), "Signal argument must have a \"value\" attribute");
        if (QString::compare(signalArg.attribute("type"), "Integer", Qt::CaseSensitive) == 0) {
            bool ok = false;
            int value = signalArg.attribute("value").toInt(&ok, 10);
            //qDebug() << value << (*it).toInt();
            QVERIFY2(ok, qPrintable(QString("Conversion of %1 to int failed").arg(signalArg.attribute("value"))));
            QVERIFY2((*it).type() == QVariant::Int, "Signal argument value should be an integer");
            QVERIFY2((*it).toInt() == value, "Signal argument value mismatch");
        } else if(QString::compare(signalArg.attribute("type"), "QModelIndex", Qt::CaseSensitive) == 0) {
            QModelIndex value = index(signalArg.attribute("value"), testResult, model, subModelIndex);
            qDebug() << signalArg.attribute("value") << value << (*it).toModelIndex();
            QVERIFY2((*it).type() == QVariant::ModelIndex, "Signal argument value should be a model index");
            QVERIFY2((*it).toModelIndex().internalPointer() == value.internalPointer(), "Signal argument value mismatch");
        } else {
            QVERIFY2(false, "Unknown argument type");
        }

        signalArg = signalArg.nextSiblingElement("arg");
        it++;
    }

    if (!signalArg.isNull() || (it != argList.constEnd()))
        QVERIFY2(false, "Signal argument number mismatch");

    END_SUB_TEST_FUNCTION
}

QModelIndex TestSuiteModelSignalTest::index(const QString& id, const QDomElement& element, const QAbstractItemModel* model, const QModelIndex& idx)
{
    if (QString::compare(id, "root", Qt::CaseSensitive) == 0)
        return idx;
    if (element.hasAttribute("id") && (QString::compare(element.attribute("id"), id, Qt::CaseSensitive) == 0))
        return idx;

    int childRow = 0;
    QDomElement childElement = element.firstChildElement();
    while ((childRow < model->rowCount(idx)) && !childElement.isNull()) {
        QModelIndex matchedIdx = index(id, childElement, model, model->index(childRow, 0, idx));
        if (matchedIdx.isValid())
            return matchedIdx;
        childRow++;
        childElement = childElement.nextSiblingElement();
    }

    return QModelIndex();
}

QModelIndex TestSuiteModelSignalTest::index(const QAbstractItemModel* model, const QStringList& list, int column, const QModelIndex& idx)
{
    if (list.isEmpty() || list.first().isNull())
        return model->index(model->rowCount(idx) - 1, column, idx);

    QStringList newList = list;
    QString name = newList.takeFirst();

    for (int r = 0; r < model->rowCount(idx); r++)
        if (QString::compare(model->data(model->index(r, 0, idx), Qt::DisplayRole).toString(), name, Qt::CaseSensitive) == 0)
            return index(model, newList, column, model->index(r, 0, idx));

    return QModelIndex();
}

QTEST_APPLESS_MAIN(TestSuiteModelSignalTest)

#include "testsuitemodelsignaltest.moc"
