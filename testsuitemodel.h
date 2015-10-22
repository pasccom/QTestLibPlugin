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
private:
    QList<QAbstractItemModel *> mSubModels; /*!< The list of sub models */
    mutable QMap<void *, QAbstractItemModel *> mInternalPointers; /*!< A map between indexes internal pointers and submodels */
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
