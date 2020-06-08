/* Copyright 2016-2020 Pascal COMBES <pascom@orange.fr>
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
 *
 * \sa TestProxyButton
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
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
private:
    /*!
     * \brief State of the filter
     *
     * Each cell in the array indicates whether the row pass through the filter (\c true in this case).
     */
    QVector<bool> mFilters;
};

/*!
 * \brief The TestProxyButton class allow to configure easily TestProxyModel.
 *
 * This class is a derivation of QToolButton which is used for the configuration
 * of the QTestLibModel filter, TestProxyModel.
 *
 * The button automatically comes with the right QIcon and tool tip
 * and uses the methods TestProxyModel::enableMessageType() and TestProxyModel::disableMessageType().
 *
 * \sa TestProxyModel
 */
class TestProxyButton : public QToolButton
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * Constructs a new instance linked with the given model for the given type.
     * The tool tip and QIcon are automatically found using
     * QTestLibModel::resultStringTr() and QTestLibModel::messageIcon().
     *
     * \param messageType The type of message this button configures
     * \param proxy The model this button configures
     * \param parent The parent of the model
     */
    TestProxyButton(QTestLibModel::MessageType messageType, TestProxyModel *proxy = NULL, QWidget *parent = NULL);
protected:
    /*!
     * \internal
     * \brief The paint event
     *
     * This function is called when the button must be repainted.
     *
     * It draws the background of the button and then call the appropriate
     * method in QStyle to draw the foreground (QIcon).
     *
     * \param pe The paint event
     */
    void paintEvent(QPaintEvent *pe) override;
private slots:
    /*!
     * \brief Handles a click
     *
     * This slot is connected to the clicked button of the button.
     * It is in charge of changing the state of TestProxyModel filter.
     *
     * \param checked \c true when the button is checked.
     */
    void handleClick(bool checked);
private:
    QTestLibModel::MessageType mType; /*!< The message type which is filtered in of out by this button */
    TestProxyModel *mProxy; /*!< The test proxy filter model on which this button acts */
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // TESTPROXYMODEL_H
