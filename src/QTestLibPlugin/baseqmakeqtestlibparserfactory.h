/* Copyright 2015-2016 Pascal COMBES <pascom@orange.fr>
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

#include "testmodelfactory.h"
#include "qtestlibargsparser.h"

namespace QTestLibPlugin {
namespace Internal {

/*!
 * \brief The BaseQMakeQTestLibParserFactory class is a base for parsers factories relying on QMake and QTestLib.
 *
 * As an implementation of AbstractTestParserFactory,
 * this class is in charge of checking that a parser
 * may parse the ProjectExplorer::RunConfiguration output.
 * It does not implement the parser cration method,
 * as it is not associated to any particular parser.
 * It is only intended to be injected into a parser factory.
 *
 * It proposes the following heuristic:
 *   \li Accept TestRunConfiguration with the accepted test output format.
 *   \li Accpet ProjectExplorer::LocalApplicationRunconfiguration associated
 * to a project using QTestLib and with the accepted test output format.
 *
 * These are sensible heuristics for most qMake projects.
 */
class BaseQMakeQTestLibParserFactory : public AbstractTestParserFactory
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * Just call the parent class constructor.
     *
     * \param format The test output format accepted by the parser
     * \param parent The parent object of the factory
     */
    inline BaseQMakeQTestLibParserFactory(QTestLibArgsParser::TestOutputFormat format, QObject* parent = nullptr):
        AbstractTestParserFactory(parent), mFormat(format) {}
    /*!
     * \brief \copybrief AbstractTestParserFactory::canParse()
     *
     * Tests whether the project can be parsed by a parser with the given output format.
     * It uses the following heuristics:
     *  \li Accept TestRunConfiguration with the accepted test output format.
     *  \li Accpet ProjectExplorer::LocalApplicationRunconfiguration associated
     * to a project using QTestLib and with the accepted test output format.
     *
     * \param runConfiguration The run configuration to test.
     * \return \c true, if the associated parser may parse the test output.
     */
    inline bool canParse(ProjectExplorer::RunConfiguration *runConfiguration) const {return canParseRunConfiguration(runConfiguration) || canParseModule(runConfiguration);}
    /*!
     * \brief \copybrief AbstractTestParserFactory::getParserInstance()
     *
     * Always returns \c nullptr, since this factory is a base (not associated with any parser).
     * \param runConfiguration Unused.
     * \return Always \c nullptr.
     */
    inline AbstractTestParser* getParserInstance(ProjectExplorer::RunConfiguration* runConfiguration) const {Q_UNUSED(runConfiguration); return nullptr;}
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
     * \brief Check if the run configuration is a TestRunConfiguration.
     *
     * If the run configuration is a TestRunConfiguration,
     * then the associated parser may parse the output
     * if the arguments do not change the outut format.
     *
     * \param runConfiguration The run configuration to check.
     * \return \c true, if the parser may parse the test
     * \sa canParseModule(), canParseArguments()
     */
    bool canParseRunConfiguration(ProjectExplorer::RunConfiguration* runConfiguration) const;
    /*!
     * \brief Check if the project uses \c testlib Qt module.
     *
     * As the associated parser can only parse QTestLib output,
     * this method check that the \c testlib module is used by the project.
     * This check should avoid parsing almost all undesired projects.
     *
     * \param runConfiguration The run configuration to check.
     * \return \c true, if the parser may parse the test.
     * \sa canParseRunConfiguration(), canParseArguments()
     */
    bool canParseModule(ProjectExplorer::RunConfiguration *runConfiguration) const;
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
