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

#include "qtestlibmodeltester.h"

#include "../../qtestlibmodel.h"

#include <QtXml>

#define QVERIFY(_expr_) \
    do { \
        if (!(_expr_)) { \
            mError = QString("'%1' returned FALSE.\n   Loc: [%2(%3)]").arg(#_expr_).arg(__FILE__).arg(__LINE__); \
            return; \
        } \
    } while(false); \

#define QVERIFY2(_expr_, _desc_) \
    do { \
        if (!(_expr_)) { \
            mError = QString("'%1' returned FALSE (%2).\n   Loc: [%3(%4)]").arg(#_expr_).arg(_desc_).arg(__FILE__).arg(__LINE__); \
            return; \
        } \
    } while(false); \

QTestLibModelTester::QTestLibModelTester(const QAbstractItemModel *model, Verbosity verbosity, const QString& format) :
    mModel(model), mError(QString::null), mParserFormat(format), mVerbosity(verbosity)
{
    mFilters.resize((int) QTestLibPlugin::Internal::QTestLibModel::LastMessageType);

    for (int i = 0; i < mFilters.size(); i++)
        mFilters[i] = true;
}

bool QTestLibModelTester::checkResults(const QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult>& results, const QString& testName)
{
    mError = QString::null;

    checkResultsInternal(results, testName);

    return SUB_TEST_FUNCTION_SUCESS_INDICATOR;
}

void QTestLibModelTester::checkResultsInternal(const QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult>& results, const QString& testName)
{
    BEGIN_SUB_TEST_FUNCTION

    QDomDocument dom(testName);
    SUB_TEST_FUNCTION(loadTestResult(dom, testName));

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

    END_SUB_TEST_FUNCTION
}

bool QTestLibModelTester::checkIndex(const QModelIndex& index, const QString& testName)
{
    mError = QString::null;

    checkIndexInternal(index, testName);

    return SUB_TEST_FUNCTION_SUCESS_INDICATOR;
}

void QTestLibModelTester::checkIndexInternal(const QModelIndex& index, const QString& testName)
{
    BEGIN_SUB_TEST_FUNCTION

    QDomDocument dom(testName);
    SUB_TEST_FUNCTION(loadTestResult(dom, testName));

    bool isElementOutput = false;
    QDomElement root = dom.documentElement().firstChildElement("root");
    QDomElement rootClass = dom.documentElement().firstChildElement("class");
    QVERIFY2(root.isNull() ^ rootClass.isNull(), "The document element should have either root or class child");
    if (!root.isNull()) {
        SUB_TEST_FUNCTION(isOutput(root, &isElementOutput));
        if (isElementOutput) {
            QVERIFY2(root.nextSiblingElement("root").isNull(), "The document element should have only one root child");
            SUB_TEST_FUNCTION(parseRoot(index, root));
        }
    }
    else if (!rootClass.isNull()) {
        SUB_TEST_FUNCTION(isOutput(rootClass, &isElementOutput));
        if (isElementOutput) {
            QVERIFY2(rootClass.nextSiblingElement("class").isNull(), "The document element should have only one class child");
            SUB_TEST_FUNCTION(parseClass(index, rootClass));
        }
    }

    END_SUB_TEST_FUNCTION
}

void QTestLibModelTester::loadTestResult(QDomDocument& dom, const QString& testName)
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

void QTestLibModelTester::isOutput(const QDomElement& element, bool *ret, bool filter)
{
    BEGIN_SUB_TEST_FUNCTION

    SUB_TEST_FUNCTION(isOutputFormat(element, ret));
    if (*ret)
        SUB_TEST_FUNCTION(isOutputVerbosity(element, ret));
    if (*ret && filter)
        SUB_TEST_FUNCTION(isOutputType(element, ret));

    END_SUB_TEST_FUNCTION
}

void QTestLibModelTester::isOutputFormat(const QDomElement& element, bool *ret)
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

void QTestLibModelTester::isOutputVerbosity(const QDomElement& element, bool *ret)
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

void QTestLibModelTester::isOutputType(const QDomElement& element, bool* ret)
{
    BEGIN_SUB_TEST_FUNCTION

    const QString results = "Signal  "
                            "Duration"
                            "Result  "
                            "QDebug  "
                            "Info    "
                            "Warning "
                            "QWarning"
                            "QSystem "
                            "QFatal  "
                            "Unknown "
                            "Skip    "
                            "Pass    "
                            "BPass   "
                            "XPass   "
                            "XFail   "
                            "BFail   "
                            "Fail    ";

    QString result = element.attribute("type", QString::null);
    QVERIFY2(!result.isNull(), "Elements must have a type attribute");

    int t = -1;
    do {
       t = results.indexOf(result, t + 1);
    } while((t != -1) && (t % 8 != 0));
    QVERIFY2(t != -1, qPrintable(QString("Unknown type \"%1\"").arg(result)));
    *ret = mFilters.at(t / 8);

    END_SUB_TEST_FUNCTION
}

void QTestLibModelTester::checkResults(QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results, const QDomElement& expected)
{
    BEGIN_SUB_TEST_FUNCTION

    QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult>::const_iterator currentResult = results.constBegin();
    QDomElement currentExpected = expected.firstChildElement();

    while ((currentResult != results.constEnd()) && (!currentExpected.isNull())) {
        bool isPrinted = false;
        SUB_TEST_FUNCTION(isOutput(currentExpected, &isPrinted, false));
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

void QTestLibModelTester::parseRoot(const QModelIndex& index, const QDomElement& element)
{
    BEGIN_SUB_TEST_FUNCTION

    qDebug() << mModel->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole);
    QVERIFY2(mModel->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).type() == QVariant::String, "Result string role for root index should be a string");
    QVERIFY2(QString::compare(mModel->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).toString(), element.attribute("type", QString::null), Qt::CaseInsensitive) == 0, "Result string for root do not match");

    QDomElement childElement = element.firstChildElement();
    int i = 0;
    while (!childElement.isNull()) {
        bool isElementOutput = false;
        SUB_TEST_FUNCTION(isOutput(childElement, &isElementOutput));
        if (isElementOutput) {
            if (QString::compare(childElement.tagName(), "class", Qt::CaseInsensitive) == 0) {
                QVERIFY2(mModel->index(i, 0, index).isValid(), qPrintable(QString("Class child %1 of root element is not valid").arg(i)));
                SUB_TEST_FUNCTION(parseClass(mModel->index(i, 0, index), childElement));
                i++;
            }
            if (QString::compare(childElement.tagName(), "message", Qt::CaseInsensitive) == 0) {
                QVERIFY2(mModel->index(i, 0, index).isValid(), qPrintable(QString("Message child %1 of class element is not valid").arg(i)));
                SUB_TEST_FUNCTION(parseMessage(mModel->index(i, 0, index), childElement));
                i++;
            }
        }
        childElement = childElement.nextSiblingElement();
    }

    QVERIFY2(mModel->rowCount(index) == i, qPrintable(QString("The model index should have %1 rows").arg(i)));
    if (i != 0)
        QVERIFY2(mModel->columnCount(index) == 3, "The model index should have 3 columns.");

    END_SUB_TEST_FUNCTION
}

void QTestLibModelTester::parseClass(const QModelIndex& index, const QDomElement& element)
{
    BEGIN_SUB_TEST_FUNCTION

    QVERIFY2(mModel->data(index, Qt::DisplayRole).type() == QVariant::String, "Display role for class index should be a string");
    qDebug() << mModel->data(index, Qt::DisplayRole).toString() << element.attribute("name", QString::null);
    QVERIFY2(QString::compare(mModel->data(index, Qt::DisplayRole).toString(), element.attribute("name", QString::null) , Qt::CaseSensitive) == 0, "Class name do not match");
    QVERIFY2(mModel->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).type() == QVariant::String, "Result string role for class index should be a string");
    QVERIFY2(QString::compare(mModel->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).toString(), element.attribute("type", QString::null) , Qt::CaseInsensitive) == 0, "Result string for class do not match");

    int i = 0;
    QDomElement childElement = element.firstChildElement();
    while (!childElement.isNull()) {
        bool isElementOutput = false;
        SUB_TEST_FUNCTION(isOutput(childElement, &isElementOutput));
        if (isElementOutput) {
            if (QString::compare(childElement.tagName(), "function", Qt::CaseInsensitive) == 0) {
                QVERIFY2(mModel->index(i, 0, index).isValid(), qPrintable(QString("Function child %1 of class element is not valid").arg(i)));
                SUB_TEST_FUNCTION(parseFunction(mModel->index(i, 0, index), childElement));
                i++;
            }
            if (QString::compare(childElement.tagName(), "message", Qt::CaseInsensitive) == 0) {
                QVERIFY2(mModel->index(i, 0, index).isValid(), qPrintable(QString("Message child %1 of class element is not valid").arg(i)));
                SUB_TEST_FUNCTION(parseMessage(mModel->index(i, 0, index), childElement));
                i++;
            }
        }
        childElement = childElement.nextSiblingElement();
    }

    QVERIFY2(mModel->rowCount(index) == i, qPrintable(QString("The model index should have %1 rows").arg(i)));
    QVERIFY2(mModel->columnCount(index) == 3, "The model index should have 3 columns.");

    END_SUB_TEST_FUNCTION
}

