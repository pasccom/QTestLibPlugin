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

#ifndef TESTLIBMODEL_H
#define TESTLIBMODEL_H

#include <QAbstractItemModel>

namespace ProjectExplorer {
    class RunControl;
}

namespace QTestLibPlugin {
namespace Internal {

class QTestLibModel;

/*!
 * \brief The AggregateItemModel class aggregates QAbstractItemModel into a new model
 *
 * This class implements a QAbstractItemModel by aggregating sub QAbstractItemModel.
 * Each children of the root corresponds to the root item of a sub model.
 *
 * QAbstractItemModel signals from sub models are forwarded by this class.
 * Their QModelIndex arguments are converted to QModelIndexes for this class.
 *
 * Sub models are added with the slot AggregateItemModel::appendSubModel() and
 * removed with the slot AggregateItemModel::removeSubModel().
 */
class AggregateItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * Just call parent constructor
     *
     * \param parent The parent of the model.
     */
    inline AggregateItemModel(QObject* parent = NULL):
        QAbstractItemModel(parent) {}
    /*!
     * \brief Returns child model index
     *
     * Returns the model index for the child of the given parent
     * at the given line and column.
     *
     * \param row The row of the desired child
     * \param column The column of the desired child
     * \param parent The parent of the desired model index
     * \return The model index for the child of the given parent
     * at the given line and column
     * \sa parent()
     */
    QModelIndex index(int row, int column, const QModelIndex& parent) const;
    /*!
     * \brief Returns parent model index
     *
     * Returns the model index for the parent of the given child.
     *
     * \param child A child of the desired model index
     * \return The model index for the parent of the given child
     * \sa index()
     */
    QModelIndex parent(const QModelIndex& child) const;
    /*!
     * \brief Number of child rows
     *
     * Number of child item rows under the given parent.
     *
     * \param parent The model index of the parent for which children row count is desired
     * \return The number of child item rows
     * \sa columnCount()
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    /*!
     * \brief Number of child colums
     *
     * Number of child item columns under the given parent.
     *
     * \param parent The model index of the parent for which children column count is desired
     * \return The number of child item columns
     * \sa rowCount()
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    /*!
     * \brief Data for model index
     *
     * Returns the data for the given role and the given model index.
     *
     * \param index The index whose data is desired
     * \param role The role for the data
     * \return The data for the given role and the given model index
     */
    QVariant data(const QModelIndex &index, int role) const;
public slots:
    /*!
     * \brief Appends a submodel.
     *
     * Appends QAbstractListModel at the end of the list of sub models.
     *
     * \param model The model to be appended to submodel list.
     * \sa removeSubModel()
     */
    void appendSubModel(QAbstractItemModel *model);
    /*!
     * \brief Removes a submodel
     *
     * Removes and desallocates the submodel at the given index.
     *
     * \param index The index where the submodel will be removed.
     * \sa appendSubModel(), clear()
     */
    void removeSubModel(int index);
    /*!
     * \brief Clears submodels
     *
     * Desallocates all submodel and clear the list of sub models.
     *
     * \sa removeSubModel()
     */
    void clear(void);
private slots:
    /*!
     * \brief Sub model data changed
     *
     * This slot is intended to be connected to sub model dataChanged() signal.
     * It converts the QModelIndexes (see index(const QAbstractItemModel*, const QModelIndex&) const)
     * and send the signal again.
     *
     * \param topLeft Top-left QModelIndex affected
     * \param bottomRight Bottom-right QModelIndex affected
     * \param roles The roles affected (if empty, all roles are affected)
     */
    void subModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());
    /*!
     * \brief Rows about to be inserted in sub model
     *
     * This slot is intended to be connected to sub model rowsAboutToBeInserted() signal.
     * It converts the QModelIndexes (see index(const QAbstractItemModel*, const QModelIndex&) const)
     * and send the signal again.
     *
     * \note This function uses beginInsertRows().
     *
     * \param parent The QModelIndex where rows will be inserted
     * \param start First row being affected
     * \param end Last row being affected
     * \sa subModelRowsInserted()
     */
    void subModelRowsAboutToBeInserted(const QModelIndex& parent, int start, int end);
    /*!
     * \brief Rows inserted in sub model
     *
     * This slot is intended to be connected to sub model rowsInserted() signal.
     * It converts the QModelIndexes (see index(const QAbstractItemModel*, const QModelIndex&) const)
     * and send the signal again.
     *
     * \note This function uses endInsertRows(), thus it does not use its arguments.
     *
     * \param parent The QModelIndex where rows were inserted (Unused)
     * \param start First row being affected (Unused)
     * \param end Last row being affected (Unused)
     * \sa subModelRowsAboutToBeInserted()
     */
    void subModelRowsInserted(const QModelIndex& parent, int start, int end);
    /*!
     * \brief Rows about to be moved in sub model
     *
     * This slot is intended to be connected to sub model rowsAboutToBeMoved() signal.
     * It converts the QModelIndexes (see index(const QAbstractItemModel*, const QModelIndex&) const)
     * and send the signal again.
     *
     * \note This function uses beginMoveRows().
     *
     * \param sourceParent The QModelIndex where rows will be moved from
     * \param sourceStart First row being affected in the source
     * \param sourceEnd Last row being affected in the source
     * \param destinationParent The QModelIndex where rows will be moved to
     * \param destinationRow Row where QModelIndexes will be moved to
     * \sa subModelRowsMoved()
     */
    void subModelRowsAboutToBeMoved(const QModelIndex& sourceParent, int sourceStart, int sourceEnd, const QModelIndex& destinationParent, int destinationRow);
    /*!
     * \brief Rows moved in sub model
     *
     * This slot is intended to be connected to sub model rowsMoved() signal.
     * It converts the QModelIndexes (see index(const QAbstractItemModel*, const QModelIndex&) const)
     * and send the signal again.
     *
     * \note This function uses endMoveRows(), thus it does not use its arguments.
     *
     * \param sourceParent The QModelIndex where rows were moved from (Unused)
     * \param sourceStart First row being affected in the source (Unused)
     * \param sourceEnd Last row being affected in the source (Unused)
     * \param destinationParent The QModelIndex where rows were moved to (Unused)
     * \param destinationRow Row where QModelIndexes were moved to (Unused)
     * \sa subModelRowsAboutToBeMoved()
     */
    void subModelRowsMoved(const QModelIndex& sourceParent, int sourceStart, int sourceEnd, const QModelIndex& destinationParent, int destinationRow);
private:
    /*!
     * \brief Convert sub model QModelIndex
     *
     * Convert a QModelIndex coming from a given sub model
     * into a QModelIndex of this model.
     * Also updates mInternalPointers accordingly.
     *
     * \param model The model of the QModelIndex (should be a sub model)
     * \param idx The QModelIndex (belonging to the given sub model)
     * \return The same QModelIndex for this model
     */
    QModelIndex index(const QAbstractItemModel* model, const QModelIndex& idx = QModelIndex()) const;

