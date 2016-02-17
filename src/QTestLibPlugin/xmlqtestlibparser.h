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

#ifndef XMLQTESTLIBPARSER_H
#define XMLQTESTLIBPARSER_H

#include "basexmlqtestlibparser.h"

namespace ProjectExplorer {
    class RunControl;
}

namespace QTestLibPlugin {
namespace Internal {

class QTestLibModel;
/*!
 * \brief The XMLQTestLibParser class implements an AbstractTestParser for XML QTestLib output.
 *
 * This class parses XML QTestLib output and populates a QTestLibModel.
 * The populated model can be retrieved thanks to getModel() method.
 *
 * The preferred (and mandatory) way to instanciate this parser
 * is to use an instance of the associated factory class XMLQTestLibParserFactory.
 *
 * See startElementParsed(), endElementParsed() and textParsed() for details on line parsing.
 */
class XMLQTestLibParser : public BaseXMLQTestLibParser
{
    Q_OBJECT
protected:
    /*!
     * \brief Constructor
     *
     * Calls the parent constructor.
     *
     * \param parent The parent object of the factory.
     */
    inline XMLQTestLibParser(QObject *parent = NULL) :
        BaseXMLQTestLibParser(parent) {}
    /*!
     * \brief \copybrief BaseXMLQTestLibParser::startElementParsed()
     *
     * Current token is the beginning of an element:
     * Store the element tag in the current element stack
     * and its attributes in the attributes map.
     *
     * \param runControl The run control from which the parsed line comes.
     * \param tag The tag of the current element.
     * \return TestModelFactory::Unsure in all cases
     */
    TestModelFactory::ParseResult startElementParsed(ProjectExplorer::RunControl* runControl, const QStringRef& tag);
    /*!
     * \brief \copybrief BaseXMLQTestLibParser::endElementParsed()
     *
     * Current token is the end of an element:
     *     - If tag is \c Environment, check Qt version, Qt build and QTestLib version
     * and returns TestModelFactory::MagicFound or TestModelFactory::MagicNotFound in
     * accordance.
     *     - Otherwise add elements to the model.
     *
     * \param runControl The run control from which the parsed line comes.
     * \param tag The tag of the currently closed element.
     * \return One of TestModelFactory::ParseResult as described hereinabove.
     */
    TestModelFactory::ParseResult endElementParsed(ProjectExplorer::RunControl* runControl, const QStringRef& tag);
    /*!
     * \brief \copybrief BaseXMLQTestLibParser::textParsed()
     *
     * Extracts the contents of CDATA sections which contains the messages.
     *
     * \param runControl The run control from which the parsed line comes
     * \return TestModelFactory::Unsure in all cases
     */
    TestModelFactory::ParseResult textParsed(ProjectExplorer::RunControl* runControl);
    /*!
     * \brief Saves the attributes
     *
     * Stores the given attributes in the attributes map
     *
     * \param attrs The attributes to save in the attributes map
     */

private:
    QString mCurrentClass; /*!< The name of the class currently parsed */
    QString mCurrentFunction; /*!< The name of the function currently parsed */
    QString mCurrentRow; /*!< The name of the data row currently parsed */
    QString mCurrentDescription; /*!< The description of the test currently parsed */

    friend class XMLQTestLibParserFactory;
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // XMLQTESTLIBPARSER_H
