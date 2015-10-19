#ifndef TESTMODELFACTORY_H
#define TESTMODELFACTORY_H

#include <QObject>
#include <QLinkedList>
#include <utils/outputformat.h>

class QAbstractItemModel;

namespace ProjectExplorer {
    class RunControl;
    class RunConfiguration;
}

namespace QTestLibPlugin {
namespace Internal {


class AbstractTestParser;

/*!
 * \brief The TestModelFactory class is a factory for test models.
 *
 * This class is in charge of allocating and populating a QAbstractItemModel.
 * The model items represent the class, the functions and the data items used by test.
 *
 * This class determines automatically the suitable test parser (if any)
 * for the program run by relying on AbstractTestParserFactory::canParse() method.
 * Many tests parsers may be suitable; only the first parser returning
 * TestModelFactory::MagicFound will be kept; the others are discarded.
 *
 * \sa AbstractTestParser, AbstractTestParserFactory
 */
class TestModelFactory : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * Constructs a new factory with the given parent for the given ProjectExplorer::RunControl
     *
     * \param runControl The run control whose output will be parsed.
     * \param parent The parent object.
     */
    TestModelFactory(ProjectExplorer::RunControl *runControl, QObject *parent = NULL);
    /*!
     * \typedef ParseResult
     * \brief Result of the parsing of a line of \c stdout of \c stderr.
     *
     * The values of this enums are used by the parsers (see AbstractTestParser) to tell
     * whether they are sure they can parse the test output or not.
     */
    typedef enum {
        Unsure, /*!< The parser is unsure whether it can parse this test. */
        MagicFound, /*!< The parser is sure it can parse this test. */
        MagicNotFound /*!< The parser is sure it cannot parse this test. */
    } ParseResult; // WARNING order matters !
signals:
    /*!
     * \brief Emitted when a model has been found.
     *
     * This signal is emmitted as soon as a parser returns MagicFound.
     *
     * \param model A pointer the the newly allocated model.
     * \sa modelPopulated()
     */
    void modelFound(QAbstractItemModel *model);
    /*!
     * \brief Emitted when a model is fully populated.
     *
     * This signal is emitted when the run control finishes.
     *
     * \param model A pointer the the fully poplated model.
     * \sa modelFound()
     */
    void modelPopulated(QAbstractItemModel *model);
private slots:
    /*!
     * \internal
     * \brief Parses run control output.
     *
     * This slots receives the output from the ProjectExplorer::RunControl being run.
     * Only the formats Utils::OutputFormat::StdOutFormat and
     * Utils::OutputFormat::StdOutFormatSameLine are currently used.
     *
     * \param runControl The run control from which this message originates.
     * \param msg The message
     * \param format The format
     */
    void parseTestOutput(ProjectExplorer::RunControl* runControl, const QString& msg, Utils::OutputFormat format);
    /*!
     * \internal
     * \brief Called when a run control finishes.
     *
     * Thanks to this slot, the factory is notified of the end of the currently run ProjectExplorer::RunControl.
     * It emits the signal modelPopulated() if a model was found and marks the factory for deletion.
     */
    void runControlFinished(void);
private:
    /*!
     * \internal
     * \brief Pass the line to known parsers.
     *
     * It calls the known parsers methods AbstractTestParser::parseStdoutLine().
     *
     * \param runControl The run control from which this message originates.
     * \param msg The message
     */
    void parseStdoutLine(ProjectExplorer::RunControl* runControl, const QString& line);
    QLinkedList<AbstractTestParser *> mParsers; /*!<  \internal The list of available parsers (parsers are removed when they return MagicNotFound) */
    bool mModelFound; /*!< \internal Whether a parser succeeded in finding a model (i.e. returned MagicFound) */
};

/*!
 * \brief The AbstractTestParser class is an abstract base for test parsers.
 *
 * This class defines the function a test parser is supposed to implement.
 *
 * The implementations of this class will be in charge of allocating
 * and populating a model associated with the test run.
 *
 * \sa TestModelFactory
 */
class AbstractTestParser : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Parses a line of \c stdout
     *
     * Parses a line of \c stdout coming from the given ProjectExplorer::RunControl.
     *
     * If this line (and optionally the previously parsed ones) contains enough information
     * for the parser to be sure it understands the format, it should return TestModelFactory::MagicFound.
     *
     * On the opposite, if this line (and optionally the previously parsed ones) contains enough information
     * for the parser to be sure it does not understands the format, it should return TestModelFactory::MagicNotFound.
     *
     * Otherwise it returns TestModelFactory::Unsure.
     *
     * \param runControl The run control from which the line comes.
     * \param line The line of \c stdout.
     * \return A TestModelFactory::ParseResult as specified in the description.
     * \sa parseStderrLine()
     */
    virtual TestModelFactory::ParseResult parseStdoutLine(ProjectExplorer::RunControl* runControl, const QString& line) = 0;
    /*!
     * \brief Parses a line of \c stderr
     *
     * Parses a line of \c stderr coming from the given ProjectExplorer::RunControl.
     *
     * If this line (and optionally the previously parsed ones) contains enough information
     * for the parser to be sure it understands the format, it should return TestModelFactory::MagicFound.
     *
     * On the opposite, if this line (and optionally the previously parsed ones) contains enough information
     * for the parser to be sure it does not understands the format, it should return TestModelFactory::MagicNotFound.
     *
     * Otherwise it returns TestModelFactory::Unsure.
     *
     * \param runControl The run control from which the line comes.
     * \param line The line of \c stderr.
     * \return A TestModelFactory::ParseResult as specified in the description.
     * \sa parseStdoutLine()
     */
    virtual TestModelFactory::ParseResult parseStderrLine(ProjectExplorer::RunControl* runControl, const QString& line) = 0;
    /*!
     * \brief Returns the (partially) populated model.
     *
     * Returns the model the parser populated.
     * Before parseStdoutLine() or parseStderrLine() returned TestModelFactory::MagicFound,
     * this function will always returns \c NULL.
     * \note The model may not be fully populated, if the test run is not finished.
     *
     * \return The model (partially) populated by the parser,
     * or \c NULL if the parser is not sure it understands the test output.
     */
    virtual QAbstractItemModel *getModel(void) const = 0;
protected:
    /*!
     * \internal
     * \brief Constructor
     * \param parent The parent of the parser.
     */
    AbstractTestParser(QObject *parent = NULL) :
        QObject(parent) {}
};

