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

#include "qtestlibmodel.h"
#include "plaintextqtestlibparser.h"

#include <projectexplorer/runconfiguration.h>

#include <QtGui>
#include <QtDebug>

#define GOOBLE(x)

namespace QTestLibPlugin {
namespace Internal {

QTestLibModel::QTestLibModel(ProjectExplorer::RunControl* runControl, QObject *parent) :
    QAbstractItemModel(parent), mTestRun(runControl), mRoot(NULL), mCurrentMessageItem(NULL)
{
    //mRoot = new TestRootItem();
}

QTestLibModel::~QTestLibModel(void)
{
    qDebug() << "Delete: " << this;
    if (mRoot)
        delete mRoot;
}

void QTestLibModel::addTestItem(ProjectExplorer::RunControl* runControl, MessageType type, const QString& className,
                                const QString& functionName, const QString& rowTitle, const QString& message)
{
#ifndef QT_DEBUG
    Q_UNUSED(runControl)
#endif
    Q_ASSERT(runControl == mTestRun);

    TestItem *testItem;
    TestClassItem *testClassItem;
    TestCaseItem *testCaseItem;
    TestRowItem *testRowItem;

    //qDebug() << type << ":" << className << "::" << functionName << "(\"" << rowTitle << "\"):" << message;

    if (className.isEmpty()) {
        if (mRoot == NULL) {
            mRoot = new TestRootItem(this);
        } else if (mRoot->type() != TestRoot) {
            TestItem *oldRoot = mRoot;
            mRoot = new TestRootItem(this);
            mRoot->replace(oldRoot);
            emit dataChanged(index(mRoot, 0), index(mRoot, 2));

            beginInsertRows(QModelIndex(), mRoot->childrenCount(), mRoot->childrenCount());
            mRoot->appendChild(oldRoot);
            endInsertRows();

            beginMoveRows(QModelIndex(), 0, mRoot->childrenCount() - 2, createIndex(mRoot->childrenCount(), 0, oldRoot), 0);
            oldRoot->replace(mRoot, 0, -2);
            endMoveRows();
        }
        createTestMessageItem(type, message, mRoot);
        return;
    }

    // Find the class item
    if (mRoot == NULL) {
        testClassItem = new TestClassItem(className, this);
        mRoot = testClassItem;
        emit dataChanged(index(mRoot, 0), index(mRoot, 2));
    } else if (mRoot->type() == TestClass) {
        if (mRoot->compareName(className) == 0) {
            testClassItem = dynamic_cast<TestClassItem *>(mRoot);
        } else {
            TestItem *oldRoot = mRoot;
            mRoot = new TestRootItem(this);
            mRoot->replace(oldRoot);
            emit dataChanged(index(mRoot, 0), index(mRoot, 2));

            beginInsertRows(QModelIndex(), mRoot->childrenCount(), mRoot->childrenCount());
            mRoot->appendChild(oldRoot);
            endInsertRows();

            beginMoveRows(QModelIndex(), 0, mRoot->childrenCount() - 2, createIndex(mRoot->childrenCount(), 0, oldRoot), 0);
            oldRoot->replace(mRoot, 0, -2);
            endMoveRows();

            beginInsertRows(QModelIndex(), 1, 1);
            testClassItem = new TestClassItem(className, this, mRoot);
            endInsertRows();
        }
    } else {
        testItem = mRoot->findChild(className);
        if (testItem == NULL) {
            // Create a new class item
            beginInsertRows(QModelIndex(), mRoot->childrenCount(), mRoot->childrenCount());
            testClassItem = new TestClassItem(className, this, mRoot);
            endInsertRows();
        } else {
            // Cast existing class item
            testClassItem = dynamic_cast<TestClassItem *>(testItem);
        }
    }

    if (functionName.isEmpty()) {
        createTestMessageItem(type, message, testClassItem);
        return;
    }

    // Find the case item
    testItem = testClassItem->findChild(functionName);
    if (testItem == NULL) {
        // Create a new case item
        beginInsertRows(index(testClassItem), testClassItem->childrenCount(), testClassItem->childrenCount());
        testCaseItem = new TestCaseItem(functionName, this, testClassItem);
        endInsertRows();
    } else {
        // Cast existing case item
        testCaseItem = dynamic_cast<TestCaseItem *>(testItem);
    }

    if (rowTitle.isEmpty()) {
        createTestMessageItem(type, message, testCaseItem);
        return;
    }

    // Find the row item
    testItem = testCaseItem->findChild(rowTitle);
    if (testItem == NULL) {
        // Create a new row item
        beginInsertRows(index(testCaseItem), testCaseItem->childrenCount(), testCaseItem->childrenCount());
        testRowItem = new TestRowItem(rowTitle, this, testCaseItem);
        endInsertRows();
    } else {
        // Cast existing case item
        testRowItem = dynamic_cast<TestRowItem *>(testItem);
    }

    createTestMessageItem(type, message, testRowItem);
}


void QTestLibModel::createTestMessageItem(MessageType type, const QString& message, TestItem* parent)
{
    QString messageTrimmed = message.trimmed();

    beginInsertRows(index(parent), parent->childrenCount(), parent->childrenCount());
    if (messageTrimmed.startsWith(QLatin1String("Signal: ")))
        mCurrentMessageItem = new TestMessageItem(Signal, messageTrimmed.mid(8), this, parent);
    else if (messageTrimmed.startsWith(QLatin1String("Slot: ")))
        mCurrentMessageItem = new TestMessageItem(Slot, messageTrimmed.mid(6).trimmed(), this, parent);
    else
        mCurrentMessageItem = new TestMessageItem(type, message, this, parent);
    endInsertRows();
}

void QTestLibModel::appendTestItemMessage(ProjectExplorer::RunControl* runControl, const QString& message)
{
#ifndef QT_DEBUG
    Q_UNUSED(runControl);
#endif
    Q_ASSERT(runControl == mTestRun);

    if (mCurrentMessageItem != NULL) {
        mCurrentMessageItem->appendMessage(message);
        emit dataChanged(index(mCurrentMessageItem, 0), index(mCurrentMessageItem, 0));
    }
}

void QTestLibModel::appendTestLocation(ProjectExplorer::RunControl* runControl, const QString& file, unsigned int line)
{
#ifndef QT_DEBUG
    Q_UNUSED(runControl);
#endif
    Q_ASSERT(runControl == mTestRun);

    if (mCurrentMessageItem != NULL) {
        mCurrentMessageItem->setFile(file);
        mCurrentMessageItem->setLine(line);
        emit dataChanged(index(mCurrentMessageItem, 1), index(mCurrentMessageItem, 2));
    }
}

bool QTestLibModel::renameClass(const QString& oldName, const QString& newName)
{
    if (mRoot == NULL)
        return false;

    TestClassItem *classItem;

    switch (mRoot->type()) {
    case TestRoot:
        for (int c = 0; c < mRoot->childrenCount(); c++) {
            if ((mRoot->child(c)->type() != TestClass) || (mRoot->child(c)->compareName(oldName) != 0))
                continue;
            classItem = dynamic_cast<TestClassItem *>(mRoot->child(c));
            Q_ASSERT(classItem != NULL);
            classItem->mClassName = newName;
            return true;
        }
        break;
    case TestClass:
        if (mRoot->compareName(oldName) != 0)
            break;
        classItem = dynamic_cast<TestClassItem *>(mRoot);
        Q_ASSERT(classItem != NULL);
        classItem->mClassName = newName;
        return true;
    default:
        Q_ASSERT_X(false, "Unreachable place", "Root should either be of type TestRoot of TestClass");
        break;
    }

    return false;
}

int QTestLibModel::rowCount(const QModelIndex& parent) const
{
    TestItem *testItem = mRoot;
    if (parent.isValid())
        testItem = static_cast<TestItem *>(parent.internalPointer());

    if (testItem == NULL)
        return 0;
    return testItem->childrenCount();
}

int QTestLibModel::columnCount(const QModelIndex& parent) const
{
    TestItem *testItem = mRoot;
    if (parent.isValid())
        testItem = static_cast<TestItem *>(parent.internalPointer());

    if (testItem == NULL)
        return 0;
    return testItem->columnCount();
}

QModelIndex QTestLibModel::index(TestItem *testItem, int column) const
{
    if (testItem == mRoot)
        return QModelIndex();

    TestItem *parentItem = testItem->parent();

    return createIndex(parentItem->findChid(testItem), column, testItem);
}


QModelIndex QTestLibModel::index(int row, int column, const QModelIndex& parent) const
{
    TestItem *testItem = mRoot;
    if (parent.isValid())
        testItem = static_cast<TestItem *>(parent.internalPointer());

    Q_ASSERT(testItem != NULL);
    if (testItem != NULL)
        return createIndex(row, column, testItem->child(row));

    return QModelIndex();
}

QModelIndex QTestLibModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return QModelIndex();

