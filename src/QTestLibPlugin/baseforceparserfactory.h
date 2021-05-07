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

#ifndef BASEFORCEPARSERFACTORY_H
#define BASEFORCEPARSERFACTORY_H

#include "qtestlibpluginconstants.h"
#include "qtestlibargsparser.h"
#include "testoutputpane.h"

#include <utils/id.h>

namespace ProjectExplorer {
    class RunConfiguration;
}

namespace QTestLibPlugin {
namespace Internal {

/*!
 * \brief The BaseForceParserFactory class is a base for parsers factories relying user.
 *
 * As an implementation of AbstractTestParserFactory,
 * this class is in charge of checking that a parser
 * may parse the ProjectExplorer::RunConfiguration output.
 * It does not implement the parser creation method,
 * as it is not associated to any particular parser.
 * It is only intended to be injected into a parser factory.
 *
 * It does not use any smart heuristics; it just relies on the user,
 * who can thus force a parser. For that, it uses the combo box located
 * in the tool bar of TestOutputPane.
 */
class BaseForceParserFactory
{
public:
    /*!
     * \brief Constructor
     *
     * Just initalize members.
     *
     * \param format The test output format accepted by the parser
     * \param outputPane The TestOutputPane where the user can force the parsers
     */
    inline BaseForceParserFactory(QTestLibArgsParser::TestOutputFormat format, TestOutputPane *outputPane = nullptr):
        mOutputPane(outputPane), mFormat(format) {}
    /*!
     * \copydoc AbstractTestParserFactory::id()
     */
    inline Utils::Id id() const {return Utils::Id(Constants::BaseForceParserFactoryId);}
    /*!
     * \brief \copybrief AbstractTestParserFactory::canParse()
     *
     * Tests whether the project can be parsed by a parser with the given output format.
     * This factory just checks whether the user forced the parser in the TestOutputPane.
     * \param runConfiguration The run configuration to test (unused)
     * \return \c true, if the iuser forced the activation of the parser
     */
    bool canParse(ProjectExplorer::RunConfiguration *runConfiguration) const;
    /*!
     * \brief Set the format accepted by the parser.
     *
     * Set the format accepted by the parser.
     * \note Child classes must call these method in their constructors.
     * It defaults to QTestLibArgsParser::NoneFormat, which makes an invalid factory.
     * \param acceptedFormat The format accepted by the associated parser.
     */
    inline void setAcceptedFormat(QTestLibArgsParser::TestOutputFormat acceptedFormat) {mFormat = acceptedFormat;}
    /*!
     * \brief Set the output pane
     *
     * Sets the TestOutputPane where the user can force the parsers
     * \param outputPane The TestOutputPane where the user can force the parsers
     * \sa outputPane()
     */
    inline void setOutputPane(TestOutputPane* outputPane) {mOutputPane = outputPane;}
    /*!
     * \brief Get the output pane
     *
     * Get the TestOutputPane where the user can force the parsers
     * \return The TestOutputPane where the user can force the parsers
     * \sa setOutputPane()
     */
    inline TestOutputPane* outputPane(void) const {return mOutputPane;}
private:
    TestOutputPane* mOutputPane;                    /*!< The test output pane (where the user can force parsers). */
    QTestLibArgsParser::TestOutputFormat mFormat;   /*!< The test output format accepted by the associated parser. */
};

} // Internal
} // QTestLibPlugin

#endif // BASEFORCEPARSERFACTORY_H
