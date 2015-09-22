#include "plaintextqtestlibparserfactory.h"

namespace QTestLibPlugin {
namespace Internal {

bool PlainTextQTestLibParserFactory::canParse(ProjectExplorer::RunConfiguration *runConfiguration) const
{
    Q_ASSERT(runConfiguration != NULL);

    return true;
}

} // namespace Internal
} // namespace QTestLibPlugin
