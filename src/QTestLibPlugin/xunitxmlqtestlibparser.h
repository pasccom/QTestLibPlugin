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

#ifndef XUNITXMLQTESTLIBPARSER_H
#define XUNITXMLQTESTLIBPARSER_H

#include "basexmlqtestlibparser.h"

namespace ProjectExplorer {
    class RunControl;
}

namespace QTestLibPlugin {
namespace Internal {

class QTestLibModel;
/*!
 * \brief The XUnitXMLQTestLibParser class implements an AbstractTestParser for XUnit XML QTestLib output.
 *
 * This class parses XUnit XML output from QTestLib and populates a QTestLibModel.
 * The populated model can be retrieved thanks to getModel() method.
 *
 * The preferred (and mandatory) way to instanciate this parser
 * is to use an instance of the associated factory class XUnitXMLQTestLibParserFactory.
 *
 * See startElementParsed(), endElementParsed() and commentParsed() for details on line parsing.
 */
class XUnitXMLQTestLibParser : public BaseXMLQTestLibParser
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
    inline XUnitXMLQTestLibParser(QObject *parent = NULL) :
        BaseXMLQTestLibParser(parent) {}
private:
    /*!
     * \brief \copybrief BaseXMLQTestLibParser::startElementParsed()
     *
     * Current token is the end of an element:
     *      - If the tag is \c property, then it stores the properties
     * in the class members.
     *      - If the tag is \c failure, then it adds the message to the model.
     *      - Otherwise, it stores the data in the class for future usage.
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
     *     - If tag is \c properties, check Qt version, Qt build and QTestLib version
     * and returns TestModelFactory::MagicFound or TestModelFactory::MagicNotFound in
     * accordance.
     *     - If this is the end of a \c testcase element and no result message has been output,
     * add an element to the model.
     *     - Otherwise, it clears the members of the class appropriately.
     *
     * \param runControl The run control from which the parsed line comes.
     * \param tag The tag of the currently closed element.
     * \return One of TestModelFactory::ParseResult as described hereinabove.
     */
    TestModelFactory::ParseResult endElementParsed(ProjectExplorer::RunControl* runControl, const QStringRef& tag);
    /*!
     * \brief \copybrief BaseXMLQTestLibParser::textParsed()
     *
     * Extracts the contents of comments which contain messages and add them to the model.
     *
     * \param runControl The run control from which the parsed line comes
     * \return TestModelFactory::Unsure in all cases
     */
    TestModelFactory::ParseResult commentParsed(ProjectExplorer::RunControl* runControl);

private:
    QString mCurrentClass; /*!< The name of the class currently parsed */
    QString mCurrentFunction; /*!< The name of the function currently parsed */
    QString mCurrentRow; /*!< The name of the data row currently parsed */
    QString mCurrentDescription; /*!< The description of the test currently parsed */
    QTestLibModel::MessageType mCurrentMessageType; /*!< The type of the current message */

    friend class XUnitXMLQTestLibParserFactory;
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // XUNITXMLQTESTLIBPARSER_H
