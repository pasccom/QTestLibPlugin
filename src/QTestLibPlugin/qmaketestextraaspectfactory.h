#ifndef QMAKETESTEXTRAASPECTFACTORY_H
#define QMAKETESTEXTRAASPECTFACTORY_H

#include "testextraaspect.h"

namespace QTestLibPlugin {
namespace Internal {

class QMakeTestExtraAspectFactory : public TestExtraAspectFactory
{
    Q_OBJECT
public:
    inline QMakeTestExtraAspectFactory(QObject* parent = NULL) :
        TestExtraAspectFactory(parent) {}
protected:
    bool isUseful(ProjectExplorer::RunConfiguration* runConfiguration) const override;
};

} // Internal
} // QTestLibPlugin

#endif // QMAKETESTEXTRAASPECTFACTORY_H
