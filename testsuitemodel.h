#ifndef TESTLIBMODEL_H
#define TESTLIBMODEL_H

#include <QAbstractItemModel>

namespace ProjectExplorer {
    class RunControl;
}

namespace QTestLibPlugin {
namespace Internal {

class QTestLibModel;

class TestSuiteModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    TestSuiteModel(QObject* parent = NULL);
    QModelIndex index(int row, int column, const QModelIndex& parent) const;
    QModelIndex parent(const QModelIndex& child) const;
    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex &index, int role) const;
public slots:
    void appendTestRun(ProjectExplorer::RunControl* runControl);
    void removeTestRun(int index);
    //void removeTestRun(const QModelIndex& index);
    void clear(void);
private slots:
    void endAppendTestRun(QAbstractItemModel *testModel);
private:
    /*QModelIndex findAncestor(const QModelIndex& index) const;
    QModelIndex subModelIndex(QAbstractItemModel *subModel, const QModelIndex& index) const;*/
    QList<QAbstractItemModel *> mTests;
    mutable QMap<void *, QAbstractItemModel *> mInternalPointers;
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // TESTLIBMODEL_H
