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

#ifndef QTESTLIBARGSPARSER_H
#define QTESTLIBARGSPARSER_H

#include <utils/fileutils.h>
#include <QString>
#include <QStringList>
#include <QPair>
#include <QLinkedList>

namespace QTestLibPlugin {
namespace Internal {

/*!
 * \brief A list of test data rows
 *
 * Type for a list of test data rows.
 * Each item in this list is a row of data in the test.
 * If the list is empty, then all test data rows are selected.
 */
 class TestRowList : public QStringList
 {
 public:
     operator QString () const;
 };

/*!
 * \brief A list of test cases.
 *
 * Type for a list of test cases.
 * The name of the test case can be accessed with \c first() and the
 * selected data rows are accessed with \c second().
 * If the list is empty, then all test cases are selected and
 * if \c second() is empty then all data rows are selected.
 */
class TestCaseList : public QLinkedList< QPair<QString, TestRowList> >
{
public:
    operator QString () const;
};

/*!
 * \brief A list of test classes.
 *
 * Type for a list of test classes.
 * The name of the test class can be accessed with \c first() and the
 * selected test cases are accessed with \c second().
 * If the list is empty, then all test classes are selected and
 * if \c second() is empty then all test cases are selected.
 */
class TestClassList : public QLinkedList< QPair<QString, TestCaseList> >
{
public:
    operator QString () const;
};

/*!
 * \brief A parser for QTestLib argument lists.
 *
 * The QTestLibArgsParser class implements a parser for QTestLib command line arguments list.
 * The arguments are parsed directly in the constructor and then the information
 * is directly available with the public methods of the class.
 *
 * Appending arguments is supported by this class. If there was previsouly no error, then incremental parsing is used;
 * if the error is QTestLibArgsParser::PrematureEndError then the whole string is reparsed
 * as this error may have been caused by a missing argument;
 * otherwise the rest of the string is not parsed as the error will still be here.
 *
 * Error codes are accessed through the error() member and a useful error message is returned by errorString() method.
 * Error codes should be used in the codes (switches, tests) whereas error string should be used to inform the user.
 *
 * Supported flag list:
 *     - <tt>-o</tt> Needs an argument: a file name optionally followed by a comma and output format (see outFileName() and outputFormat())
 *     - <tt>-txt</tt> Text output format (see QTestLibArgsParser::TxtFormat and outputFormat())
 *     - <tt>-csv</tt> CSV output format (see QTestLibArgsParser::CsvFormat and outputFormat())
 *     - <tt>-xunitxml</tt> XUnit XML output format (see QTestLibArgsParser::XUnitXmlFormat and outputFormat())
 *     - <tt>-xml</tt> QTestLib XML output format (see QTestLibArgsParser::XmlFormat and outputFormat())
 *     - <tt>-lightxml</tt> Light XML output format (see QTestLibArgsParser::LightXmlFormat and outputFormat())
 *     - <tt>-silent</tt> Run test silently (see QTestLibArgsParser::Silent and verbosity())
 *     - <tt>-v1</tt> Run test more verbosely (see QTestLibArgsParser::Verbose1 and verbosity())
 *     - <tt>-v2</tt> Run test very verbosely (see QTestLibArgsParser::Verbose2 and verbosity())
 *     - <tt>-vs</tt> Capture emitted signals (see QTestLibArgsParser::VerboseSignal and verbosity())
 *     - <tt>-vb</tt> Run test with verbosity adapted to benchmarks (see QTestLibArgsParser::VerboseBenchmark and verbosity())
 *     - <tt>-help</tt> Displays QTestLib help (see isHelpOutput())
 *     - <tt>-functions</tt> Displays test case list (see areFunctionsOutput())
 *     - <tt>-datatags</tt> Displays data tags list (see areDatatagsOutput())
 *     - <tt>-eventdelay</tt> Set event delay for keyboard and mouse event simulation to the given argument (in ms, see eventDelay())
 *     - <tt>-keydelay</tt> Set event delay for keyboard event simulation to the given argument (in ms, see keyDelay())
 *     - <tt>-mousedelay</tt> Set event delay for mouse event simulation to the given argument (in ms, see mouseDelay())
 *     - <tt>-maxwarnings</tt> Set maximum number of warnings to the given argument (see maxWarnings())
 *     - <tt>-nocrashhandler</tt> Disabled crash handler (see crashHandlerEnabled())
 *
 * Known but unsupported flag list:
 *     - <tt>-callgrind</tt> Use callgrind to time benchmarks
 *     - <tt>-perf</tt> Use Linux perf events to time benchmarks
 *     - <tt>-perfcounter</tt> Use the counter given in the argument
 *     - <tt>-perfcounterlist</tt> Lists the counters available
 *     - <tt>-tickcounter</tt> Use CPU tick counters to time benchmarks
 *     - <tt>-eventcounter</tt> Counts events received during benchmarks
 *     - <tt>-minimumvalue</tt> Sets the minimum acceptable measurement value to the given argument
 *     - <tt>-minimumtotal</tt> Sets the minimum acceptable total for repeated executions of a test function to the given argument
 *     - <tt>-iterations</tt> Sets the number of accumulation iterations to the given argument
 *     - <tt>-median</tt> Sets the number of median iterations to the given argument
 */
class QTestLibArgsParser
{
public:
    /*!
     * \brief Test output format
     *
     * Describes the format of QTestLib output.
     */
    typedef enum {
        NoneFormat = 0, /*!< Unspecified format, used when test is not output (see \ref isTestOutput()) */
        TxtFormat, /*!< Text format, which is the default */
        CsvFormat, /*!< CSV format (to be used with benchmarks */
        XUnitXmlFormat, /*!< XUnit XML format */
        XmlFormat, /*!< QTestLIb XML format */
        LightXmlFormat, /*!< Light QTestLib XML format */
        AllFormats, /*!< Unspecified format, used when unknown test format is output */
    } TestOutputFormat; // WARNING order matters!
    /*!
     * \brief Test verbosity
     *
     * The possible verbosity levels for QTestLib tests.
     */
    typedef enum {
        Silent = -1, /*!< Minimal verbosity: only failures are output (affects only \ref TxtFormat) */
        NormalVerbosity, /*!< Normal verbosity, which is the default */
        Verbose1, /*!< More verbose (affects only \ref TxtFormat) */
        Verbose2, /*!< Very verbose: outputs a message for each test macro */
        VerboseSignal, /*!< Output the signals emitted */
        VerboseBenchmark, /*!< A verbosity mode designed specially for benchmarks */
    } TestVerbosity; // WARNING order matters!
    /*!
     * \brief What is output
     *
     * Describes what is output when the test is run.
     * This may not include the test output which may not be run.
     */
    typedef enum {
        NormalOutput = 0, /*!< The test is being output */
        HelpOutput, /*!< The help of QTestLIb is being output */
        FunctionsOutput, /*!< The list of test cases is being output */
        DataTagsOutput, /*!< The list of data row is being output */
        PerfCounterNamesOutput, /*!< The list of performance counter methods is being output */
    } Output;
    /*!
     * \brief Error codes
     *
     * Desribes the possible errors of this parser.
     */
    typedef enum {
        NoError, /*!< No error occurred */
        PrematureEndError, /*!< The command line argument list ended prematurely */
        UnknownFlagError, /*!< A flag (an argument beginning with <tt>-</tt>) was unknown */
        EmptyFlagError, /*!< Empty flag in argument list */
        InvalidArgumentError, /*!< The argument of a flag does not have the right format */
        InvalidTestClassError, /*!< The name of a test class does not fulfill the constraints on class names */
        InvalidTestCaseError, /*!< The name of a test function does not fulfill the constraints on function names */
    } Error;
    /*!
     * \brief Constructor from a string.
     *
     * The command line arguments parser will parse the given string
     * assuming it is command line arguments for QTestLib.
     *
     * \param cmdArgs Command line arguments as a string (must not include the name of the program <tt>argv[0]</tt>)
     * \sa QTestLibArgsParser(const QStringList&)
     */
    inline QTestLibArgsParser(const QString& cmdArgs = QString::null) :
        mArgs(cmdArgs.trimmed()) {parse();}
    /*!
     * \brief Constructor from a string list .
     *
     * The command line arguments parser will parse the given string list
     * assuming it is command line arguments list for QTestLib.
     * The argument list is merged into a single string an the previous constructor is used.
     *
     * \param cmdArgs Command line arguments as a string list (must not include the name of the program <tt>argv[0]</tt>)
     * \sa QTestLibArgsParser(const QString&)
     */
    inline QTestLibArgsParser(const QStringList& cmdArgs) :
        QTestLibArgsParser(cmdArgs.join(QLatin1Char(' '))) {}
    /*!
     * \brief Copy constructor
     *
     * Copies the given instance in a new instance.
     * \param other The instance to copy
     */
    QTestLibArgsParser(const QTestLibArgsParser& other);
    /*!
     * \brief Append arguments to the current arguement list
     *
     * Appends the given string to the current argument list and reparses the argument list.
     * If there was previsouly no error, then incremental parsing is used;
     * if the error is QTestLibArgsParser::PrematureEndError then the whole string is reparsed
     * as this error may have been caused by a missing argument;
     * otherwise the rest of the string is not parsed as the error will still be here.
     *
     * \param cmdArgs The command line arguments as a string to be appended to the existing ones.
     * \sa appendArgs(const QStringList&)
     */
    inline void appendArgs(const QString& cmdArgs) {mArgs.append(QLatin1Char(' ') + cmdArgs.trimmed()); incrementalParse();}
    /*!
     * \brief Append arguments to the current arguement list
     *
     * Appends the given string list to the current argument list and reparses the argument list.
     * If there was previsouly no error, then incremental parsing is used;
     * if the error is QTestLibArgsParser::PrematureEndError then the whole string is reparsed
     * as this error may have been caused by a missing argument;
     * otherwise the rest of the string is not parsed as the error will still be here.
     *
     * \param cmdArgs The command line arguments as a string list to be appended to the existing ones.
     * \sa appendArgs(const QString&)
     */
    inline void appendArgs(const QStringList& cmdArgs) {appendArgs(cmdArgs.join(QLatin1Char(' ')));}
    /*!
     * \brief Set the current arguement list
     *
     * Sets the given string in the current argument list and reparses the argument list.
     *
     * \param cmdArgs The command line arguments as a string (replace the existing ones).
     * \sa appendArgs(const QStringList&)
     */
    inline void setArgs(const QString& cmdArgs) {mArgs = cmdArgs.trimmed(); parse();}
    /*!
     * \brief Set the current arguement list
     *
     * Sets the given string list in the current argument list and reparses the argument list.
     *
     * \param cmdArgs The command line arguments as a string list (replace the existing ones).
     * \sa appendArgs(const QString&)
     */
    inline void setArgs(const QStringList& cmdArgs) {mArgs = cmdArgs.join(QLatin1Char(' ')); parse();}

