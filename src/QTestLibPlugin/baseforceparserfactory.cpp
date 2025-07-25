/* Copyright 2016-2020 Pascal COMBES <pascom@orange.fr>
 * 
 * This file is part of QTestLibPlugin.
 * 
 * QTestLibPlugin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * QTestLibPlugin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with QTestLibPlugin. If not, see <http://www.gnu.org/licenses/>
 */

#include "baseforceparserfactory.h"

#include "utils/qtcassert.h"

namespace QTestLibPlugin {
namespace Internal {

bool BaseForceParserFactory::canParse(ProjectExplorer::RunControl* runControl) const
{
    Q_UNUSED(runControl);

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
