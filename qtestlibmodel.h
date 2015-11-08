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

#ifndef QTESTLIBMODEL_H
#define QTESTLIBMODEL_H

#include <QAbstractItemModel>
#include <QLinkedList>
#include <utils/outputformat.h>

namespace ProjectExplorer {
    class RunControl;
}

namespace QTestLibPlugin {
namespace Internal {
/*!
 * \brief The QTestLibModel class models QTestLib output.
 *
 * This class is the implementation of a QAbstractItemModel to model QTestLib output.
 * Items are arranged in a tree where the test contains tested classes,
 * which contain tested function, which contain data sets and the messages.
 *
 * This class add two new item roles:
 *     - QTestLibModel::ResultRole contains a QVariant of type QTestLibModel::MessageType
 * which represents the type of the item.
 *     - QTestLibModel::ResultStringRole contains a QString which represents the type
 * of the item under textual form.
 *
 * The indexes of this model have three columns:
 *     1. This column contains the message or the name
 * (class name, test case name or data row title) of the item;
 *     2. This column contains the file part of the location of the event (if it is known).
 *     3. This column contains the line part of the location of the event (if it is known).
 */
class QTestLibModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * Constructs an instance of this class for the given ProjectExplorer::RunControl
     * with the given parent.
     *
     * \param runControl The run control whose output will be modelled
     * \param parent The parent of the model
     */
    QTestLibModel(ProjectExplorer::RunControl* runControl, QObject* parent = NULL);
    /*!
     * \brief Destructor
     *
     * Deallocates recursively the underlying item tree.
     */
    ~QTestLibModel(void);
    /*!
     * \brief Types of message
     *
     * List the possible types of message.
     * These types are associated with the decoration of items.
     *
     * \note It is declared as a Qt metatype.
     */
    typedef enum {// WARNING Order matters!
        Signal, /*!< The message describes a signal */
        Duration, /*!< The message gives the duration of a function or a test */
        BenchmarkResult, /*!< The message gives the result of a benchmark */
        QDebug, /*!< The message is a debug message */
        Info, /*!< The message is an information message coming from the test system */
        Warning, /*!< The message is a warning message coming from the test system */
        QWarning, /*!< The message is a warning message */
        QSystem,/*!< The message is a system message */
        QFatal, /*!< The message is a fatal error message */
        Unknown, /*!< The message is of unknown type */
        Skip, /*!< The message signals a test case has been skipped */
        Pass, /*!< The message signals a test case has passed */
        BlackListedPass, /*!< The message signals a blacklisted test case has passed */
        XPass, /*!< The message signals a test case has passed unexpectedly */
        XFail, /*!< The message signals a test case has failed unexpectedly */
        BlackListedFail, /*!< The message signals a blacklisted test case has failed */
        Fail, /*!< The message signals a test case has failed */
    } MessageType;
    /*!
     * \brief New item roles
     *
     * Describes new item roles related to message types.
     */
    typedef enum {
        ResultRole = Qt::UserRole, /*!< The message type as a QVariant with underlying type QTestLibModel::MessageType */
        ResultStringRole, /*!< The message type as a QString */
        UserRole /*!< New begin for other defined roles */ // WARNING No value after this one!
    } ItemRole;
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
    int rowCount(const QModelIndex& parent) const;
    /*!
     * \brief Number of child colums
     *
     * Number of child item columns under the given parent.
     *
     * \param parent The model index of the parent for which children column count is desired
     * \return The number of child item columns
     * \sa rowCount()
     */
    int columnCount(const QModelIndex& parent) const;
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
     * \brief Clears the model
     *
     * Recursively deletes all items in the model.
     */
    void clear(void) {if (mRoot != NULL) mRoot->removeChildren();}
    /*!
     * \brief Adds a test item
     *
     * Add a new test item in the model with the given data.
     * The item is appended to the given class and the given function.
     *
     * \note If the class, the function of the data row do not exist,
     * they are created automatically.
     *
     * \param runControl The run control from which the item originates.
     * It must be identical to the run control which is passed to the constructor.
     * \param type The type of the message
     * \param className The class being tested
     * \param functionName The function being tested
     * \param rowTitle The title of the data row being used
     * \param message The message from the test
     * \sa appendTestItemMessage() appendTestLocation()
     */
    void addTestItem(ProjectExplorer::RunControl* runControl,
                     MessageType type,
                     const QString& className,
                     const QString& functionName,
                     const QString& rowTitle,
                     const QString& message);
    /*!
     * \brief Appends location to current item.
     *
     * This function should be used after addTestItem()
     * to add information on the location of the event
     *
     * \param runControl The run control from which the item originates.
     * It must be identical to the run control which is passed to the constructor.
     * \param file The file part of the location
     * \param line The line part of the location
     * \sa addTestItem()
     */
    void appendTestLocation(ProjectExplorer::RunControl* runControl, const QString& file, unsigned int line);
    /*!
     * \brief Append text to current MessageType
     *
     * Append the given text to the current message.
     * This is especially useful for multiline messages.
     *
     * \param runControl The run control from which the item originates.
     * It must be identical to the run control which is passed to the constructor.
     * \param message The text to be appended to the current message item.
     * \sa addTestItem()
     */
    void appendTestItemMessage(ProjectExplorer::RunControl* runControl, const QString& message);
private:
    /*!
     * \brief Test item type.
     *
     * Represent internal types of items.
     *
     * \sa TestItem, TestRootItem, TestClassItem, TestCaseItem, TestRowItem, TestMessageItem
     */
    typedef enum {
        TestRoot, /*!< Type for the root item of the test (see \ref TestRootItem) */
        TestClass, /*!< Type for class items in the test (see \ref TestClassItem) */
        TestCase, /*!< Type for case items in the test (see \ref TestCaseItem) */
        TestRow, /*!< Type for row items in the test (see \ref TestRowItem) */
        TestMessage /*!< Type for message items in the test (see \ref TestMessageItem) */
    } TestItemType;

