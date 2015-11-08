#ifndef TESTPROXYMODEL_H
#define TESTPROXYMODEL_H

#include <QSortFilterProxyModel>

namespace QTestLibPlugin {
namespace Internal {

class TestProxyModel : public QSortFilterProxyModel
{
public:
    inline TestProxyModel(QObject *parent = NULL) :
        QSortFilterProxyModel(parent) {}
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // TESTPROXYMODEL_H