    TestItem *testItem = static_cast<TestItem *>(child.internalPointer());
    if ((testItem == NULL) || (testItem->parent() == NULL) || (testItem->parent()->parent() == NULL))
        return QModelIndex();
    return createIndex(testItem->parent()->parent()->findChid(testItem->parent()), 0, testItem->parent());
}

QVariant QTestLibModel::data(const QModelIndex &index, int role) const
{
    if (mRoot == NULL) {
        if (!index.isValid()) {
            if (role ==  Qt::DecorationRole)
                return QVariant(QIcon(QLatin1String(":/messages/unknown.png")));
            if (role == ResultRole)
                return QVariant::fromValue<MessageType>(Unknown);
            if (role == ResultStringRole)
                return QVariant(QLatin1String("Unknown"));
        }
        return QVariant();
    }
    if (!index.isValid())
        return mRoot->data(0, role);

    TestItem *testItem = static_cast<TestItem *>(index.internalPointer());

    Q_ASSERT(testItem != NULL);
    if (testItem != NULL) {
        switch (index.column()) {
        case 0:
            return testItem->data(index.column(), role);
        case 1:
            if (testItem->isLocated() && ((role == Qt::DisplayRole) || (role == Qt::ToolTipRole)))
                return testItem->file();
            else
                return QVariant();
        case 2:
            if (testItem->isLocated() && ((role == Qt::DisplayRole) || (role == Qt::ToolTipRole)))
                return testItem->line();
            else
                return QVariant();
        default:
            Q_ASSERT(false); // NOTE the column count is three, so the column number is less or equal to 2.
            break;
        }
    }
    return QVariant();
}

QIcon QTestLibModel::messageIcon(QTestLibModel::MessageType type)
{
    switch(type) {
    case QTestLibModel::Unknown:
        return QIcon(QLatin1String(":/messages/unknown.png"));
    case QTestLibModel::Warning:
        return QIcon(QLatin1String(":/messages/warning.png"));
    case QTestLibModel::QWarning:
        return QIcon(QLatin1String(":/messages/qwarning.png"));
    case QTestLibModel::QDebug:
        return QIcon(QLatin1String(":/messages/debug.png"));
    case QTestLibModel::QSystem:
        return QIcon(QLatin1String(":/messages/system.png"));
    case QTestLibModel::QFatal:
        return QIcon(QLatin1String(":/messages/fatal.png"));
    case QTestLibModel::Info:
        return QIcon(QLatin1String(":/messages/info.png"));
    case QTestLibModel::Skip:
        return QIcon(QLatin1String(":/events/skip.png"));
    case QTestLibModel::Pass:
        return QIcon(QLatin1String(":/events/pass.png"));
    case QTestLibModel::XPass:
        return QIcon(QLatin1String(":/events/xpass.png"));
    case QTestLibModel::BlackListedPass:
        return QIcon(QLatin1String(":/events/bpass.png"));
    case QTestLibModel::Fail:
        return QIcon(QLatin1String(":/events/fail.png"));
    case QTestLibModel::XFail:
        return QIcon(QLatin1String(":/events/xfail.png"));
    case QTestLibModel::BlackListedFail:
        return QIcon(QLatin1String(":/events/bfail.png"));
    case QTestLibModel::BenchmarkResult:
        return QIcon(QLatin1String(":/events/result.png"));
    case QTestLibModel::Duration:
        return QIcon(QLatin1String(":/messages/duration.png"));
    case QTestLibModel::Signal:
        return QIcon(QLatin1String(":/messages/signal.png"));
    case QTestLibModel::Slot:
        return QIcon(QLatin1String(":/messages/slot.png"));
    default:
        qWarning() << "Sentinel value used in" << __func__;
    }
    return QIcon();
}

QString defaultMessage(QTestLibModel::MessageType type)
{
    switch(type) {
    case QTestLibModel::Unknown:
    case QTestLibModel::Warning:
    case QTestLibModel::QWarning:
    case QTestLibModel::QDebug:
    case QTestLibModel::QSystem:
    case QTestLibModel::QFatal:
    case QTestLibModel::Info:
    case QTestLibModel::BenchmarkResult:
    case QTestLibModel::Duration:
    case QTestLibModel::Signal:
    case QTestLibModel::Slot:
        break;
    case QTestLibModel::Skip:
        return QTestLibModel::trUtf8("Test skipped");
    case QTestLibModel::Pass:
        return QTestLibModel::trUtf8("Test passed");
    case QTestLibModel::XPass:
        return QTestLibModel::trUtf8("Test passed unexpectedly");
    case QTestLibModel::BlackListedPass:
        return QTestLibModel::trUtf8("Blacklisted test passed");
    case QTestLibModel::Fail:
        return QTestLibModel::trUtf8("Test failed");
    case QTestLibModel::XFail:
        return QTestLibModel::trUtf8("Test failed unexpectedly");
    case QTestLibModel::BlackListedFail:
        return QTestLibModel::trUtf8("Blacklisted test failed");
    default:
        qWarning() << "Sentinel value used in" << __func__;
    }
    return QString::null;
}

QString QTestLibModel::resultString(QTestLibModel::MessageType type)
{
    QString str = QLatin1String("Slot    "
                                "Signal  "
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
                                "Fail    ");

    if ((type != FirstMessageType) && (type != LastMessageType))
        return str.mid((int) type * 8, 8).trimmed();

    qWarning() << "Sentinel value used in" << __func__;
    return QString::null;
}

QString QTestLibModel::resultStringTr(QTestLibModel::MessageType type)
{
    GOOBLE(QT_TR_NOOP("Slot"));
    GOOBLE(QT_TR_NOOP("Signal"));
    GOOBLE(QT_TR_NOOP("Duration"));
    GOOBLE(QT_TR_NOOP("Result"));
    GOOBLE(QT_TR_NOOP("QDebug"));
    GOOBLE(QT_TR_NOOP("Info"));
    GOOBLE(QT_TR_NOOP("Warning"));
    GOOBLE(QT_TR_NOOP("QWarning"));
    GOOBLE(QT_TR_NOOP("QSystem"));
    GOOBLE(QT_TR_NOOP("QFatal"));
    GOOBLE(QT_TR_NOOP("Unknown"));
    GOOBLE(QT_TR_NOOP("Skip"));
    GOOBLE(QT_TR_NOOP("Pass"));
    GOOBLE(QT_TR_NOOP("BPass"));
    GOOBLE(QT_TR_NOOP("XPass"));
    GOOBLE(QT_TR_NOOP("XFail"));
    GOOBLE(QT_TR_NOOP("BFail"));
    GOOBLE(QT_TR_NOOP("Fail"));

    return tr(qPrintable(QTestLibModel::resultString(type)));
}

QTestLibModel::TestItem::TestItem(QTestLibModel* model, TestItem *parent) :
    mResult(QTestLibModel::Unknown), mChildrenCount(0), mParent(NULL), mModel(model), mFile(QString::null), mLine(0)
{
    if (parent != NULL)
        parent->appendChild(this);
}

QTestLibModel::TestMessageItem::TestMessageItem(MessageType type, const QString& msg, QTestLibModel* model, TestItem *parent) :
    TestItem(model, parent), mMessage(msg)
{
    mResult = type;
    if (this->parent() != NULL)
        this->parent()->updateResult(mResult);
}

void QTestLibModel::TestItem::appendChild(TestItem *item)
{
    if (item->mParent != NULL)
        return;

    mChildren.append(item);
    item->mParent = this;
    mChildrenCount++;
}

void QTestLibModel::TestItem::replace(TestItem *item, int first, int last)
{
    QLinkedList<TestItem*>::iterator firstIt = item->mChildren.begin();
    QLinkedList<TestItem*>::iterator lastIt = (last < 0) ? item->mChildren.end() : item->mChildren.begin();

    while (first-- > 0)
        firstIt++;
    while (last-- > 0)
        lastIt++;
    last++;
    while (++last < 0)
        lastIt--;

    while (firstIt != lastIt) {
        TestItem *child = *firstIt;
        firstIt = item->mChildren.erase(firstIt);
        item->mChildrenCount--;

        mChildren.append(child);
        child->mParent = this;
        mChildrenCount++;
    }

    updateResult(item->mResult);
}

void QTestLibModel::TestItem::updateResult(MessageType result)
{
    MessageType oldResult = mResult;
    mResult = (MessageType) qMax((int) Unknown, qMax((int) mResult, (int) result));
    if ((mResult != oldResult) && (mParent != NULL))
        mParent->updateResult(result);
}

QTestLibModel::TestItem* QTestLibModel::TestItem::findChild(const QString& name) const
{
    QLinkedList<TestItem *>::const_iterator it = mChildren.end();

    if (mChildren.isEmpty())
        return NULL;

    do {
        it--;
        if ((*it)->compareName(name) == 0)
                return *it;
    } while(it != mChildren.begin());

    return NULL;
}

int QTestLibModel::TestItem::findChid(TestItem *item) const
{
    int c = 0;
    foreach (TestItem *child, mChildren) {
        if (item == child)
            return c;
        c++;
    }

    return -1;
}

QTestLibModel::TestItem* QTestLibModel::TestItem::child(int row) const
{
    QLinkedList<TestItem *>::const_iterator it = mChildren.begin();

    while ((it != mChildren.end()) && row--)
        it++;

    if (it == mChildren.end())
        return NULL;
    return *it;
}

bool QTestLibModel::TestItem::removeChild(TestItem *item)
{
    int removed = mChildren.removeAll(item);

    if (removed == 0)
        return false;

    delete item;
    mChildrenCount-=removed;
    return true;
}

void QTestLibModel::TestItem::removeChildren(void)
{
    foreach (TestItem *child, mChildren)
        delete child;
    mChildren.clear();
    mChildrenCount = 0;
}

QVariant QTestLibModel::TestItem::data(int column, int role) const
{
    if ((column == 0) && (role ==  Qt::DecorationRole))
        return QVariant(QTestLibModel::messageIcon(mResult));
    if ((column == 0) && (role == ResultRole))
        return QVariant::fromValue<MessageType>(mResult);
    if ((column == 0) && (role == ResultStringRole))
        return QVariant(QTestLibModel::resultString(mResult));
    return QVariant();
}

QVariant QTestLibModel::TestClassItem::data(int column, int role) const
{
    if ((column == 0) && ((role == Qt::DisplayRole) || (role == Qt::ToolTipRole)))
        return QVariant(mClassName);
    return TestItem::data(column, role);
}

QVariant QTestLibModel::TestCaseItem::data(int column, int role) const
{
    if ((column == 0) && ((role == Qt::DisplayRole) || (role == Qt::ToolTipRole)))
        return QVariant(mFunctionName);
    return TestItem::data(column, role);
}

QVariant QTestLibModel::TestRowItem::data(int column, int role) const
{
    if ((column == 0) && ((role == Qt::DisplayRole) || (role == Qt::ToolTipRole)))
        return QVariant(mTitle);
    return TestItem::data(column, role);
}

QVariant QTestLibModel::TestMessageItem::data(int column, int role) const
{
    if ((column == 0) && (role ==  Qt::DecorationRole))
        return QVariant(QTestLibModel::messageIcon(mResult));
    if ((column == 0) && ((role == Qt::DisplayRole) || (role == Qt::ToolTipRole)))
        return mMessage.isEmpty() ? QVariant(defaultMessage(mResult)) : QVariant(mMessage.trimmed());
    return TestItem::data(column, role);
}


} // namespace Internal
} // namespace QTestLibPlugin

