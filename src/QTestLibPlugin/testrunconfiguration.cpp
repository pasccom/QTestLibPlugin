#include "testrunconfiguration.h"

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/localenvironmentaspect.h>
#include <projectexplorer/toolchain.h>
#include <projectexplorer/target.h>

namespace QTestLibPlugin {
namespace Internal {

TestRunConfigurationData::TestRunConfigurationData(ProjectExplorer::Target *target)
    :  mWorkingDirectory(QLatin1String(".")), mAutoMakeExe(), mMakeExe()
{
    mCmdArgs << QLatin1String("check");

    if (target != NULL) {
        Utils::Environment env = target->activeBuildConfiguration()->environment();
        ProjectExplorer::ToolChain *toolChain = ProjectExplorer::ToolChainKitInformation::toolChain(target->kit());
        mAutoMakeExe = Utils::FileName::fromString(toolChain->makeCommand(env));
    }
}

void TestRunConfigurationData::setMakeExe(const QString& path)
{
    Utils::FileName makeExe = Utils::FileName::fromUserInput(path);

    mMakeExe = (mAutoMakeExe == makeExe ? Utils::FileName() : makeExe);
}

QString TestRunConfigurationData::commandLineArguments(void) const
{
    return mCmdArgs.join(QLatin1Char(' '));
}

TestRunConfiguration::TestRunConfiguration(ProjectExplorer::Target *parent, Core::Id id):
    ProjectExplorer::LocalApplicationRunConfiguration(parent, id)
{
    addExtraAspect(new ProjectExplorer::LocalEnvironmentAspect(this));

    mData = new TestRunConfigurationData(parent);
    setDefaultDisplayName(QLatin1String("make check"));
}

TestRunConfiguration::~TestRunConfiguration()
{
    delete mData;
}

} // Internal
} // QTestLibPlugin