void QTestLibModelTester::parseFunction(const QModelIndex& index, const QDomElement& element)
{
    BEGIN_SUB_TEST_FUNCTION

    qDebug() << mModel->data(index, Qt::DisplayRole).toString()
             << element.attribute("name", QString::null);
    QVERIFY2(mModel->data(index, Qt::DisplayRole).type() == QVariant::String, "Display role for function index should be a string");
    QVERIFY2(QString::compare(mModel->data(index, Qt::DisplayRole).toString(), element.attribute("name", QString::null) , Qt::CaseSensitive) == 0, "Function name do not match");
    /*qDebug() << mModel->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).toString()
             << element.attribute("type", QString::null);*/
    QVERIFY2(mModel->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).type() == QVariant::String, "Result string role for function index should be a string");
    QVERIFY2(QString::compare(mModel->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).toString(), element.attribute("type", QString::null) , Qt::CaseInsensitive) == 0, "Result string for function do not match");

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
                QVERIFY2(mModel->index(i, 0, index).isValid(), qPrintable(QString("Row child %1 of function element is not valid").arg(i)));
                SUB_TEST_FUNCTION(parseRow(mModel->index(i, 0, index), childElement));
                i++;
            }
            if (QString::compare(childElement.tagName(), "message", Qt::CaseInsensitive) == 0) {
                QVERIFY2(mModel->index(i, 0, index).isValid(), qPrintable(QString("Message child %1 of function element is not valid").arg(i)));
                SUB_TEST_FUNCTION(parseMessage(mModel->index(i, 0, index), childElement));
                i++;
            }
        }
        childElement = childElement.nextSiblingElement();
    }

    QVERIFY2(mModel->rowCount(index) == i, qPrintable(QString("The model index should have %1 rows").arg(i)));
    QVERIFY2(mModel->columnCount(index) == 3, "The model index should have 3 columns.");

    END_SUB_TEST_FUNCTION
}

