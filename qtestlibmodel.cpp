#include "qtestlibmodel.h"
#include "plaintextqtestlibparser.h"

#include <projectexplorer/runconfiguration.h>

#include <QtGui>
#include <QtDebug>

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
    Q_ASSERT(runControl == mTestRun);

    //qDebug() << type << className << functionName << rowTitle << message;

    TestItem *testItem;
    TestClassItem *testClassItem;
    TestCaseItem *testCaseItem;
    TestRowItem *testRowItem;

    //qDebug() << messageType << type << ":" << className << "::" << functionName << "(\"" << rowTitle << "\"):" << message;

    if (className.isEmpty()) {
        if (mRoot == NULL) {
            mRoot = new TestRootItem();
        } else if (mRoot->type() != TestRoot) {
            TestItem *oldRoot = mRoot;
            mRoot = new TestRootItem();
            mRoot->appendChild(oldRoot);
        }
        mCurrentMessageItem = createTestMessageItem(type, message, mRoot);
        return;
    }

    // Find the class item
    if (mRoot == NULL) {
        testClassItem = new TestClassItem(className);
        mRoot = testClassItem;
    } else if (mRoot->type() == TestClass) {
        if (mRoot->compareName(className) == 0) {
            testClassItem = dynamic_cast<TestClassItem *>(mRoot);
        } else {
            TestItem *oldRoot = mRoot;
            mRoot = new TestRootItem();
            mRoot->appendChild(oldRoot);
            testClassItem = new TestClassItem(className, mRoot);
        }
    } else {
        testItem = mRoot->findChild(className);
        if (testItem == NULL)
            // Create a new class item
            testClassItem = new TestClassItem(className, mRoot);
        else
            // Cast existing class item
            testClassItem = dynamic_cast<TestClassItem *>(testItem);
    }

    if (functionName.isEmpty()) {
        mCurrentMessageItem = createTestMessageItem(type, message, testClassItem);
        return;
    }

    // Find the case item
    testItem = testClassItem->findChild(functionName);
    if (testItem == NULL)
        // Create a new case item
        testCaseItem = new TestCaseItem(functionName, testClassItem);
    else
        // Cast existing case item
        testCaseItem = dynamic_cast<TestCaseItem *>(testItem);

    if (rowTitle.isEmpty()) {
        mCurrentMessageItem = createTestMessageItem(type, message, testCaseItem);
        return;
    }

    // Find the row item
    testItem = testCaseItem->findChild(rowTitle);
    if (testItem == NULL)
        // Create a new row item
        testRowItem = new TestRowItem(rowTitle, testCaseItem);
    else
        // Cast existing case item
        testRowItem = dynamic_cast<TestRowItem *>(testItem);

    mCurrentMessageItem = createTestMessageItem(type, message, testRowItem);
}


QTestLibModel::TestMessageItem* QTestLibModel::createTestMessageItem(MessageType type, const QString& message, TestItem* parent)
{
    if (message.startsWith(QLatin1String("Signal: ")))
        return new TestMessageItem(Signal, message.mid(8).trimmed(), parent);
    return new TestMessageItem(type, message, parent);
}

void QTestLibModel::appendTestItemMessage(ProjectExplorer::RunControl* runControl, const QString& message)
{
    Q_ASSERT(runControl == mTestRun);

    if (mCurrentMessageItem != NULL)
        mCurrentMessageItem->appendMessage(message);
}

void QTestLibModel::appendTestLocation(ProjectExplorer::RunControl* runControl, const QString& file, unsigned int line)
{
    Q_ASSERT(runControl == mTestRun);

    if (mCurrentMessageItem != NULL) {
        mCurrentMessageItem->setFile(file);
        mCurrentMessageItem->setLine(line);
    }
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
    if (mRoot == NULL)
        return QVariant();
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

QIcon messageIcon(QTestLibModel::MessageType type)
{
    switch(type) {
    case QTestLibModel::Unknown:
        return QIcon(QLatin1String(":/messages/unknown.png"));
    case QTestLibModel::Warning:
        return QIcon(QLatin1String(":/messages/warning.png"));
    case QTestLibModel::QWarning:
        return QIcon(QLatin1String(":/messages/warning.png"));
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
    }
    return QString::null;
}

QString resultString(QTestLibModel::MessageType type)
{
    QString str = QLatin1String("Signal  "
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
    return str.mid((int) type * 8, 8).trimmed();
}

QTestLibModel::TestItem::TestItem(TestItem *parent) :
    mResult(QTestLibModel::Unknown), mChildrenCount(0), mParent(NULL), mFile(QString::null), mLine(0)
{
    if (parent != NULL)
        parent->appendChild(this);
}

QTestLibModel::TestMessageItem::TestMessageItem(MessageType type, const QString& msg, TestItem *parent) :
    TestItem(parent), mMessage(msg)
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
        return QVariant(messageIcon(mResult));
    if ((column == 0) && (role == ResultRole))
        return QVariant::fromValue<MessageType>(mResult);
    if ((column == 0) && (role == ResultStringRole))
        return QVariant(resultString(mResult));
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
        return QVariant(messageIcon(mResult));
    if ((column == 0) && ((role == Qt::DisplayRole) || (role == Qt::ToolTipRole)))
        return mMessage.isEmpty() ? QVariant(defaultMessage(mResult)) : QVariant(mMessage.trimmed());
    return TestItem::data(column, role);
}


} // namespace Internal
} // namespace QTestLibPlugin

