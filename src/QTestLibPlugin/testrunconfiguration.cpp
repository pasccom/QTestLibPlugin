#include "testrunconfiguration.h"

namespace QTestLibPlugin {
namespace Internal {

TestRunConfiguration::TestRunConfiguration(ProjectExplorer::Target *parent, Core::Id id):
    ProjectExplorer::LocalApplicationRunConfiguration(parent, id), mWorkingDirectory(QLatin1String("."))
{
    setDefaultDisplayName(QLatin1String("make check"));
    mCmdArgs << QLatin1String("check");
    mMakeExe = QLatin1String("C:\\Dev\\msys2-w64\\mingw64\\bin\\mingw32-make.exe");
}

TestRunConfiguration::~TestRunConfiguration()
{

}

} // Internal
} // QTestLibPlugin

