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

#ifndef XMLQTESTLIBPARSERFACTORY_H
#define XMLQTESTLIBPARSERFACTORY_H

#include "baseqmakeqtestlibparserfactory.h"
#include "xmlqtestlibparser.h"

namespace QTestLibPlugin {
namespace Internal {

/*!
 * \brief The XMLQTestLibParserFactory class is a factory for XMLQTestLibParser
 *
 * As an implementation of AbstractTestParserFactory,
 * this class is in charge of checking that the associated parser, XMLQTestLibParser,
 * may parse the ProjectExplorer::RunConfiguration output and
 * allocate instances of the associated parser if needed.
 */
class XMLQTestLibParserFactory : public BaseQMakeQTestLibParserFactory
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
    inline XMLQTestLibParserFactory(QObject *parent = NULL):
        BaseQMakeQTestLibParserFactory(parent) {setAcceptedFormat(QTestLibArgsParser::XmlFormat);}
    AbstractTestParser* getParserInstance(ProjectExplorer::RunConfiguration *runConfiguration) const;
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // XMLQTESTLIBPARSERFACTORY_H
