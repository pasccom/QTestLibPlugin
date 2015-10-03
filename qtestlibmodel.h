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

class QTestLibModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    QTestLibModel(ProjectExplorer::RunControl* runControl, QObject* parent = NULL);
    ~QTestLibModel(void);
    // WARNING Order matters!
    typedef enum {
        BenchmarkResult,
        QDebug,
        Info,
        Warning,
        QWarning,
        QSystem,
        QFatal,
        Unknown,
        Skip,
        Pass,
        BlackListedPass,
        XPass,
        XFail,
        BlackListedFail,
        Fail,
    } MessageType;
    typedef enum {
        ResultRole = Qt::UserRole,
        ResultStringRole,
        UserRole // WARNING No value after this one!
    } ItemRole;
    QModelIndex index(int row, int column, const QModelIndex& parent) const;
    QModelIndex parent(const QModelIndex& child) const;
    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex &index, int role) const;
public slots:
    void clear(void) {if (mRoot != NULL) mRoot->removeChildren();}
    void addTestItem(ProjectExplorer::RunControl* runControl,
                     MessageType type,
                     const QString& className,
                     const QString& functionName,
                     const QString& rowTitle,
                     const QString& message);
    void appendTestItemMessage(ProjectExplorer::RunControl* runControl, const QString& message);
private:
    //QIcon messageIcon(MessageType type) const;

    ProjectExplorer::RunControl *mTestRun;

    typedef enum {
        TestRoot,
        TestClass,
        TestCase,
        TestRow,
        TestMessage
    } TestItemType;

    class TestItem
    {
    public:
        TestItem(TestItem *parent = NULL);
        virtual inline ~TestItem(void) {removeChildren();}
        virtual TestItemType type(void) const = 0;
        virtual int compareName(const QString& name) const = 0;
        virtual int columnCount(void) const = 0;
        virtual QVariant data(int column, int role = Qt::DisplayRole) const;

        virtual TestItem* findChild(const QString& name) const;
        virtual int findChid(TestItem *item) const;
        virtual TestItem* child(int row) const;
        virtual inline int childrenCount(void) const {return mChildrenCount;}
        virtual void appendChild(TestItem *item);
        virtual bool removeChild(TestItem *item);
        virtual void removeChildren(void);

        inline TestItem* parent(void) const {return mParent;}
        void updateResult(MessageType result);
    protected:
        MessageType mResult;
    private:
        QLinkedList<TestItem *> mChildren;
        int mChildrenCount;
        TestItem* mParent;
    };

    class TestRootItem : public TestItem
    {
    public:
        inline TestRootItem(TestItem *parent = NULL) :
            TestItem(parent) {}
        inline TestItemType type(void) const {return TestRoot;}
        inline int compareName(const QString& name) const {Q_UNUSED(name); return 1;}
        inline int columnCount(void) const {return 1;}
        //inline QVariant data(int column, int role = Qt::DisplayRole) const {Q_UNUSED(column); Q_UNUSED(role); return QVariant();}
    };

    class TestClassItem : public TestItem
    {
    public:
        inline TestClassItem(const QString& className, TestItem *parent = NULL) :
            TestItem(parent), mClassName(className) {}
        inline TestItemType type(void) const {return TestClass;}
        inline int compareName(const QString& name) const {return QString::compare(mClassName, name, Qt::CaseSensitive);}
        inline int columnCount(void) const {return 1;}
        QVariant data(int column, int role = Qt::DisplayRole) const;
        inline QString getClassName(void) const {return mClassName;}
    private:
        QString mClassName;
    };

    class TestCaseItem : public TestItem
    {
    public:
        inline TestCaseItem(const QString& functionName, TestItem *parent = NULL) :
            TestItem(parent), mFunctionName(functionName) {}
        inline TestItemType type(void) const {return TestCase;}
        inline int compareName(const QString& name) const {return QString::compare(mFunctionName, name, Qt::CaseSensitive);}
        inline int columnCount(void) const {return 1;}
        QVariant data(int column, int role = Qt::DisplayRole) const;
        inline QString getFunctionName(void) const {return mFunctionName;}
    private:
        QString mFunctionName;
    };

    class TestRowItem : public TestItem
    {
    public:
        inline TestRowItem(const QString& title, TestItem *parent = NULL) :
            TestItem(parent), mTitle(title) {}
        inline TestItemType type(void) const {return TestRow;}
        inline int compareName(const QString& name) const {return QString::compare(mTitle, name, Qt::CaseSensitive);}
        inline int columnCount(void) const {return 1;}
        QVariant data(int column, int role = Qt::DisplayRole) const;
        inline QString getTitle(void) const {return mTitle;}
    private:
        QString mTitle;
    };

    class TestMessageItem : public TestItem
    {
    public:
        TestMessageItem(MessageType type, const QString& msg = QString::null, TestItem *parent = NULL);
        inline TestItemType type(void) const {return TestMessage;}
        inline int compareName(const QString& name) const {Q_UNUSED(name); return 1;}
        inline int columnCount(void) const {return 0;}
        QVariant data(int column, int role = Qt::DisplayRole) const;

        inline TestItem* findChild(const QString& name) const {Q_UNUSED(name); return NULL;}
        inline int findChid(TestItem *item) const {Q_UNUSED(item); return -1;}
        inline TestItem* child(int row) const {Q_UNUSED(row); return NULL;}
        inline int childrenCount(void) const {return 0;}
        inline void appendChild(TestItem *item) {Q_UNUSED(item); Q_ASSERT(false);}
        inline bool removeChild(TestItem *item) {Q_UNUSED(item); Q_ASSERT(false); return false;}
        inline void removeChildren(void) {}

        inline MessageType getMessageType(void) const {return mResult;}
        inline QString getMessage(void) const {return mMessage.trimmed();}
        inline void appendMessage(const QString& message) {mMessage.append(QLatin1Char('\n')).append(message);}
    private:
        QString mMessage;
    };

    TestItem *mRoot;
    TestMessageItem *mCurrentMessageItem;
};

} // namespace Internal
} // namespace QTestLibPlugin

Q_DECLARE_METATYPE(QTestLibPlugin::Internal::QTestLibModel::MessageType)

#endif // QTESTLIBMODEL_H