    /*!
     * \brief Parses the current command line argument list
     *
     * This function updates the cached parse results by reparsing the current command line argument list.
     */
    inline void parse(void) {parse(false);}
    /*!
     * \brief Returns a QStringList representation of the arguments
     *
     * This function is the opposite of parse(): It converts the arguments in memory into a list of arguments.
     * \param version The version of the testlib arguments (0, 1 or 2).
     * \return A list of the arguments currently set.
     * \sa toString()
     */
    QStringList toStringList(uint version = 2) const;
    /*!
     * \brief Returns a QString representation of the arguments
     *
     * This function is the opposite of parse(): It converts the arguments in memory into a string of arguments.
     * \param version The version of the testlib arguments (0, 1 or 2).
     * \return The string of arguments currently set.
     * \sa toString()
     */
    inline QString toString(uint version = 2) const {return toStringList(version).join(QLatin1Char(' '));}

    /*!
     * \brief Convert to map.
     *
     * Saves the contents of the QTestLib arg parser in a QVariant Map.
     * \param map The QVariant map where to save the current object.
     * \sa fromMap()
     */
    void toMap(QVariantMap& map) const;
    /*!
     * \brief Initialises from map.
     *
     * Initializes the contents of the QTestLib args parser from a QVariant map.
     * \param map The map to use for initialisation.
     * \sa toMap()
     */
    void fromMap(const QVariantMap& map);