/*!
 * \brief The AbstractTestParserFactory class is an abstract base for factory for classes implementing AbstractTestParser.
 *
 * The implementaions of this class is in charge of allocating instances of the associated parser (implementing AbstractTestParser)/
 * It first tests whether the parser may parse the test output by checking the test run configuration.
 * See the implementations for the criteria on which rely these checks.
 *
 * \sa TestModelFactory
 */
class AbstractTestParserFactory : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Tests whether the associated parser can be useful.
     *
     * Tells whether the associated parser can potentially parse the test associated with the given ProjectExplorer::RunConfiguration.
     *
     * \param runConfiguration The run configuration to test.
     * \return \c true, if the associated parser may parse the test output.
     */
    virtual bool canParse(ProjectExplorer::RunConfiguration *runConfiguration) const = 0;
    /*!
     * \brief Returns an associated parser instance.
     *
     * Allocates a new associated parser instance, with the given ProjectExplorer::RunConfiguration as parent.
     * The parser is thus deallocated when the ProjectExplorer::RunConfiguration is (but you can do this manually before).
     * \note If canParse() returns \c false, this function retuns \c NULL.
     *
     * \param runConfiguration The run configuation the parser will be run against.
     * \return An instance of the associated test output parser
     * or \c NULL if the parser can not possibly parse the test output.
     */
    virtual AbstractTestParser* getParserInstance(ProjectExplorer::RunConfiguration *runConfiguration) const = 0;
protected:
    /*!
     * \internal
     * \brief Constructor
     * \param parent The parent of the parser factory.
     */
    AbstractTestParserFactory(QObject *parent = NULL) :
        QObject(parent) {}
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // TESTMODELFACTORY_H