    QList<const QAbstractItemModel *> mSubModels; /*!< The list of sub models */
    mutable QMap<const void *, const QAbstractItemModel *> mInternalPointers; /*!< A map between indexes internal pointers and submodels */
};

/*!
 * \brief Specialisation of AggregateItemModel for test suites.
 *
 * The TestSuiteModel class is a specialisation of AggregateItemModel
 * which will contain test models (see \ref QTestLibModel).
 *
 * A ProjectExplorer::RunControl can be added to the model with the slot
 * TestSuiteModel::appendTestRun().
 */
class TestSuiteModel : public AggregateItemModel
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * Just call parent constructor
     *
     * \param parent The parent of the model.
     */
    inline TestSuiteModel(QObject* parent = NULL):
        AggregateItemModel(parent) {}
public slots:
    /*!
     * \brief Appends a test run to the test suite model.
     *
     * This function uses an instance of TestModelFactory to
     * populate the test run model with the output of the ProjectExplorer::RunControl.
     * When the run control is finished, the sub model is effectively appended
     * by endAppendTestRun(;
     *
     * \param runControl The run control whose output will be used to populate the model.
     * \sa removeTestRun()
     */
    void appendTestRun(ProjectExplorer::RunControl* runControl);
    /*!
     * \brief Removes a test run from the test suite model
     *
     * Removes a test run from the test suite model by using AggregateItemModel::removeSubModel().
     *
     * \param index The index where the test run will be removed.
     * \sa appendTestRun()
     */
    inline void removeTestRun(int index) {removeSubModel(index);}
private slots:
    /*!
     * \brief Effectively append a test run model to the test suite model
     *
     * Effectively appends a test run to the test suite model by using AggregateItemModel::appendSubModel().
     *
     * \param testModel The test model created by TestModelFactory to append.
     */
    inline void endAppendTestRun(QAbstractItemModel *testModel) {appendSubModel(testModel);}
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // TESTLIBMODEL_H
