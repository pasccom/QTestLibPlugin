/* Copyright 2015 Pascal COMBES <pascom@orange.fr>
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

#ifndef XUNITXMLQTESTLIBPARSERFACTORY_H
#define XUNITXMLQTESTLIBPARSERFACTORY_H

#include "baseqmakeqtestlibparserfactory.h"
#include "xunitxmlqtestlibparser.h"

namespace QTestLibPlugin {
namespace Internal {

/*!
 * \brief The XUnitXMLQTestLibParserFactory class is a factory for XUnitXMLQTestLibParser
 *
 * As an implementation of AbstractTestParserFactory,
 * this class is in charge of checking that the associated parser, XUnitXMLQTestLibParser,
 * may parse the ProjectExplorer::RunConfiguration output and
 * allocate instances of the associated parser if needed.
 */
class XUnitXMLQTestLibParserFactory : public BaseQMakeQTestLibParserFactory
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * Just call the parent class constructor.
     *
     * \param parent The parent object of the factory.
     */
    inline XUnitXMLQTestLibParserFactory(QObject *parent = NULL):
        BaseQMakeQTestLibParserFactory(parent) {setAcceptedFormat(QTestLibArgsParser::XUnitXmlFormat);}
    AbstractTestParser* getParserInstance(ProjectExplorer::RunConfiguration *runConfiguration) const;
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // XUNITXMLQTESTLIBPARSERFACTORY_H
