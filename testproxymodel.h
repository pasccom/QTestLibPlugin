#ifndef TESTPROXYMODEL_H
#define TESTPROXYMODEL_H

#include "qtestlibmodel.h"

#include <QSortFilterProxyModel>
#include <QToolButton>

namespace QTestLibPlugin {
namespace Internal {

/*!
 * \brief The TestProxyModel class filters QTestLibModel (or TestSuiteModel)
 *
 * This class is an implementation of QAbstractProxyModel to filter QTestLibModel
 * according to the QTestLibModel::MessageType of the message.
 *
 * The filter can be configured easily thanks to TestProxyButton, which allow to
 * filter in or out some message types easily.
 */
class TestProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * Constructs a new TestProxyModel with all QTestLibModel::MessageType enabled.
     *
     * \param parent The parent of the model.
     */
    TestProxyModel(QObject *parent = NULL);
    /*!
     * \brief Filters in a QTestLibModel::MessageType
     *
     * Allows a QTestLibModel::MessageType to pass through the filter.
     *
     * \param type The type of the message to allow
     * \sa disableMessageType(), toogleMessageType(), isMessageTypeEnabled()
     */
    inline void enableMessageType(QTestLibModel::MessageType type) {mFilters[(int) type] = true; invalidateFilter();}
    /*!
     * \brief Filters out a QTestLibModel::MessageType
     *
     * Blocks a QTestLibModel::MessageType. The items of this type will not be displayed.
     *
     * \param type The type of the message to block
     * \sa enableMessageType(), toogleMessageType(), isMessageTypeEnabled()
     */
    inline void disableMessageType(QTestLibModel::MessageType type) {mFilters[(int) type] = false; invalidateFilter();}
    /*!
     * \brief Toogles a QTestLibModel::MessageType
     *
     * Allows a QTestLibModel::MessageType if it was previsouly blocked
     * of blocks it if it was previously allowed.
     *
     * \param type The type of the message whose state will be changed
     * \sa enableMessageType(), disableMessageType(), isMessageTypeEnabled()
     */
    inline void toogleMessageType(QTestLibModel::MessageType type) {mFilters[(int) type] = !mFilters.at((int) type); invalidateFilter();}
    /*!
     * \brief Is a QTestLibModel::MessageType enabled?
     *
     * Tells whether a QTestLibModel::MessageType is filtered out.
     *
     * \param type The type of message to be tested
     * \return \c true is the message type pass through the filter
     * \sa enableMessageType(), disableMessageType(), toogleMessageType()
     */
    inline bool isMessageTypeEnabled(QTestLibModel::MessageType type) const {return mFilters.at((int) type);}
protected:
    /*!
     * \internal
     * \brief Does the row pass through  the filter?
     *
     * This is the core function of TestProxyModel.
     * It tells whether a source model row pass through the filter.
     *
     * \param sourceRow The number of the row to be tested in the source model.
     * \param sourceParent The parent of the row to be tested in the source model.
     * \return \c true if the source model row pass through the filter.
     */
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;
private:
    /*!
     * \brief State of the filter
     *
     * Each cell in the array indicates whether the row pass through the filter (\c true in this case).
     */
    QVector<bool> mFilters;
};

class TestProxyButton : public QToolButton
{
    Q_OBJECT
public:
    TestProxyButton(QTestLibModel::MessageType messageType, TestProxyModel *proxy = NULL, QWidget *parent = NULL);
protected:
    void paintEvent(QPaintEvent *pe);
private slots:
    void handleClick(bool checked);
private:
    QTestLibModel::MessageType mType;
    TestProxyModel *mProxy;
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // TESTPROXYMODEL_H