    /*!
     * \brief Abstract base for representation of test items.
     *
     * The TestItem class is an abstract base for the representation of test items.
     * It implements the tree structure for the test items.
     * It also contains the location information for test items.
     */
    class TestItem
    {
    public:
        /*!
         * \brief Constructor
         *
         * Constructs a test item with the given parent.
         *
         * \note If parent is not \c NULL, the child is
         * automatically apppended to its children.
         *
         * \param parent The parent of the test item.
         */
        TestItem(TestItem *parent = NULL);
        /*!
         * \brief Destructor
         *
         * Deletes children recursively.
         */
        virtual inline ~TestItem(void) {removeChildren();}
        /*!
         * \brief Item type
         *
         * Returns the internal item type of the current item.
         *
         * \return The internal item type of the current item
         * \sa TestItemType
         */
        virtual TestItemType type(void) const = 0;
        /*!
         * \brief Compare names
         *
         * This function should return 0 when the internal name
         * of the item compares to the given name.
         * Otherwise returns 1 or -1 as QString::compare().
         *
         * \param name The name to compare to.
         * \return -1, 0 or 1 to indicate whether the item internal name
         * is less, equal or greather to the given name.
         */
        virtual int compareName(const QString& name) const = 0;
        /*!
         * \brief Number of child columns
         *
         * Returns the number of child item columns.
         *
         * \return The number of child item columns.
         * \sa QTestLibModel::columnCount()
         */
        virtual int columnCount(void) const = 0;
        /*!
         * \brief Data for item
         *
         * Returns the data for the given role and the given column
         *
         * \param column The column of the desired data
         * \param role The role for the data
         * \return The data for the given role and the given column
         * \sa QTestLibModel::data()
         */
        virtual QVariant data(int column, int role = Qt::DisplayRole) const;

        /*!
         * \brief Find last child
         *
         * Find the last child of the item matching the given name.
         * This function uses compareName() for the matches.
         *
         * \note This function is not recursive
         *
         * \param name The name of the searched child.
         * \return A pointer to the searched child item
         * or \c NULL if not found.
         */
        virtual TestItem* findChild(const QString& name) const;
        /*!
         * \brief Find row for child
         *
         * Find the row of a child of the current item.
         * If the given child is not found, returns -1.
         *
         * \param item A child of the current item.
         * \return The line of the given child or
         * -1 if the given item is not a child of the current item.
         */
        virtual int findChid(TestItem *item) const;
        /*!
         * \brief Return child at given row
         *
         * Returns the child of the current item at the given row.
         * \c NULL is returned if row is too big (greather than childrenCount()).
         *
         * \param row The row of the desired child
         * \return The child of the item at the given row or
         * \c NULL is the row is too large.
         */
        virtual TestItem* child(int row) const;
        /*!
         * \brief Number of children
         *
         * Returns the number of children of the current item.
         *
         * \return The number of children of the current item
         */
        virtual inline int childrenCount(void) const {return mChildrenCount;}
        /*!
         * \brief Append a child to current item
         *
         * Append an item to the children list of the current item.
         *
         * \note The given item should not have a parent.
         *
         * \param item The item to append to children list of the current item
         */
        virtual void appendChild(TestItem *item);
        /*!
         * \brief Replace the parent
         *
         * This item will replace the given item by taking ownership of its children.
         * \note The parent of child items is changed accordingly.
         * \note The result is set to the result of the replaced item.
         *
         * \param item The item to be replaced
         * \param first The first child whose ownership is taken
         * \param last The last child whose ownership is taken
         */
        virtual void replace(TestItem *item, int first = 0, int last = -1);
        /*!
         * \brief Remove a child from current item
         *
         * Removes a child from current item and delete it.
         * Returns \c true if at least an item has been removed.
         *
         * \param item The child to be removed
         * \return \c true if at least an item has been removed
         * \sa removeChildren()
         */
        virtual bool removeChild(TestItem *item);
        /*!
         * \brief Remove children of current item
         *
         * Removes all the children of current item and delete them.
         */
        virtual void removeChildren(void);

