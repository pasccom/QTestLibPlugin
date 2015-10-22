#include "testsuitemodel.h"
#include "testmodelfactory.h"

#include <projectexplorer/runconfiguration.h>

#include <QtDebug>

namespace QTestLibPlugin {
namespace Internal {

int AggregateItemModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
        return mSubModels.size();

    QAbstractItemModel *subModel = mInternalPointers.value(parent.internalPointer(), NULL);
    Q_ASSERT(subModel != NULL);

    if (subModel == parent.internalPointer())
        return subModel->rowCount(QModelIndex());
    return subModel->rowCount(parent);
}

int AggregateItemModel::columnCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
        return 3;

    QAbstractItemModel *subModel = mInternalPointers.value(parent.internalPointer(), NULL);
    Q_ASSERT(subModel != NULL);

    if (subModel == parent.internalPointer())
        return subModel->columnCount(QModelIndex());
    return subModel->columnCount(parent);
}

QModelIndex AggregateItemModel::index(int row, int column, const QModelIndex& parent) const
{
    void *internalPointer = NULL;
    QAbstractItemModel *subModel = NULL;

    if (!parent.isValid()) {
        subModel = mSubModels.at(row);
        internalPointer = mSubModels.at(row);
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

QModelIndex AggregateItemModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return QModelIndex();

    QAbstractItemModel *subModel = mInternalPointers.value(child.internalPointer(), NULL);
    Q_ASSERT(subModel != NULL);

    if (subModel == child.internalPointer())
        return QModelIndex();

    QModelIndex parentIndex = subModel->parent(child);
    int row = mSubModels.indexOf(subModel);

    if (!parentIndex.isValid())
        return index(row, child.column(), QModelIndex());

    if (subModel->parent(parentIndex).isValid())
        return index(parentIndex.row(), parentIndex.column(), subModel->parent(parentIndex));
    else
        return index(parentIndex.row(), parentIndex.column(), index(row, 0, QModelIndex()));
}

QVariant AggregateItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    QAbstractItemModel *subModel = mInternalPointers.value(index.internalPointer(), NULL);
    Q_ASSERT(subModel != NULL);

    if (subModel != index.internalPointer())
        return subModel->data(index, role);

    if (index.column() == 0)
        return subModel->data(QModelIndex(), role);

    return QVariant();
}

void AggregateItemModel::appendSubModel(QAbstractItemModel *model)
{
    beginInsertRows(QModelIndex(), mSubModels.size(), mSubModels.size());
    Q_ASSERT(model->parent() == NULL);
    model->setParent(this);
    mSubModels.append(model);
    endInsertRows();
}

void AggregateItemModel::removeSubModel(int index)
{
    if ((index < 0) || (index >= mSubModels.size()))
        return;

    beginRemoveRows(QModelIndex(), index, index);

    QAbstractItemModel *testRun = mSubModels.takeAt(index);
    delete testRun;

    endRemoveRows();

    QMap<void*, QAbstractItemModel*>::iterator it = mInternalPointers.begin();
    while (it != mInternalPointers.end()) {
        if (it.value() == testRun)
            it = mInternalPointers.erase(it);
        else
            it++;
    }
}

void AggregateItemModel::clear(void)
{
    beginResetModel();
    foreach (QAbstractItemModel* model, mSubModels) {
        delete model;
    }

    mSubModels.clear();
    mInternalPointers.clear();
    endResetModel();
}

void TestSuiteModel::appendTestRun(ProjectExplorer::RunControl* runControl)
{
    TestModelFactory *factory = new TestModelFactory(runControl, this);
    connect(factory, SIGNAL(modelPopulated(QAbstractItemModel *)),
            this, SLOT(endAppendTestRun(QAbstractItemModel *)));
}

} // namespace Internal
} // namespace QTestLibPlugin
