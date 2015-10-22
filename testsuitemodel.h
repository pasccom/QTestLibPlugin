#ifndef TESTLIBMODEL_H
#define TESTLIBMODEL_H

#include <QAbstractItemModel>

namespace ProjectExplorer {
    class RunControl;
}

namespace QTestLibPlugin {
namespace Internal {

class QTestLibModel;

class AggregateItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    inline AggregateItemModel(QObject* parent = NULL):
        QAbstractItemModel(parent) {}
    QModelIndex index(int row, int column, const QModelIndex& parent) const;
    QModelIndex parent(const QModelIndex& child) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
public slots:
    void appendSubModel(QAbstractItemModel *model);
    void removeSubModel(int index);
    void clear(void);
private:
    QList<QAbstractItemModel *> mTests;
    mutable QMap<void *, QAbstractItemModel *> mInternalPointers;
};

class TestSuiteModel : public AggregateItemModel
{
    Q_OBJECT
public:
    inline TestSuiteModel(QObject* parent = NULL):
        AggregateItemModel(parent) {}
public slots:
    void appendTestRun(ProjectExplorer::RunControl* runControl);
    inline void removeTestRun(int index) {removeSubModel(index);}
private slots:
    inline void endAppendTestRun(QAbstractItemModel *testModel) {appendSubModel(testModel);}
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // TESTLIBMODEL_H