void QTestLibModelTester::parseRow(const QModelIndex& index, const QDomElement& element)
{
    BEGIN_SUB_TEST_FUNCTION

    QVERIFY2(mModel->data(index, Qt::DisplayRole).type() == QVariant::String, "Display role for row index should be a string");
    /*qDebug() << mModel->data(index, Qt::DisplayRole).toString()
             << element.attribute("title", QString::null);*/
    QVERIFY2(QString::compare(mModel->data(index, Qt::DisplayRole).toString(), element.attribute("title", QString::null) , Qt::CaseSensitive) == 0, "Row title do not match");
    QVERIFY2(mModel->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).type() == QVariant::String, "Result string role for row index should be a string");
    QVERIFY2(QString::compare(mModel->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).toString(), element.attribute("type", QString::null) , Qt::CaseInsensitive) == 0, "Result string for row do not match");

    QDomElement messageElement = element.firstChildElement("message");
    int i = 0;
    while (!messageElement.isNull()) {
        bool isElementOutput = false;
        SUB_TEST_FUNCTION(isOutput(messageElement, &isElementOutput));
        if (isElementOutput) {
            QVERIFY2(mModel->index(i, 0, index).isValid(), qPrintable(QString("Children %1 of class element is not valid").arg(i)));
            SUB_TEST_FUNCTION(parseMessage(mModel->index(i, 0, index), messageElement));
            i++;
        }
        messageElement = messageElement.nextSiblingElement("message");
    }

    QVERIFY2(mModel->rowCount(index) == i, qPrintable(QString("The model index should have %1 rows").arg(i)));
    QVERIFY2(mModel->columnCount(index) == 3, "The model index should have 3 columns.");

    END_SUB_TEST_FUNCTION
}

void QTestLibModelTester::parseMessage(const QModelIndex& index, const QDomElement& element)
{
    BEGIN_SUB_TEST_FUNCTION

    QVERIFY2(mModel->data(index, Qt::DisplayRole).type() == QVariant::String, "Display role for message index should be a string");
    QVERIFY2(element.firstChild().isText(), "Child node should be a text node.");
    QVERIFY2(element.firstChild().nextSibling().isNull(), "The element should have only one child");
    qDebug() << mModel->data(index, Qt::DisplayRole).toString()
             << element.firstChild().toText().data().trimmed();
    if (QString::compare(element.attribute("strict", "false"), "true", Qt::CaseInsensitive) == 0)
        QVERIFY2(QString::compare(mModel->data(index, Qt::DisplayRole).toString(), element.firstChild().toText().data().trimmed() , Qt::CaseSensitive) == 0, "Message text do not match");
    QVERIFY2(mModel->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).type() == QVariant::String, "Result string role for message index should be a string");
    /*qDebug() << mModel->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).toString()
             << element.attribute("type", QString::null);*/
    QVERIFY2(QString::compare(mModel->data(index, QTestLibPlugin::Internal::QTestLibModel::ResultStringRole).toString(), element.attribute("type", QString::null) , Qt::CaseInsensitive) == 0, "Result string for message do not match");

    QVERIFY2(mModel->rowCount(index) == 0, "The model index should have 0 rows");
    QVERIFY2(mModel->columnCount(index) == 0, "The model index should have 0 columns.");

    SUB_TEST_FUNCTION(checkLocation(index, element));

    END_SUB_TEST_FUNCTION
}


