#include "testsuitemodel.h"
#include "testmodelfactory.h"

#include <projectexplorer/runconfiguration.h>

#include <QtDebug>

namespace QTestLibPlugin {
namespace Internal {

TestSuiteModel::TestSuiteModel(QObject *parent) :
    QAbstractItemModel(parent)
{

}

int TestSuiteModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
        return mTests.size();

    QAbstractItemModel *subModel = mInternalPointers.value(parent.internalPointer(), NULL);
    Q_ASSERT(subModel != NULL);

    if (subModel == parent.internalPointer())
        return subModel->rowCount(QModelIndex());
    return subModel->rowCount(parent);
}

int TestSuiteModel::columnCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
        return 1;

    QAbstractItemModel *subModel = mInternalPointers.value(parent.internalPointer(), NULL);
    Q_ASSERT(subModel != NULL);

    if (subModel == parent.internalPointer())
        return subModel->columnCount(QModelIndex());
    return subModel->columnCount(parent);
}

QModelIndex TestSuiteModel::index(int row, int column, const QModelIndex& parent) const
{
    void *internalPointer = NULL;
    QAbstractItemModel *subModel = NULL;

    if (!parent.isValid()) {
        subModel = mTests.at(row);
        internalPointer = mTests.at(row);
    } else {
        subModel =  mInternalPointers.value(parent.internalPointer(), NULL);
        Q_ASSERT(subModel != NULL);
        if (parent.internalPointer() == subModel)
            internalPointer = subModel->index(row, column, QModelIndex()).internalPointer();
        else
            internalPointer = subModel->index(row, column, parent).internalPointer();
    }

    Q_ASSERT(!mInternalPointers.contains(internalPointer) || (mInternalPointers.value(internalPointer, NULL) == subModel));
    mInternalPointers.insert(internalPointer, subModel);
    return createIndex(row, column, internalPointer);
}

QModelIndex TestSuiteModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return QModelIndex();

    QAbstractItemModel *subModel = mInternalPointers.value(child.internalPointer(), NULL);
    Q_ASSERT(subModel != NULL);

    if (subModel == child.internalPointer())
        return QModelIndex();

    QModelIndex parentIndex = subModel->parent(child);
    int row = mTests.indexOf(subModel);

    if (!parentIndex.isValid())
        return index(row, child.column(), QModelIndex());

    if (subModel->parent(parentIndex).isValid())
        return index(parentIndex.row(), parentIndex.column(), subModel->parent(parentIndex));
    else
        return index(parentIndex.row(), parentIndex.column(), index(row, 0, QModelIndex()));
}

QVariant TestSuiteModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    QAbstractItemModel *subModel = mInternalPointers.value(index.internalPointer(), NULL);
    Q_ASSERT(subModel != NULL);

    if (subModel != index.internalPointer())
        return subModel->data(index, role);

    return QVariant(); // TODO change this adequately.
}

void TestSuiteModel::appendTestRun(ProjectExplorer::RunControl* runControl)
{
    TestModelFactory *factory = new TestModelFactory(runControl, this);
    connect(factory, SIGNAL(modelPopulated(QAbstractItemModel *)),
            this, SLOT(endAppendTestRun(QAbstractItemModel *)));
}

void TestSuiteModel::endAppendTestRun(QAbstractItemModel *testModel)
{
    beginInsertRows(QModelIndex(), mTests.size(), mTests.size());
    Q_ASSERT(testModel->parent() == NULL);
    testModel->setParent(this);
    mTests.append(testModel);
    endInsertRows();
}

void TestSuiteModel::removeTestRun(int index)
{
    if ((index < 0) || (index >= mTests.size()))
        return;

    QAbstractItemModel *testRun = mTests.takeAt(index);
    delete testRun;

    QMap<void*, QAbstractItemModel*>::iterator it = mInternalPointers.begin();
    while (it != mInternalPointers.end()) {
        if (it.value() == testRun)
            it = mInternalPointers.erase(it);
        else
            it++;
    }
}

void TestSuiteModel::clear(void)
{
    beginResetModel();
    foreach (QAbstractItemModel* testModel, mTests) {
        delete testModel;
    }

    mTests.clear();
    mInternalPointers.clear();
    endResetModel();
}


} // namespace Internal
} // namespace QTestLibPlugin