    /*!
     * \brief Error code
     *
     * Returns the parser error code.
     *
     * \return The parser error code.
     * \sa errorString()
     */
    inline Error error(void) const {return mError;}
    /*!
     * \brief Error message
     *
     * Returns a user-friendly error message.
     *
     * \return A user-friendly error message.
     * \sa error()
     */
    inline QString errorString(void) const {return mErrorString;}
    /*!
     * \brief Unknown/bad flags
     *
     * Returns all the token which could not be parsed.
     *
     * \return The tokens which could not be parsed.
     */
    inline QStringList unknownArgs(void) const {return mUnknownFlags;}

    /*!
     * \brief The name of the output file.
     *
     * Returns the name of the file where the output is written.
     *
     * \return The name of the output file
     * \sa setOutFileName()
     */
    inline Utils::FileName outFileName(void) const {return mOutFileName;}
    /*!
     * \brief Set the name of the output file.
     *
     * Sets the name of the file where the output is written.
     *
     * \param fileName The name of the output file
     * \sa outFileName()
     */
    inline void setOutFileName(const Utils::FileName& fileName) {mOutFileName = fileName;}
    /*!
     * \brief The output format
     *
     * Returns the format of the output.
     *
     * \return The output format
     * \sa TestOutputFormat, setOutputFormat()
     */
    inline TestOutputFormat outputFormat(void) const {return mParser;}
    /*!
     * \brief Set the output format
     *
     * Sets the format of the output.
     *
     * \param format The output format
     * \sa TestOutputFormat, outputFormat()
     */
    inline void setOutputFormat(TestOutputFormat format) {mParser = format;}
    /*!
     * \brief The verbosity
     *
     * Returns the verbosity of the test output.
     *
     * \return The verbosity of the test output.
     * \sa TestVerbosity, setVerbosity()
     */
    inline TestVerbosity verbosity(void) const {return mVerbosity;}
    /*!
     * \brief Set the verbosity
     *
     * Sets the verbosity of the test output.
     *
     * \param verbosity The verbosity of the test output.
     * \sa TestVerbosity, verbosity()
     */
    inline void setVerbosity(TestVerbosity verbosity) {mVerbosity = verbosity;}

