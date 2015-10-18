#include "runconfiguration.h"

namespace ProjectExplorer {

RunControl::RunControl(RunConfiguration *runConfig, const QString& name, QObject *parent) :
    QObject(parent), mConfig(runConfig), mName(name)
{
    if (mName.isNull()) {
        mName = runConfig->displayName();
    }
}

} // ProjectExplorer