        /*!
         * \brief Parent of current item
         *
         * Returns the parent of current item.
         *
         * \return The parent of current item
         */
        inline TestItem* parent(void) const {return mParent;}
        /*!
         * \brief Updates the test result of current item
         *
         * This function updates the test results of the current item and
         * recursively of its parents if it is necessary.
         * The test result for an item is computed as the max of
         * the new result and the previous result.
         *
         * \param result The new result
         */
        void updateResult(MessageType result);

        /*!
         * \brief Set the file part of the location.
         *
         * Set the file part of the location of the associated event.
         *
         * \param file The file part of the location of the associated event.
         * \sa file(), setLine()
         */
        inline void setFile(const QString& file) {mFile = file;}
        /*!
         * \brief Returns the file part of the location.
         *
         * Retruns the file part of the location of the associated event.
         *
         * \returns The file part of the location of the associated event.
         * \sa setFile(), line()
         */
        inline QString file(void) const {return mFile;}
        /*!
         * \brief Set the line part of the location.
         *
         * Set the line part of the location of the associated event.
         *
         * \param line The line part of the location of the associated event.
         * \sa line(), setLine()
         */
        inline void setLine(unsigned int line) {mLine = line;}
        /*!
         * \brief Returns the line part of the location.
         *
         * Retruns the line part of the location of the associated event.
         *
         * \returns The line part of the location of the associated event.
         * \sa setLine(), file()
         */
        inline unsigned int line(void) const {return mLine;}
        /*!
         * \brief Is item located?
         *
         * Returns \c true if the item is located, i.e.
         * the file part of the location is not empty and the line is not 0.
         *
         * \return \c true if the item is located (see above).
         */
        inline bool isLocated(void) const {return (!mFile.isEmpty() && (mLine != 0));}
    protected:
        MessageType mResult; /*!< The current result of the test item */
    private:
        QLinkedList<TestItem *> mChildren; /*!< The children of the test item. */
        int mChildrenCount; /*!< The number of children of the test item (cached for facter access) */
        TestItem* mParent; /*!< The parent of the current item */

        QString mFile; /*!< The file part of the location of the associated event */
        unsigned int mLine; /*!< The line part of the location of the associated event */
    };

    /*!
     * \brief Implemenentation of TestItem to represent the root item of a test
     *
     * The TestRootItem class represent the root item of a test.
     * Only implement the three required methods from TestItem.
     *
     * This type of item does not have any internal name, hence the function
     * TestRootItem::compareName() always returns 1.
     */
    class TestRootItem : public TestItem
    {
    public:
        /*!
         * \brief Constructor
         *
         * Just call parent constructor
         *
         * \param parent The parent of the test item.
         */
        inline TestRootItem(TestItem *parent = NULL) :
            TestItem(parent) {}
        inline TestItemType type(void) const {return TestRoot;}
        inline int compareName(const QString& name) const {Q_UNUSED(name); return 1;}
        inline int columnCount(void) const {return 3;}
    };