    /*!
     * \brief Is the crash handler enabled?
     *
     * Tells whether the crash handler is enabled for this test.
     *
     * \return \c true when the crash handler is enabled
     * \sa enableCrashHandler()
     */
    inline bool crashHandlerEnabled(void) const {return mCrashHandlerEnabled;}
    /*!
     * \brief Enables the crash handler
     *
     * Enables or disables the crash handler for this test.
     *
     * \param enabled \c true to enable the crash handler
     * \sa crashHandlerEnabled()
     */
    inline void enableCrashHandler(bool enabled) {mCrashHandlerEnabled = enabled;}
    /*!
     * \brief Maximum number of warnings
     *
     * Gives the maximum number of warnings which will be output.
     * \c 0 means that this limit is disabled.
     *
     * \return The maximum number of output messages or \c 0 if this limit is disabled.
     * \sa setMaxWarnings()
     */
    inline unsigned int maxWarnings(void) const {return mMaxWarnings;}
    /*!
     * \brief Set the maximum number of warnings
     *
     * Sets the maximum number of warnings which will be output.
     * \c 0 disables this limit.
     *
     * \param max The maximum number of output messages or \c 0 to disable this limit.
     * \sa maxWarnings()
     */
    inline void setMaxWarnings(unsigned int max = 2000) {mMaxWarnings = max;}
    /*!
     * \brief Event delay
     *
     * Gives the event delay (in ms) for the simulation of the keyboard and the mouse.
     *
     * \return The event delay (in ms)
     * \sa keyDelay(), mouseDelay(), setEventDelay()
     */
    inline int eventDelay(void) const {return mEventDelay;}
    /*!
     * \brief Set event delay
     *
     * Sets the event delay (in ms) for the simulation of the keyboard and the mouse.
     *
     * \param delay The event delay (in ms)
     * \sa setKeyDelay(), setMouseDelay(), eventDelay()
     */
    inline void setEventDelay(int delay = -1) {mEventDelay = delay;}
    /*!
     * \brief Keyborad Event delay
     *
     * Gives the event delay (in ms) for the simulation of the keyboard.
     *
     * \return The keyboard event delay (in ms)
     * \sa eventDelay(), setKeyDelay()
     */
    inline int keyDelay(void) const {return mKeyDelay;}
    /*!
     * \brief Set keyborad Event delay
     *
     * Sets the event delay (in ms) for the simulation of the keyboard.
     *
     * \param delay The keyboard event delay (in ms)
     * \sa setEventDelay(), keyDelay()
     */
    inline void setKeyDelay(int delay = -1) {mKeyDelay =  delay;}
    /*!
     * \brief Mouse event delay
     *
     * Gives the event delay (in ms) for the simulation of the mouse.
     *
     * \return The mouse event delay (in ms)
     * \sa eventDelay(), setMouseDelay()
     */
    inline int mouseDelay(void) const {return mMouseDelay;}
    /*!
     * \brief Set mouse event delay
     *
     * Sets the event delay (in ms) for the simulation of the mouse.
     *
     * \param delay The mouse event delay (in ms)
     * \sa setEventDelay(), mouseDelay()
     */
    inline void setMouseDelay(int delay = -1) {mMouseDelay = delay;}

