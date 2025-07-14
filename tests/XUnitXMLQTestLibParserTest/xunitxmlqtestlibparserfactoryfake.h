/* Copyright 2015-2020 Pascal COMBES <pascom@orange.fr>
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

#ifndef XUNITXMLQTESTLIBPARSERFACTORYFAKE_H
#define XUNITXMLQTESTLIBPARSERFACTORYFAKE_H

#include <testmodelfactory.h>
#include <xunitxmlqtestlibparser.h>

#include <projectexplorer/runcontrol.h>

class Fake {};

namespace QTestLibPlugin {
namespace Internal {

template<class B>
class XUnitXMLQTestLibParserFactory : public AbstractTestParserFactory
{
public:
    inline XUnitXMLQTestLibParserFactory(void):
        AbstractTestParserFactory() {}
    inline bool canParse(ProjectExplorer::RunControl* runControl) const {Q_UNUSED(runControl); return true;}
    inline AbstractTestParser* getParserInstance(ProjectExplorer::RunControl* runControl) const {return new XUnitXMLQTestLibParser(runControl);}
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // XUNITXMLQTESTLIBPARSERFACTORYFAKE_H
