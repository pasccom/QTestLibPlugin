#ifndef QTESTLIBARGSPARSER_H
#define QTESTLIBARGSPARSER_H

#include <utils/fileutils.h>
#include <QString>
#include <QStringList>
#include <QLinkedList>

namespace QTestLibPlugin {
namespace Internal {

class QTestLibArgsParser
{
public:
    typedef QLinkedList< QPair<QString, QStringList> > TestCaseList;

    typedef enum {
        NoneFormat = 0,
        TxtFormat,
        CsvFormat,
        XUnitXmlFormat,
        XmlFormat,
        LightXmlFormat,
    } TestOutputFormat; // WARNING order matters!
    typedef enum {
        Silent = -1,
        NormalVerbosity,
        Verbose1,
        Verbose2,
        VerboseSignal,
        VerboseBenchmark,
    } TestVerbosity; // WARNING order matters!
    typedef enum {
        NormalOutput,
        HelpOutput,
        FunctionsOutput,
        DataTagsOutput,
        PerfCounterNamesOutput,
    } Output;
    typedef enum {
        NoError,
        PrematureEndError,
        UnknownFlagError,
        InvalidArgumentError,
    } Error;
    inline QTestLibArgsParser(const QString& cmdArgs) :
        mArgs(cmdArgs.trimmed()) {parse();}
    inline QTestLibArgsParser(const QStringList& cmdArgs) :
        QTestLibArgsParser(cmdArgs.join(QLatin1Char(' '))) {}
    inline void appendArgs(const QString& cmdArgs) {mArgs + QLatin1Char(' ') + cmdArgs.trimmed(); incrementalParse();}
    inline void appendArgs(const QStringList& cmdArgs) {appendArgs(cmdArgs.join(QLatin1Char(' ')));}

    inline Error error(void) const {return mError;}
    inline QString errorString(void) const {return mErrorString;}

    inline Utils::FileName outFileName(void) const {return mOutFileName;}
    inline TestOutputFormat outputFormat(void) const {return mParser;}
    inline TestVerbosity verbosity(void) const {return mVerbosity;}

    inline bool crashHandlerEnabled(void) const {return mCrashHandlerEnabled;}
    inline unsigned int maxWarnings(void) const {return mMaxWarnings;}
    inline int eventDelay(void) const {return mEventDelay;}
    inline int keyDelay(void) const {return mKeyDelay;}
    inline int mouseDelay(void) const {return mMouseDelay;}

    inline bool isTestOutput(void) const {return (mOutput == NormalOutput);}
    inline bool isHelpOutput(void) const {return (mOutput == HelpOutput);}
    inline bool areFunctionsOutput(void) const {return (mOutput == FunctionsOutput);}
    inline bool areDatatagsOutput(void) const {return (mOutput == DataTagsOutput);}

    inline TestCaseList selectedTestCases(void) const {return mSelectedTestCases;}
private:
    void parse(bool incremental = false);
    void incrementalParse(void);
    void initDefaults(void);
    QString nextToken(void);

    unsigned int parseUnsignedInteger(const QString& token, unsigned int defaultValue = 0);
    int parseInteger(const QString& token, int defaultValue = -1);
    QString parseString(const QString& token);

    void parseSelectedTest(const QString& token);
    void parseOutput(const QString& token);

    QString mArgs;
    int mPos;
    Error mError;
    QString mErrorString;

    TestCaseList mSelectedTestCases;

    Utils::FileName mOutFileName;
    TestOutputFormat mParser;
    TestVerbosity mVerbosity;
    Output mOutput;
    bool mCrashHandlerEnabled;
    unsigned int mMaxWarnings;
    int mEventDelay;
    int mKeyDelay;
    int mMouseDelay;
    bool mSeenOutputWithFormat;
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // QTESTLIBARGSPARSER_H
