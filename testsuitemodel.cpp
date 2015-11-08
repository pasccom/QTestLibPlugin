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

    const QAbstractItemModel *subModel = mInternalPointers.value(parent.internalPointer(), NULL);
    Q_ASSERT(subModel != NULL);

    if (subModel == parent.internalPointer())
        return subModel->rowCount(QModelIndex());
    return subModel->rowCount(parent);
}

int AggregateItemModel::columnCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
        return 3;

    const QAbstractItemModel *subModel = mInternalPointers.value(parent.internalPointer(), NULL);
    Q_ASSERT(subModel != NULL);

    if (subModel == parent.internalPointer())
        return subModel->columnCount(QModelIndex());
    return subModel->columnCount(parent);
}

QModelIndex AggregateItemModel::index(int row, int column, const QModelIndex& parent) const
{
    const void *internalPointer = NULL;
    const QAbstractItemModel *subModel = NULL;

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
    return createIndex(row, column, const_cast<void *>(internalPointer));
}

QModelIndex AggregateItemModel::index(const QAbstractItemModel* model, const QModelIndex& idx) const
{
    if (!idx.isValid()) {
        int row = mSubModels.indexOf(model);
        Q_ASSERT(row != -1);
        if (row == -1)
            return QModelIndex();
        mInternalPointers.insert(model, model);
        return createIndex(row, 0, const_cast<QAbstractItemModel *>(model));
    }

    Q_ASSERT(mSubModels.contains(model));
    mInternalPointers.insert(idx.internalPointer(), model);
    return createIndex(idx.row(), idx.column(), idx.internalPointer());
}

QModelIndex AggregateItemModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return QModelIndex();

    const QAbstractItemModel *subModel = mInternalPointers.value(child.internalPointer(), NULL);
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

    const QAbstractItemModel *subModel = mInternalPointers.value(index.internalPointer(), NULL);
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

    connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, QVector<int>)),
            this, SLOT(subModelDataChanged(const QModelIndex&, const QModelIndex&, QVector<int>)));
    connect(model, SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int)),
            this, SLOT(subModelRowsAboutToBeInserted(const QModelIndex&, int, int)));
    connect(model, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
            this, SLOT(subModelRowsInserted(const QModelIndex&, int, int)));
    connect(model, SIGNAL(rowsAboutToBeMoved(const QModelIndex&, int, int, const QModelIndex&, int)),
            this, SLOT(subModelRowsAboutToBeMoved(const QModelIndex&, int, int, const QModelIndex&, int)));
    connect(model, SIGNAL(rowsMoved(const QModelIndex&, int, int, const QModelIndex&, int)),
            this, SLOT(subModelRowsMoved(const QModelIndex&, int, int, const QModelIndex&, int)));

    endInsertRows();
}

void AggregateItemModel::removeSubModel(int index)
{
    if ((index < 0) || (index >= mSubModels.size()))
        return;

    beginRemoveRows(QModelIndex(), index, index);

    const QAbstractItemModel *testRun = mSubModels.takeAt(index);
    delete testRun;

    endRemoveRows();

    QMap<const void*, const QAbstractItemModel*>::iterator it = mInternalPointers.begin();
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
    foreach (const QAbstractItemModel* model, mSubModels) {
        delete model;
    }

    mSubModels.clear();
    mInternalPointers.clear();
    endResetModel();
}


void AggregateItemModel::subModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(sender());
    Q_ASSERT(model != NULL);

    emit dataChanged(index(model, topLeft), index(model, bottomRight), roles);
}

void AggregateItemModel::subModelRowsAboutToBeInserted(const QModelIndex& parent, int start, int end)
{
    QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(sender());
    Q_ASSERT(model != NULL);

    beginInsertRows(index(model, parent), start, end);
}

void AggregateItemModel::subModelRowsInserted(const QModelIndex& parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);

    endInsertRows();
}

void AggregateItemModel::subModelRowsAboutToBeMoved(const QModelIndex& sourceParent, int sourceStart, int sourceEnd, const QModelIndex& destinationParent, int destinationRow)
{
    QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(sender());
    Q_ASSERT(model != NULL);

    beginMoveRows(index(model, sourceParent), sourceStart, sourceEnd, index(model, destinationParent), destinationRow);
}

void AggregateItemModel::subModelRowsMoved(const QModelIndex& sourceParent, int sourceStart, int sourceEnd, const QModelIndex& destinationParent, int destinationRow)
{
    Q_UNUSED(sourceParent);
    Q_UNUSED(sourceStart);
    Q_UNUSED(sourceEnd);
    Q_UNUSED(destinationParent);
    Q_UNUSED(destinationRow);

    endMoveRows();
}

void TestSuiteModel::appendTestRun(ProjectExplorer::RunControl* runControl)
{
    TestModelFactory *factory = new TestModelFactory(runControl, this);
    connect(factory, SIGNAL(modelFound(QAbstractItemModel *)),
            this, SLOT(endAppendTestRun(QAbstractItemModel *)));
    /*connect(factory, SIGNAL(modelPopulated(QAbstractItemModel *)),
            this, SLOT(endAppendTestRun(QAbstractItemModel *)));*/
}

} // namespace Internal
} // namespace QTestLibPlugin
