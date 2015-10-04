#include "xmlqtestlibparserfactory.h"

namespace QTestLibPlugin {
namespace Internal {

bool XMLQTestLibParserFactory::canParse(ProjectExplorer::RunConfiguration *runConfiguration) const
{
    Q_ASSERT(runConfiguration != NULL);

    return true;
}

} // namespace Internal
} // namespace QTestLibPlugin