    /*!
     * \brief Implemenentation of TestItem to represent a test class item
     *
     * The TestClassItem class represent a test class and stores its name.
     *
     * The internal name for TestClassItem is the name of the class.
     */
    class TestClassItem : public TestItem
    {
    public:
        /*!
         * \brief Constructor
         *
         * Constructs a test class item with the given class name.
         * The parent is passed to the parent constructor.
         *
         * \param className The class name for the current item
         * \param parent The parent of the current item.
         */
        inline TestClassItem(const QString& className, TestItem *parent = NULL) :
            TestItem(parent), mClassName(className) {}
        inline TestItemType type(void) const {return TestClass;}
        inline int compareName(const QString& name) const {return QString::compare(mClassName, name, Qt::CaseSensitive);}
        inline int columnCount(void) const {return 3;}
        QVariant data(int column, int role = Qt::DisplayRole) const;
        /*!
         * \brief The name of the test class
         *
         * Returns the name of the test class.
         *
         * \return The name of the test class
         */
        inline QString getClassName(void) const {return mClassName;}
    private:
        QString mClassName; /*!< The name of the test class */
    };

    /*!
     * \brief Implemenentation of TestItem to represent a test case item
     *
     * The TestCaseItem class represent a test function and stores its name.
     *
     * The internal name for TestCaseItem is the name of the function.
     */
    class TestCaseItem : public TestItem
    {
    public:
        /*!
         * \brief Constructor
         *
         * Constructs a test case item with the given function name.
         * The parent is passed to the parent constructor.
         *
         * \param functionName The function name for the current item
         * \param parent The parent of the current item.
         */
        inline TestCaseItem(const QString& functionName, TestItem *parent = NULL) :
            TestItem(parent), mFunctionName(functionName) {}
        inline TestItemType type(void) const {return TestCase;}
        inline int compareName(const QString& name) const {return QString::compare(mFunctionName, name, Qt::CaseSensitive);}
        inline int columnCount(void) const {return 3;}
        QVariant data(int column, int role = Qt::DisplayRole) const;
        /*!
         * \brief The name of the test function
         *
         * Returns the name of the test function.
         *
         * \return The name of the test function
         */
        inline QString getFunctionName(void) const {return mFunctionName;}
    private:
        QString mFunctionName; /*!< The name of the test function */
    };

    /*!
     * \brief Implemenentation of TestItem to represent a test data row item
     *
     * The TestRowItem class represent a test data row and stores its title.
     *
     * The internal name for TestRowItem is the title of the data row.
     */
    class TestRowItem : public TestItem
    {
    public:
        /*!
         * \brief Constructor
         *
         * Constructs a test data row item with the given title.
         * The parent is passed to the parent constructor.
         *
         * \param title The title for the current item
         * \param parent The parent of the current item.
         */
        inline TestRowItem(const QString& title, TestItem *parent = NULL) :
            TestItem(parent), mTitle(title) {}
        inline TestItemType type(void) const {return TestRow;}
        inline int compareName(const QString& name) const {return QString::compare(mTitle, name, Qt::CaseSensitive);}
        inline int columnCount(void) const {return 3;}
        QVariant data(int column, int role = Qt::DisplayRole) const;
        /*!
         * \brief The title of the test data row
         *
         * Returns the title of the test data row.
         *
         * \return The title of the test data row
         */
        inline QString getTitle(void) const {return mTitle;}
    private:
        QString mTitle; /*!< The title of the test data row */
    };

    /*!
     * \brief Implemenentation of TestItem to represent a test message item
     *
     * The TestMessageItem class represent a test message and stores it.
     * Test messages also have a type which is equal to their results.
     *
     * Test messages cannot have children, hence all child lookup functions of TestItem
     * are reimplemented to return the results thy would return if there was no children.
     *
     * The internal name for TestRowItem is the title of the data row.
     */
    class TestMessageItem : public TestItem
    {
    public:
        /*!
         * \brief Constructor
         *
         * Construcs a test message item from its type and the message.
         *
         * \param type The type of the associated event.
         * \param msg The message
         * \param parent The parent of the current item.
         */
        TestMessageItem(MessageType type, const QString& msg = QString::null, TestItem *parent = NULL);
        inline TestItemType type(void) const {return TestMessage;}
        inline int compareName(const QString& name) const {Q_UNUSED(name); return 1;}
        inline int columnCount(void) const {return 0;}
        QVariant data(int column, int role = Qt::DisplayRole) const;

