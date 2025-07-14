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

#ifndef BASEXMLQTESTLIBPARSER_H
#define BASEXMLQTESTLIBPARSER_H

#include "testmodelfactory.h"
#include "qtestlibmodel.h"

#include <QStack>
#include <QMap>
#include <QXmlStreamReader>


namespace QTestLibPlugin {
namespace Internal {

/*!
 * \brief The BaseXMLQTestLibParser class implements a basic XML parser.
 *
 * This class proposes a basic parser for XML output from QTestLib. To be
 * functionnal it must be subclassed and the methods startElementParsed(),
 * endElementParsed(), textParsed() and commentParsed() must defined consistently.
 *
 * See parseStdoutLine() and parseStderrLine() for details on line parsing.
 */
class BaseXMLQTestLibParser : public AbstractTestParser
{
public:
    virtual inline ~BaseXMLQTestLibParser(void) override {delete mReader;}
    /*!
     * \brief \copybrief AbstractTestParser::parseStdoutLine()
     *
     * \copybrief AbstractTestParser::parseStdoutLine()
     *
     * This function ignores the lines preceeding
     * the elements of mXmlStarts as well
     * as lines after the end of document tag.
     *
     * Otherwise when a line of data is received,
     * it is fed to the XML stream reader. When the XML is correct,
     * the function returns TestModelFactory::Unsure,
     * otherwise TestModelFactory::MagicNotFound is returned.
     *
     * The magic is found after Environment data has been obtained.
     *
     * \param runControl The run control from which the line comes.
     * \param line The line of \c stdout.
     * \return A TestModelFactory::ParseResult as specified in the description.
     * \sa parseStderrLine()
     */
    TestModelFactory::ParseResult parseStdoutLine(ProjectExplorer::RunControl* runControl, const QString& line) override;
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
    TestModelFactory::ParseResult parseStderrLine(ProjectExplorer::RunControl* runControl, const QString& line) override;
    QAbstractItemModel *getModel(void) const override {return mModel;}
protected:
    /*!
     * \brief Constructor
     *
     * Calls the parent constructor and
     * allocates the XML stream reader.
     *
     * \param parent The parent object of the factory.
     */
    BaseXMLQTestLibParser(QObject *parent = NULL);

    /*!
     * \brief Handles beginning of elements
     *
     * This implementation does nothing and should be subclassed.
     *
     * \param runControl The run control from which the parsed line comes.
     * \param tag The tag of the current element.
     * \return TestModelFactory::Unsure in all cases
     */
    virtual inline TestModelFactory::ParseResult startElementParsed(ProjectExplorer::RunControl* runControl, const QStringView& tag) {Q_UNUSED(runControl); Q_UNUSED(tag); return TestModelFactory::Unsure;}
    /*!
     * \brief Handles end of elements
     *
     * This implementation does nothing and should be subclassed.
     *
     * \param runControl The run control from which the parsed line comes.
     * \param tag The tag of the currently closed element.
     * \return One of TestModelFactory::ParseResult as described hereinabove.
     */
    virtual inline TestModelFactory::ParseResult endElementParsed(ProjectExplorer::RunControl* runControl, const QStringView& tag) {Q_UNUSED(runControl); Q_UNUSED(tag); return TestModelFactory::Unsure;}
    /*!
     * \brief Handles CDATA sections
     *
     * This implementation does nothing and should be subclassed.
     *
     * \param runControl The run control from which the parsed line comes
     * \return TestModelFactory::Unsure in all cases
     */
    virtual inline TestModelFactory::ParseResult textParsed(ProjectExplorer::RunControl* runControl) {Q_UNUSED(runControl); return TestModelFactory::Unsure;}
    /*!
     * \brief Handles comments
     *
     * This implementation does nothing and should be subclassed.
     *
     * \param runControl The run control from which the parsed line comes
     * \return TestModelFactory::Unsure in all cases
     */
    virtual inline TestModelFactory::ParseResult commentParsed(ProjectExplorer::RunControl* runControl) {Q_UNUSED(runControl); return TestModelFactory::Unsure;}

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
     * \sa messageType()
     */
    QTestLibModel::MessageType currentMessageType(void);
    /*!
     * \brief Converts a QString into a message type
     *
     * Converts the given QString into a QTestLibModel::MessageType.
     *
     * \param messageType A QString representing a QTestLibModel::MessageType.
     * \return The type corresponding to the given QString.
     * \sa currentMessageType()
     */
    QTestLibModel::MessageType messageType(const QString& messageType);

    QTestLibModel *mModel; /*!< The model for the test output (it is \c NULL while TestModelFactory::MagicFound has not been returned by parseStdoutLine() or parseStderrLine()) */
    QXmlStreamReader *mReader; /*!< The XML stream reader used to parse XML */

    QStringList mXmlStarts; /*!< List of possible magic string which start XML the parser can understand */
    bool mParserActive; /*!< \c true when a test is being parsed */

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
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // BASEXMLQTESTLIBPARSER_H