    /*!
     * \brief Is test output?
     *
     * Tells whether the test is really run and output.
     *
     * \return \c true when the test is run and output
     * \sa isHelpOutput(), areFunctionsOutput(), areDatatagsOutput(), setOutput()
     */
    inline bool isTestOutput(void) const {return (mOutput == NormalOutput);}
    /*!
     * \brief Is help output?
     *
     * Tells whether the help of QTestLib is output instead of the test output.
     *
     * \return \c true when the help of QTestLib is output
     * \sa isTestOutput(), setOutput()
     */
    inline bool isHelpOutput(void) const {return (mOutput == HelpOutput);}
    /*!
     * \brief Are test cases output?
     *
     * Tells whether the list of test cases is output instead of the test output.
     *
     * \return \c true when the test case list is output
     * \sa isTestOutput(), setOutput()
     */
    inline bool areFunctionsOutput(void) const {return (mOutput == FunctionsOutput);}
    /*!
     * \brief Are data tags output?
     *
     * Tells whether data tag list is output instead of the test output.
     *
     * \return \c true when the data tag list is output
     * \sa isTestOutput(), setOutput()
     */
    inline bool areDatatagsOutput(void) const {return (mOutput == DataTagsOutput);}
    /*!
     * \brief Set the type of output.
     *
     * Set the type of the output, i.e. what the test will output.
     *
     * \param output What will be output by the test.
     * \sa Output, isTestOutput(), isHelpOutput(), areFunctionsOutput(), areDatatagsOutput()
     */
    inline void setOutput(Output output) {mOutput = output;}

    /*!
     * \brief List of selected test cases
     *
     * Returns the list of selected test cases.
     *
     * \return The list of selected test cases
     * \sa TestClassList
     */
    inline TestClassList selectedTestCases(void) const {return mSelectedTestCases;}

    // TODO Comment
    void addSelectedTestClass(const QString& selectedTestClass, const QString& selectedTestCase = QString::null, const QString& selectedTestData = QString::null);
    inline void addSelectedTestCase(const QString& selectedTestCase, const QString& selectedTestData = QString::null) {addSelectedTestClass(QString::null, selectedTestCase, selectedTestData);}
    // TODO Comment
    void removeSelectedTestClass(const QString& selectedTestClass, const QString& selectedTestCase = QString::null, const QString& selectedTestData = QString::null);
    inline void removeSelectedTestCase(const QString& selectedTestCase, const QString& selectedTestData = QString::null) {removeSelectedTestClass(QString::null, selectedTestCase, selectedTestData);}
private:
    /*!
     * \brief Clean test specification
     *
     * This function unquotes and unescapes, if required, test specifications.
     * Its return value indicates whether the test specification is valid.
     *
     * \param spec A Test specification (for class, function or data row)
     * \param escaped Whether this function should unescape the characters
     * \return \c true when the test specification is valid, \¢ false otherwise
     */
    bool cleanTestSpecification(QString& spec, bool escaped = false);

    /*!
     * \brief Parses the current command line argument list
     *
     * This function updates the cached parse results by reparsing the current command line argument list.
     *
     * \param incremental \c true to use incremental parsing
     * \sa parse()
     */
    void parse(bool incremental);
    /*!
     * \brief Require incremental parse
     *
     * Require an incremental parse of the current argument list to update the cached parse result.
     * This function should be used when arguments are appended to the command line argument list
     * (see appendArgs(const QString&) and appendArgs(const QStringList&)).
     *
     * If there was previsouly no error, then incremental parsing is used;
     * if the error is QTestLibArgsParser::PrematureEndError then the whole string is reparsed
     * as this error may have been caused by a missing argument;
     * otherwise the rest of the string is not parsed as the error will still be here.
     *
     * \sa parse(bool)
     */
    void incrementalParse(void);
    /*!
     * \brief Initialisation of the parser
     *
     * Initializes the parsed results with their default values.
     * This function is used when parse(bool) is used.
     */
    void initDefaults(void);
    /*!
     * \brief Returns the next token
     *
     * Call the lexer to give the next token in the current argument list.
     *
     * \return The next token in the current argument list
     */
    QString nextToken(void);

