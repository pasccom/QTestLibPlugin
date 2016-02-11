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

#ifndef XUNITXMLQTESTLIBPARSER_H
#define XUNITXMLQTESTLIBPARSER_H

#include "testmodelfactory.h"
#include "qtestlibmodel.h"

#include <QStack>
#include <QMap>
#include <QXmlStreamReader>

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
 * See parseStdoutLine() and parseStderrLine() for details on line parsing.
 */
class XUnitXMLQTestLibParser : public AbstractTestParser
{
    Q_OBJECT
public:
    /*!
     * \brief Destructor.
     *
     * Deletes the XML stream reader allocated in the constructor.
     */
    inline ~XUnitXMLQTestLibParser(void) {delete mReader;}
    /*!
     * \brief \copybrief AbstractTestParser::parseStdoutLine()
     *
     * \copybrief AbstractTestParser::parseStdoutLine()
     *
     * This function ignores the lines preceeding
     * the <tt><?xml ... ?></tt> tag as well
     * as lines after the end of document tag.
     *
     * Otherwise when a line of data is received,
     * it is fed to the XML stream reader. When the XML is correct,
     * the function returns TestModelFactory::Unsure,
     * otherwise TestModelFactory::MagicNotFound is returned.
     *
     * The magic is found after Environment tag has been parsed.
     *
     * \param runControl The run control from which the line comes.
     * \param line The line of \c stdout.
     * \return A TestModelFactory::ParseResult as specified in the description.
     * \sa parseStderrLine()
     */
    TestModelFactory::ParseResult parseStdoutLine(ProjectExplorer::RunControl* runControl, const QString& line);
    /*!
     * \brief \copybrief AbstractTestParser::parseStderrLine()
     *
     * \copybrief AbstractTestParser::parseStderrLine()
     *
     * As now XML QTestLib output is all done in \c stdout.
     * Consequently messages are just ignored and this functions
     * returns always TestModelFactory::Unsure.
     *
     * \param runControl The run control from which the line comes.
     * \param line The line of \c stderr.
     * \return A TestModelFactory::ParseResult as specified in the description.
     * \sa parseStdoutLine()
     */
    TestModelFactory::ParseResult parseStderrLine(ProjectExplorer::RunControl* runControl, const QString& line);
    QAbstractItemModel *getModel(void) const {return mModel;}
protected:
    /*!
     * \brief Constructor
     *
     * Calls the parent constructor and
     * allocates the XML stream reader.
     *
     * \param parent The parent object of the factory.
     */
    XUnitXMLQTestLibParser(QObject *parent = NULL);
private:
    /*!
     * \brief Handles beginning of elements
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
     * \brief Handles end of elements
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
     * \brief Handles CDATA sections
     *
     * Extracts the contents of CDATA sections.
     *
     * \param runControl The run control from which the parsed line comes
     * \return TestModelFactory::Unsure in all cases
     */
    //TestModelFactory::ParseResult textParsed(ProjectExplorer::RunControl* runControl);
    /*!
     * \brief Handles comments
     *
     * Extracts the contents of comments.
     *
     * \param runControl The run control from which the parsed line comes
     * \return TestModelFactory::Unsure in all cases
     */
    TestModelFactory::ParseResult commentParsed(ProjectExplorer::RunControl* runControl);
    /*!
     * \brief Saves the attributes
     *
     * Stores the given attributes in the attributes map
     *
     * \param attrs The attributes to save in the attributes map
     */
    void saveAttributes(const QXmlStreamAttributes& attrs);
    /*!
     * \brief Gives the message type
     *
     * Converts the type attribute of the current tag
     * into a QTestLibModel::MessageType.
     *
     * \return The type corresponding to the type attribute of the current tag
     */
    QTestLibModel::MessageType currentMessageType(void);
    /*!
     * \brief Converts a QString into a message type
     *
     * Converts the given QString into a QTestLibModel::MessageType.
     *
     * \return The type corresponding to the given QString
     */
    QTestLibModel::MessageType messageType(const QString& messageType);

    QTestLibModel *mModel; /*!< The model for the test output (it is \c NULL while TestModelFactory::MagicFound has not been returned by parseStdoutLine() or parseStderrLine()) */
    QXmlStreamReader *mReader; /*!< The XML stream reader used to parse XML */
    bool mParserActive; /*!< \c true when a test is being parsed */

    QString mCurrentClass; /*!< The name of the class currently parsed */
    QString mCurrentFunction; /*!< The name of the function currently parsed */
    QString mCurrentRow; /*!< The name of the data row currently parsed */
    QString mCurrentDescription; /*!< The description of the test currently parsed */
    QTestLibModel::MessageType mCurrentMessageType; /*!< The type of the current message */

    QStack<QString> mCurrentElement; /*!< A stack for the names of elements currently parsed */
    QMap<QString, QString> mCurrentAttributes; /*!< A map which stores the attributes of the current element */

    QString mQtVersion; /*!< The version of Qt used by the test */
    QString mQtBuild;  /*!< Build information corresponding to the version of Qt used in the test */
    QString mQTestLibVersion;  /*!< The version of QTestLib used by the test */

    /*!
     * \brief Initialize XML entities table
     *
     * Initializes the XML entities tables which is used by decodeXMLEntities().
     */
    static void initXMLEntities(void);
    /*!
     * \brief Decodes XML entities in a QString.
     *
     * Replaces XML entities by corresponding unicode characters in a QString.
     * \param encoded An XML encoded QString
     * \return A QString in which the XML entities were decoded.
     */
    static QString decodeXMLEntities(const QString& encoded);

    static QHash<QString, QString> sEntities; /*!< The map of known XML entities */
    friend class XUnitXMLQTestLibParserFactory;
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // XUNITXMLQTESTLIBPARSER_H
