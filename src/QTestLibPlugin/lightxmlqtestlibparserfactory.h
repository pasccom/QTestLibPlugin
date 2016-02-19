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

#ifndef LIGHTXMLQTESTLIBPARSERFACTORY_H
#define LIGHTXMLQTESTLIBPARSERFACTORY_H

#include "testmodelfactory.h"
#include "lightxmlqtestlibparser.h"

namespace QTestLibPlugin {
namespace Internal {

/*!
 * \brief The LightXMLQTestLibParserFactory class is a factory for LightXMLQTestLibParser
 *
 * As an implementation of AbstractTestParserFactory,
 * this class is in charge of checking that the associated parser, LightXMLQTestLibParser,
 * may parse the ProjectExplorer::RunConfiguration output and
 * allocate instances of the associated parser if needed.
 */
class LightXMLQTestLibParserFactory : public AbstractTestParserFactory
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
    inline LightXMLQTestLibParserFactory(QObject *parent = NULL):
        AbstractTestParserFactory(parent) {}
    inline bool canParse(ProjectExplorer::RunConfiguration *runConfiguration) const {return canParseModule(runConfiguration);}
    AbstractTestParser* getParserInstance(ProjectExplorer::RunConfiguration *runConfiguration) const;
private:
    /*!
     * \brief Check if the project uses \c testlib Qt module.
     *
     * As the associated parser can only parse QTestLib output,
     * this method check that the \c testlib module is used by the project.
     * This check should avoid parsing almost all undesired projects.
     *
     * \param runConfiguration The run configuration to check.
     * \return \c true, if the parser may parse the test.
     */
    bool canParseModule(ProjectExplorer::RunConfiguration *runConfiguration) const;
    /*!
     * \brief Check the command line arguments
     *
     * Once the \c testlib module is found, the arguments are checked
     * to see whether the output may be Light XML.
     *
     * \param runConfiguration The run configuration to check.
     * \return \c true, if the parser may parse the test.
     */
    bool canParseArguments(ProjectExplorer::RunConfiguration *runConfiguration) const;
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // LIGHTXMLQTESTLIBPARSERFACTORY_H