    /*!
     * \brief Parses an unsigned integer
     *
     * Tries to convert the given token into an unsigned integer.
     * The given default value is used if the conversion fails and
     * error() will return QTestLibArgsParser::InvalidArgumentError.
     *
     * \param token The token to convert into an unsigned integer
     * \param cmd The name of the flag preceeding the unsigned integer
     * \param defaultValue The default value returned when the conversion fails
     * \return The unsigned integer value contained in the given token or
     * the default value if the given token can not be converted to an unsigned integer.
     * \sa parseInteger()
     */
    unsigned int parseUnsignedInteger(const QString& token, const QString& cmd, unsigned int defaultValue = 0);
    /*!
     * \brief Parses an integer
     *
     * Tries to convert the given token into an integer.
     * The given default value is used if the conversion fails and
     * error() will return QTestLibArgsParser::InvalidArgumentError.
     *
     * \param token The token to convert into an integer
     * \param cmd The name of the flag preceeding the integer
     * \param defaultValue The default value returned when the conversion fails
     * \return The integer value contained in the given token or
     * the default value if the given token can not be converted to an integer.
     * \sa parseUnsignedInteger()
     */
    int parseInteger(const QString& token, const QString& cmd, int defaultValue = -1);
    /*!
     * \brief Parses a string
     *
     * Converts the given token into a string
     * by unescaping, unquoting and then trimming the token.
     *
     * \param token The token which will be unescaped, unquoted and then trimmed
     * \return The unescaped, unquoted and then trimmed token
     */
    QString parseString(const QString& token);

    /*!
     * \brief Parses selected test list
     *
     * Parses the selected test cases and data tags and
     * sets appropriately the internal selected test case list.
     *
     * \param token A test case (and optionally data tag specification)
     * \sa selectedTestCases()
     */
    void parseSelectedTest(const QString& token);
    /*!
     * \brief Parses output flag argument
     *
     * Parses output flag (<tt>-o</tt>) argument.
     * This function supports V1 and V2 output specifcation format.
     * The results are directly written to the suitable members.
     *
     * \param token The argument of the output flag (<tt>-o</tt>)
     */
    void parseOutput(const QString& token);

    QString mArgs; /*!< Currently parsed command line argument list */
    int mPos; /*!< Position of the parser in the current command line argument list */
    Error mError; /*!< Current error state of the parser (see error())*/
    QString mErrorString; /*!< Current error string for the parser (cached for simplicity, see errorString()) */
    QStringList mUnknownFlags; /*!< List of unknown flags */

    TestClassList mSelectedTestCases; /*!< List of selected test cases (see selectedTestCases()) */

    Utils::FileName mOutFileName; /*!< Output file name (currently only one output file is supported (see outFileName()) */
    TestOutputFormat mParser; /*!< The output format of the test (see outputFormat()) */
    TestVerbosity mVerbosity; /*!< The output verbosity of the test (see verbosity()) */
    Output mOutput; /*!< What will be output by the executable (see isTestOutput(), isHelpOutput(), areFunctionsOutput(), areDatatagsOutput()) */
    bool mCrashHandlerEnabled; /*!< Is crash handler enabled ? (see crashHandlerEnabled()) */
    unsigned int mMaxWarnings;/*!< The maximum number of output messages (see maxWarnings()) */
    int mEventDelay; /*!< Delay in ms for keyboard and mouse event simultation (see eventDelay()) */
    int mKeyDelay; /*!< Delay in ms for keyboard event simultation (see keyDelay()) */
    int mMouseDelay; /*!< Delay in ms for mouse event simultation (see mouseDelay()) */
    bool mSeenOutputWithFormat; /*!< Has a V2 output flags been seen? */
};

} // namespace Internal
} // namespace QTestLibPlugin

Q_DECLARE_METATYPE(QTestLibPlugin::Internal::QTestLibArgsParser::TestOutputFormat)
Q_DECLARE_METATYPE(QTestLibPlugin::Internal::QTestLibArgsParser::TestVerbosity)

#endif // QTESTLIBARGSPARSER_H
