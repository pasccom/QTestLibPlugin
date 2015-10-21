#ifndef PLAINTEXTQTESTLIBPARSER_H
#define PLAINTEXTQTESTLIBPARSER_H

#include "testmodelfactory.h"
#include "qtestlibmodel.h"

namespace ProjectExplorer {
    class RunControl;
}

namespace QTestLibPlugin {
namespace Internal {

class QTestLibModel;
/*!
 * \brief The PlainTextQTestLibParser class implements an AbstractTestParser for plain text QTestLib output.
 *
 * This class parses plain text QTestLib output and populates a QTestLibModel.
 * The populated model can be retrieved thanks to getModel() method.
 *
 * The preferred (and mandatory) way to instanciate this parser
 * is to use an instance of the associated factory class PlainTextQTestLibParserFactory.
 *
 * \note For extreme cases (involving the output of QTestLib output in debug messages),
 * the parser may fail and extract unconsistent information. With plain text, this problem
 * is unfortunately not decidable. In such cases, you should resort to the XMLQTestLibParser.
 *
 * See parseStdoutLine() and parseStderrLine() for details on line parsing.
 */
class PlainTextQTestLibParser : public AbstractTestParser
{
    Q_OBJECT
public:
    /*!
     * \brief \copybrief AbstractTestParser::parseStdoutLine()
     *
     * \copybrief AbstractTestParser::parseStdoutLine()
     *
     * Two types of lines are possible:
     *     1. The beginning of a line of QTestLib output.
     *     2. The continuation of a line of QTestLib output.
     *     3. The location of a test event.
     *
     * Locations are extracted using a simple regular expression.
     *
     * To choose between the two remaining types, this function uses isMessageBeginning().
     * If it returns \c true, then the information contained in the line is parsed
     * using processMessageBeginning().
     *
     * If one of the methods retuns \c false, then this line is considered as the
     * continuation of the previous line and the information is added to the model
     * using QTestLibModel::appendTestItemMessage().
     *
     * This parser never returns TestModelFactory::MagicNotFound
     * as other lines may be output before the beginning of the test.
     *
     * The magic is implemented as two regular expressions (one for silent mode)
     * and the other for verboses modes. They can be seen at any moment in the test execution.
     *
     * Lines before the first magic are ignored as well as lines after a result magic.
     * However the parser can restart if a line with a begin magic occurs after.
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
     * As now plain text QTestLib output is all done in \c stdout.
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
     * Just call the parent class constructor.
     *
     * \param parent The parent object of the factory.
     */
    PlainTextQTestLibParser(QObject *parent = 0) :
        AbstractTestParser(parent), mModel(NULL) {}
private:
    /*!
     * \internal
     * \brief Tells whether a line is the beginning of a line of test output.
     *
     * Checks whether a line is the beginning of a line of test output and
     * extract the type of the message.
     * It justs checks that
     *     1. The eighth character in the string is a colon \c :
     *     2. The text before the colon is one of the know message type.
     * This may noy be robust enough, as debug information may create such information.
     *
     * \param line The line of test output to be parsed
     * \param type A pointer to an emplacement where the parsed type will be stored.
     * \return \c true if the above conditions are checked.
     * \sa processMessageBeginning()
     */
    bool isMessageBeginning(const QString& line, QTestLibModel::MessageType *type = NULL);
    /*!
     * \internal
     * \brief Process the beginning of a line of test output.
     *
     * This function extracts the information contained in a line of test output
     * stripped from the type of the message and insert it in the model using
     * QTestLibModel::addTestItem().
     * The following format is assumed:
     * \code
     *     ClassName::FunctionName(DataRowTitle)[: ?]
     * \endcode
     *
     * \param runControl The run control from which the line originates
     * \param line The line of test output to be parsed
     * \param type The type of the message (found by isMessageBeginning())
     * \return \c true if the line matches the specifications.
     * \sa isMessageBeginning(), parseStdoutLine()
     */
    bool processMessageBeginning(ProjectExplorer::RunControl* runControl, const QString& line, QTestLibModel::MessageType type);
    /*!
     * \internal
     * \brief Match the brackets.
     *
     * This function returns the position of the closing bracket
     * corresponding to the one opened at the given position.
     * This function supports nesting of brackets.
     *
     * \note If there is no bracket at the given index, this function
     * behaves as if there was one.
     *
     * \param line The line of text in which brackets are matched
     * \param b The index of the (possibly fictive) opening bracket
     * \return The index of the corresponding closin bracket or
     * the index after the end index for the string.
     */
    int matchBracket(const QString& line, int b) const;

    QTestLibModel *mModel; /*!< \internal The model for the test output (it is \c NULL while TestModelFactory::MagicFound has not been returned by parseStdoutLine() or parseStderrLine()) */
    bool mParserActive; /*!< \internal \c true when a test is being parsed */

    friend class PlainTextQTestLibParserFactory;
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // PLAINTEXTQTESTLIBPARSER_H
