#include "testrunconfiguration.h"

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/localenvironmentaspect.h>
#include <projectexplorer/toolchain.h>
#include <projectexplorer/target.h>

namespace QTestLibPlugin {
namespace Internal {

TestRunConfigurationData::TestRunConfigurationData(ProjectExplorer::Target *target)
    :  jobNumber(1), testRunner(), workingDirectory(QLatin1String(".")), mAutoMakeExe(), mMakeExe()
{
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
    QStringList cmdArgs;
    QStringList testCmdArgs = testArgsParser.toStringList();

    if (jobNumber > 1)
        cmdArgs << QString(QLatin1String("-j%1")).arg(jobNumber);
    cmdArgs << QLatin1String("check");
    if (!testRunner.isEmpty())
        cmdArgs << QString(QLatin1String("TESTRUNNER=%1")).arg(testRunner);
    if (!testCmdArgs.isEmpty())
        cmdArgs << QString(QLatin1String("TESTARGS=\"%1\"")).arg(testCmdArgs.join(QLatin1Char(' ')));
    qDebug() << cmdArgs;

    return cmdArgs.join(QLatin1Char(' '));
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