        /*!
         * \brief Find last child
         *
         * Find the last child of the item matching the given name.
         *
         * \note As test message item do not have any children, this function always returns \c NULL.
         *
         * \param name The name of the searched child (Unused)
         * \return \c NULL as test message items can not have children
         */
        inline TestItem* findChild(const QString& name) const {Q_UNUSED(name); return NULL;}
        /*!
         * \brief Find row for child
         *
         * Find the row of a child of the current item.
         *
         * \note As test message item do not have any children, this function always returns -1.
         *
         * \param item A child of the current item (Unused)
         * \return -1 as test message items can not have any children
         */
        inline int findChid(TestItem *item) const {Q_UNUSED(item); return -1;}
        /*!
         * \brief Return child at given row
         *
         * Returns the child of the current item at the given row.
         * \note As test message item do not have any children, this function always returns \c NULL.
         *
         * \param row The row of the desired child (Unused)
         * \return \c NULL as test message items can not have any children
         */
        inline TestItem* child(int row) const {Q_UNUSED(row); return NULL;}
        /*!
         * \brief Number of children
         *
         * Returns the number of children of the current item,
         * which is 0 as test message items can not have any children.
         *
         * \return 0 as test message items can not have any children
         */
        inline int childrenCount(void) const {return 0;}
        /*!
         * \brief Append a child to current item
         *
         * Append an item to the children list of the current item.
         *
         * \warning This function does nothing and should not be called
         * as test message items can not have any children.
         *
         * \param item The item to append to children list of the current item (Unused)
         */
        inline void appendChild(TestItem *item) {Q_UNUSED(item); Q_ASSERT(false);}
        /*!
         * \brief Replace the parent
         *
         * This item will replace the given item by taking ownershop of its children.
         *
         * \warning This function does nothing and should not be called
         * as test message items can not have any children.
         *
         * \param item The item to be replaced (Unused)
         * \param first The first child whose ownership is taken (Unused)
         * \param last The last child whose ownership is taken (Unused)
         */
        virtual void replace(TestItem *item, int first = 0, int last = -1) {Q_UNUSED(item); Q_UNUSED(first); Q_UNUSED(last); Q_ASSERT(false);}
        /*!
         * \brief Remove a child from current item
         *
         * Removes a child from current item and delete it.
         *
         * \warning This function does nothing and should not be called
         * as test message items can not have any children.
         *
         * \param item The child to be removed (Unused)
         * \return \c false as test message items can not have any children.
         * \sa removeChildren()
         */
        inline bool removeChild(TestItem *item) {Q_UNUSED(item); Q_ASSERT(false); return false;}
        /*!
         * \brief Remove children of current item
         *
         * Removes all the children of current item and delete them.
         * This function does nothing
         * as test message items can not have any children.
         *
         */
        inline void removeChildren(void) {}

        /*!
         * \brief Message type for the current item
         *
         * Returns the message type for the current item.
         *
         * \return The message type for the current item
         */
        inline MessageType getMessageType(void) const {return mResult;}
        /*!
         * \brief Message stored in the current item
         *
         * Returns the message stored in the current item.
         *
         * \return The message stored in the current item
         */
        inline QString getMessage(void) const {return mMessage.trimmed();}
        /*!
         * \brief Append a message to the current item
         *
         * Appends the given text to the message stored in the current item.
         *
         * \param message Text to be appended to the message stored in the current item
         */
        inline void appendMessage(const QString& message) {mMessage.append(QLatin1Char('\n')).append(message);}
    private:
        QString mMessage; /*!< The test message */
    };

    /*!
     * \brief Creates a test message item
     *
     * Allocates a test message item.
     * Automatically detects signal messages and
     * set the message type accordingly.
     * The newly allocated test message item
     * is set as mCurrentMessageItem.
     *
     * \param type The original message type
     * \param message The text of the test message
     * \param parent The parent item for the constructed test message item
     */
    void createTestMessageItem(MessageType type, const QString& message, TestItem* parent);

    /*!
     * \brief Returns QModelIndex corresponding to TestItem
     *
     * Recursively computes the QModelIndex associated with this test item for the given column.
     * \note In future releases, caching test item may be envisaged
     * as this function is quite expensive, when the model is big.
     *
     * \param testItem A test item whose corresponding QModelIndex is desired
     * \param column The column of the QModelIndex.
     * \return The corresponding QModelIndex
     */
    QModelIndex index(TestItem *testItem, int column = 0) const;

    ProjectExplorer::RunControl *mTestRun; /*!< The run control from which test data originates */
    TestItem *mRoot; /*!< The internal root item (may be a TestClassItem or a TestRootItem) */
    TestMessageItem *mCurrentMessageItem; /*!< The last test message item added (where message are appended by appendTestItemMessage() */
};

} // namespace Internal
} // namespace QTestLibPlugin

Q_DECLARE_METATYPE(QTestLibPlugin::Internal::QTestLibModel::MessageType)

#endif // QTESTLIBMODEL_H
