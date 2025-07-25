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

#ifndef BASEQMAKEQTESTLIBPARSERFACTORY_H
#define BASEQMAKEQTESTLIBPARSERFACTORY_H

#include "qtestlibpluginconstants.h"
#include "qtestlibargsparser.h"

#include <utils/id.h>

namespace ProjectExplorer {
    class RunControl;
}

namespace QTestLibPlugin {
namespace Internal {

/*!
 * \brief The BaseQMakeQTestLibParserFactory class is a base for parsers factories relying on QMake and QTestLib.
 *
 * As an implementation of AbstractTestParserFactory,
 * this class is in charge of checking that a parser
 * may parse the ProjectExplorer::RunControl output.
 * It does not implement the parser creation method,
 * as it is not associated to any particular parser.
 * It is only intended to be injected into a parser factory.
 *
 * It proposes the following heuristic:
 *   \li Accept TestRunConfiguration with the accepted test output format.
 *   \li Accept ProjectExplorer::LocalApplicationRunconfiguration associated
 * to a project using QTestLib and with the accepted test output format.
 *
 * These are sensible heuristics for most qMake projects.
 */
class BaseQMakeQTestLibParserFactory
{
public:
    /*!
     * \brief Constructor
     *
     * Just initialize the members.
     * \param format The test output format accepted by the parser
     */
    inline BaseQMakeQTestLibParserFactory(QTestLibArgsParser::TestOutputFormat format):
        mFormat(format) {}
    /*!
     * \copydoc AbstractTestParserFactory::id()
     */
    inline Utils::Id id() const {return Utils::Id(Constants::BaseQMakeQTestLibParserFactoryId);}
    /*!
     * \brief \copybrief AbstractTestParserFactory::canParse()
     *
     * Tests whether the project can be parsed by a parser with the given output format.
     * It uses the following heuristics:
     *  \li Accept TestRunConfiguration with the accepted test output format.
     *  \li Accept ProjectExplorer::LocalApplicationRunconfiguration associated
     * to a project using QTestLib and with the accepted test output format.
     *
     * \param runControl The run control to test.
     * \return \c true, if the associated parser may parse the test output.
     */
    inline bool canParse(ProjectExplorer::RunControl* runControl) const {return canParseRunConfiguration(runControl) || canParseModule(runControl);}
    /*!
     * \brief Set the format accepted by the parser.
     *
     * Set the format accepted by the parser.
     * \note Child classes must call these method in their constructors.
     * It defaults to QTestLibArgsParser::NoneFormat, which makes an invalid factory.
     * \param acceptedFormat The format accepted by the associated parser.
     */
    inline void setAcceptedFormat(QTestLibArgsParser::TestOutputFormat acceptedFormat) {mFormat = acceptedFormat;}
private:
    /*!
     * \brief Check if the run control is from a TestRunConfiguration.
     *
     * If the run control was created from a TestRunConfiguration,
     * then the associated parser may parse the output
     * if the arguments do not change the outut format.
     *
     * \param runControl The run control to check.
     * \return \c true, if the parser may parse the test
     * \sa canParseModule(), canParseArguments()
     */
    bool canParseRunConfiguration(ProjectExplorer::RunControl* runControl) const;
    /*!
     * \brief Check if the project uses \c testlib Qt module.
     *
     * As the associated parser can only parse QTestLib output,
     * this method check that the \c testlib module is used by the project.
     * This check should avoid parsing almost all undesired projects.
     *
     * \param runControl The run control to check.
     * \return \c true, if the parser may parse the test.
     * \sa canParseRunConfiguration(), canParseArguments()
     */
    bool canParseModule(ProjectExplorer::RunControl* runControl) const;
    /*!
     * \brief Check the command line arguments
     *
     * Once the \c testlib module is found, the arguments are checked
     * to see whether the output may be plain text.
     *
     * \param cmdArgs The command line arguments from the run configuration.
     * \return \c true, if the parser may parse the test.
     * \sa canParseRunConfiguration(), canParseModule()
     */
    bool canParseArguments(const QString& cmdArgs) const;

    QTestLibArgsParser::TestOutputFormat mFormat; /*!< The test output format accepted by the associated parser. */
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // BASEQMAKEQTESTLIBPARSERFACTORY_H
