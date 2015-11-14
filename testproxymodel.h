#ifndef TESTPROXYMODEL_H
#define TESTPROXYMODEL_H

#include "qtestlibmodel.h"

#include <QSortFilterProxyModel>
#include <QToolButton>

namespace QTestLibPlugin {
namespace Internal {

class TestProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    TestProxyModel(QObject *parent = NULL);
    inline void enableMessageType(QTestLibModel::MessageType type) {mFilters[(int) type] = true; invalidateFilter();}
    inline void disableMessageType(QTestLibModel::MessageType type) {mFilters[(int) type] = false; invalidateFilter();}
    inline void toogleMessageType(QTestLibModel::MessageType type) {mFilters[(int) type] = !mFilters.at((int) type); invalidateFilter();}
    inline bool isMessageTypeEnabled(QTestLibModel::MessageType type) const {return mFilters.at((int) type);}
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;
private:
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