void QTestLibModelTester::checkLocation(const QModelIndex& index, const QDomElement& element)
{
    BEGIN_SUB_TEST_FUNCTION

    QVERIFY2(element.hasAttribute("file") == element.hasAttribute("line"), "Elements should either have a file and a line or none");

    if (element.hasAttribute("file") && element.hasAttribute("line")) {
        QString file = element.attribute("file");
        bool ok = false;
        unsigned int line = element.attribute("line", "0").toUInt(&ok, 10);

        QVERIFY2(ok, qPrintable(QString("Bad line number %1").arg(element.attribute("line", "0"))));
        QVERIFY2(mModel->sibling(index.row(), 1, index).isValid(), "There should be an item in column 2");
        QVERIFY2(mModel->sibling(index.row(), 1, index).data(Qt::DisplayRole).type() == QVariant::String, "Item in column 2 should be a string (DisplayRole)");
        /*qDebug() << mModel->sibling(index.row(), 1, index).data(Qt::DisplayRole).toString() << file;*/
        QVERIFY2(QString::compare(mModel->sibling(index.row(), 1, index).data(Qt::DisplayRole).toString(), file, Qt::CaseSensitive) ==0, "File does not match (DisplayRole)");
        QVERIFY2(mModel->sibling(index.row(), 1, index).data(Qt::ToolTipRole).type() == QVariant::String, "Item in column 2 should be a string (ToolTipRole)");
        /*qDebug() << mModel->sibling(index.row(), 1, index).data(Qt::ToolTipRole).toString() << file;*/
        QVERIFY2(QString::compare(mModel->sibling(index.row(), 1, index).data(Qt::ToolTipRole).toString(), file, Qt::CaseSensitive) ==0, "File does not match (ToolTipRole)");

        QVERIFY2(mModel->sibling(index.row(), 2, index).isValid(), "There should be an item in column 3");
        QVERIFY2(mModel->sibling(index.row(), 2, index).data(Qt::DisplayRole).type() == QVariant::UInt, "Item in column 3 should be an unsigned integer (DisplayRole)");
        /*qDebug() << mModel->sibling(index.row(), 2, index).data(Qt::DisplayRole).toUInt() << line;*/
        QVERIFY2(mModel->sibling(index.row(), 2, index).data(Qt::DisplayRole).toUInt() == line, "Line does not match (DisplayRole)");
        QVERIFY2(mModel->sibling(index.row(), 2, index).data(Qt::ToolTipRole).type() == QVariant::UInt, "Item in column 2 should be an unsigned integer (ToolTipRole)");
        /*qDebug() << mModel->sibling(index.row(), 2, index).data(Qt::ToolTipRole).toUInt() << line;*/
        QVERIFY2(mModel->sibling(index.row(), 2, index).data(Qt::ToolTipRole).toUInt() == line, "Line does not match (ToolTipRole)");
    } else {
        QVERIFY2(mModel->sibling(index.row(), 1, index).isValid(), "There should be an item in column 2");
        QVERIFY2(!mModel->sibling(index.row(), 1, index).data(Qt::DisplayRole).isValid(), "Item in column 2 should be empty (DisplayRole)");
        QVERIFY2(!mModel->sibling(index.row(), 1, index).data(Qt::ToolTipRole).isValid(), "Item in column 2 should be empty (ToolTipRole)");
        QVERIFY2(mModel->sibling(index.row(), 2, index).isValid(), "There should be an item in column 3");
        QVERIFY2(!mModel->sibling(index.row(), 2, index).data(Qt::DisplayRole).isValid(), "Item in column 3 should be empty (DisplayRole)");
        QVERIFY2(!mModel->sibling(index.row(), 2, index).data(Qt::ToolTipRole).isValid(), "Item in column 2 should be empty (ToolTipRole)");
    }

    END_SUB_TEST_FUNCTION
}
