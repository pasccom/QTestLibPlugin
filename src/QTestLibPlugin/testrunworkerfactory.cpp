#include "testrunworkerfactory.h"

#include "qtestlibpluginconstants.h"

#include <projectexplorer/projectexplorerconstants.h>

namespace QTestLibPlugin {
namespace Internal {

TestRunWorkerFactory::TestRunWorkerFactory()
    : ProjectExplorer::RunWorkerFactory()
{
    addSupportedRunMode(ProjectExplorer::Constants::NORMAL_RUN_MODE);
    addSupportedRunMode(ProjectExplorer::Constants::DEBUG_RUN_MODE);

    addSupportedDeviceType(ProjectExplorer::Constants::DESKTOP_DEVICE_TYPE);

    addSupportedRunConfig(Utils::Id(Constants::TestRunConfigurationId));

    setProduct<ProjectExplorer::ProcessRunner>();
}

} // Internal
} // QTestLibPlugin
