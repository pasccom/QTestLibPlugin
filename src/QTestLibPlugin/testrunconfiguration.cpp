#include "testrunconfiguration.h"

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/toolchain.h>
#include <projectexplorer/target.h>

namespace QTestLibPlugin {
namespace Internal {

TestRunConfiguration::TestRunConfiguration(ProjectExplorer::Target *parent, Core::Id id):
    ProjectExplorer::LocalApplicationRunConfiguration(parent, id), mWorkingDirectory(QLatin1String("."))
{
    setDefaultDisplayName(QLatin1String("make check"));
    mCmdArgs << QLatin1String("check");
    qDebug() << "make Command:" << findMake(parent);
    mMakeExe = findMake(parent);
}

TestRunConfiguration::~TestRunConfiguration()
{

}

QString TestRunConfiguration::findMake(ProjectExplorer::Target* target)
{
    Utils::Environment env = target->activeBuildConfiguration()->environment();
    ProjectExplorer::ToolChain *toolChain = ProjectExplorer::ToolChainKitInformation::toolChain(target->kit());
    return toolChain->makeCommand(env);
}

} // Internal
} // QTestLibPlugin

