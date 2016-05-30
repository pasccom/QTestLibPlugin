#include "baseforceparserfactory.h"

#include "utils/qtcassert.h"

namespace QTestLibPlugin {
namespace Internal {

bool BaseForceParserFactory::canParse(ProjectExplorer::RunConfiguration *runConfiguration) const
{
    Q_UNUSED(runConfiguration);

    switch (mOutputPane->userForceParser()) {
    case QTestLibArgsParser::NoneFormat:
        return false;
    case QTestLibArgsParser::TxtFormat:
        return (mFormat == QTestLibArgsParser::TxtFormat);
    case QTestLibArgsParser::XmlFormat:
        return (mFormat == QTestLibArgsParser::XmlFormat);
    case QTestLibArgsParser::LightXmlFormat:
        return (mFormat == QTestLibArgsParser::LightXmlFormat);
    case QTestLibArgsParser::XUnitXmlFormat:
        return (mFormat == QTestLibArgsParser::XUnitXmlFormat);
    case QTestLibArgsParser::AllFormats:
        return true;
    default:
        break;
    }

    QTC_ASSERT(false, return false);
}

} // Internal
} // QTestLibPlugin
