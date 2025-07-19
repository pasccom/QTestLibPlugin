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

#include "qtestlibargsparser.h"
#include "qtestlibpluginconstants.h"

#include <utils/qtcassert.h>

#include <QCoreApplication>
#include <QRegExp>
#include <QtDebug>

namespace QTestLibPlugin {
namespace Internal {

TestRowList::operator QString () const
{
    QString ret;
    QRegExp escapeRegExp("([\"\\])");
    QRegExp nonAlphaRegExp("[^A-Za-z0-9_]");

    for (const_iterator it = constBegin(); it != constEnd(); it++) {
        if (it->isEmpty())
            continue;
        if (nonAlphaRegExp.containedIn(*it))
            ret = ret + " \"" + escapeRegExp.replaceIn(*it, "\\\\\\1") + "\"";
        else
            ret = ret + " " + *it;
    }

    return ret.mid(1);
}

TestCaseList::operator QString () const
{
    QString ret;
    QRegExp escapeRegExp("([\"\\])");
    QRegExp nonAlphaRegExp("[^A-Za-z0-9_]");

    for (const_iterator it = constBegin(); it != constEnd(); it++) {
        if (it->first.isEmpty())
            continue;
        if (it->second.isEmpty()) {
            ret = ret + " " + it->first;
        } else {
            for (QString testRow : it->second) {
                if (nonAlphaRegExp.containedIn(testRow))
                    ret = ret + " \"" + it->first + ":" + escapeRegExp.replaceIn(testRow, "\\\\\\1") + "\"";
                else
                    ret = ret + " " + it->first + ":" + testRow;
            }
        }
    }

    return ret.mid(1);
}

TestClassList::operator QString () const
{
    QString ret;
    QRegExp escapeRegExp("([\"\\])");
    QRegExp nonAlphaRegExp("[^A-Za-z0-9_]");

    for (const_iterator it = constBegin(); it != constEnd(); it++) {
        if (it->first.isEmpty() && it->second.isEmpty())
            continue;
        if (it->second.isEmpty()) {
            ret = ret + " " + it->first;
        } else {
            for (QPair<QString, TestRowList> testCase : it->second) {
                QString prefix;
                if (!it->first.isEmpty())
                    prefix.append(it->first).append("::");
                if (!testCase.first.isEmpty())
                    prefix.append(testCase.first).append(":");
                if (prefix.endsWith("::"))
                    prefix.chop(1);

                if (testCase.second.isEmpty()) {
                    prefix.chop(1);
                    ret = ret + " " + prefix;
                } else {
                    for (QString testRow : testCase.second) {
                        if (nonAlphaRegExp.containedIn(testRow))
                            ret = ret + " \"" + prefix + escapeRegExp.replaceIn(testRow, "\\\\\\1") + "\"";
                        else
                            ret = ret + " " + prefix + testRow;
                    }
                }
            }
        }
    }

    return ret.mid(1);
}

QTestLibArgsParser::QTestLibArgsParser(const QTestLibArgsParser& other)
{
    if (!other.mArgs.isNull()) {
        // Copy the other args and parse them.
        mArgs = other.mArgs;
        parse();
    } else {
        // Copy the internal data.
        mSelectedTestCases = other.mSelectedTestCases;
        mOutFileName = other.mOutFileName;
        mParser = other.mParser;
        mVerbosity = other.mVerbosity;
        mOutput = other.mOutput;
        mCrashHandlerEnabled = other.mCrashHandlerEnabled;
        mMaxWarnings = other.mMaxWarnings;
        mEventDelay = other.mEventDelay;
        mKeyDelay = other.mKeyDelay;
        mMouseDelay = other.mMouseDelay;

        mError = other.mError;
        mErrorString = other.mErrorString;
    }
}

void QTestLibArgsParser::toMap(Utils::Store& map) const
{
    QTC_ASSERT(mOutput == NormalOutput, );

    if (mParser != TxtFormat)
        map.insert(Constants::FormatKey, (int) mParser);
    if (mVerbosity != NormalVerbosity)
        map.insert(Constants::VerbosityKey, (int) mVerbosity);
    if (!mOutFileName.isEmpty())
        map.insert(Constants::OutputFileKey, mOutFileName.toFSPathString());
    if (mMaxWarnings != 2000)
        map.insert(Constants::MaxWarningKey, mMaxWarnings);
    if (mEventDelay > 0)
        map.insert(Constants::EventDelayKey, mEventDelay);
    if (mKeyDelay > 0)
        map.insert(Constants::KeyDelayKey, mKeyDelay);
    if (mMouseDelay > 0)
        map.insert(Constants::MouseDelayKey, mMouseDelay);
    if (!mCrashHandlerEnabled)
        map.insert(Constants::CrashHandlerEnabledKey, false);
}

void QTestLibArgsParser::fromMap(const Utils::Store& map)
{
    QTC_ASSERT(mOutput == NormalOutput, );

    mParser = (TestOutputFormat) map.value(Constants::FormatKey, (int) TxtFormat).toInt();
    mVerbosity = (TestVerbosity) map.value(Constants::VerbosityKey, (int) NormalVerbosity).toInt();
    mOutFileName = Utils::FilePath::fromString(map.value(Constants::OutputFileKey).toString());
    mMaxWarnings = map.value(Constants::MaxWarningKey, 2000).toUInt();
    mEventDelay = map.value(Constants::EventDelayKey, -1).toInt();
    mKeyDelay = map.value(Constants::KeyDelayKey, -1).toInt();
    mMouseDelay = map.value(Constants::MouseDelayKey, -1).toInt();
    mCrashHandlerEnabled = map.value(Constants::CrashHandlerEnabledKey, true).toBool();
}


QStringList QTestLibArgsParser::toStringList(uint version) const
{
    Q_ASSERT((version == 1) || (version == 2));

    QStringList ret;
    QStringList formats;
    QStringList verbosities;
    QStringList outputs;
    QString outFile(mOutFileName.toFSPathString());
    if (!QRegExp(QLatin1String("[A-Za-z0-9_.-]+")).exactMatch(outFile) || (outFile == "-")) {
        outFile.replace(QLatin1Char('\"'), QLatin1String("\\\""));
        outFile.prepend(QLatin1Char('\"')).append(QLatin1Char('\"'));
    }

    formats << QLatin1String("txt") << QLatin1String("csv") << QLatin1String("xunitxml") << QLatin1String("xml") << QLatin1String("lightxml");
    verbosities << QLatin1String("-silent") << QLatin1String("") << QLatin1String("-v1") << QLatin1String("-v2") << QLatin1String("-vs") << QLatin1String("-vb");
    outputs << QLatin1String("-help") << QLatin1String("-functions") << QLatin1String("-datatags")  << QLatin1String("-perfcounterlist");

    if (mOutput != NormalOutput) {
        ret << outputs.at((int) mOutput - 1);
    } else {
        if (version == 1) {
            if (mParser != TxtFormat)
                ret << QLatin1String("-") + formats.at((int) mParser - 1);
            if (!mOutFileName.isEmpty())
                ret << QLatin1String("-o") << outFile;
        } else {
            if ((mParser != TxtFormat) || (!mOutFileName.isEmpty())) {
                ret << QLatin1String("-o");
                if (mOutFileName.isEmpty())
                    ret << QLatin1String("-,") + formats.at((int) mParser - 1);
                else
                    ret << outFile + QLatin1String(",") + formats.at((int) mParser - 1);
            }
        }

        if (mVerbosity != NormalVerbosity)
            ret << verbosities.at((int) mVerbosity + 1);
        if (!mCrashHandlerEnabled)
            ret << QLatin1String("-nocrashhandler");
        if (mMaxWarnings != 2000)
            ret << QLatin1String("-maxwarnings") << QString::number(mMaxWarnings, 10);
        if (mEventDelay > 0)
            ret << QLatin1String("-eventdelay") << QString::number(mEventDelay, 10);
        if (mKeyDelay > 0)
            ret << QLatin1String("-keydelay") << QString::number(mKeyDelay, 10);
        if (mMouseDelay > 0)
            ret << QLatin1String("-mousedelay") << QString::number(mMouseDelay, 10);
    }

    return ret;
}

void QTestLibArgsParser::parse(bool incremental)
{
    if (!incremental)
        initDefaults();

    QStringList knownFlags;
    // Output flag
    knownFlags << QLatin1String("o");
    // ParserFormat flags
    knownFlags << QLatin1String("txt") << QLatin1String("csv") << QLatin1String("xunitxml") << QLatin1String("xml") << QLatin1String("lightxml");
    // Verbosity flags
    knownFlags << QLatin1String("silent") << QLatin1String("") << QLatin1String("v1") << QLatin1String("v2") << QLatin1String("vs") << QLatin1String("vb");
    // Options affecting output
    knownFlags << QLatin1String("functions") << QLatin1String("datatags") << QLatin1String("help") << QLatin1String("perfcounterlist");
    // Other test flags
    knownFlags << QLatin1String("eventdelay") << QLatin1String("keydelay") << QLatin1String("mousedelay") << QLatin1String("maxwarnings") << QLatin1String("nocrashhandler");
    // Other benchmark flags
    knownFlags << QLatin1String("callgrind") << QLatin1String("perf") << QLatin1String("perfcounter") << QLatin1String("tickcounter") << QLatin1String("eventcounter") << QLatin1String("minimumvalue") << QLatin1String("minimumtotal") << QLatin1String("iterations") << QLatin1String("median");

    int currentFlag = -1;
    QString token;

    while (!(token = nextToken()).isNull()) {
        qDebug() << token;
        if (currentFlag == -1) {
            // Token is not a flag argument
            if (token.startsWith(QLatin1Char('-')))
                currentFlag = knownFlags.indexOf(token.mid(1));
            else
                currentFlag = -2;
            qDebug() << currentFlag;
            switch (currentFlag) {
            case -2: // Test case name?
                parseSelectedTest(token);
                break;
            case -1: // Unknown flag
                mError = UnknownFlagError;
                mErrorString = QCoreApplication::translate("QTestLibPlugin::Internal::QTestLibArgsParser", "Unknown flag \"%1\"").arg(token);
                mUnknownFlags << token;
                break;
            case 7: // Empty flag
                mError = EmptyFlagError;
                mErrorString = QCoreApplication::translate("QTestLibPlugin::Internal::QTestLibArgsParser", "Empty flag");
                mUnknownFlags << token;
                continue;
            case 0: // -o
                continue;
            case 1: // -txt
            case 2: // -csv
            case 3: // -xunitxml
            case 4: // -xml
            case 5: // -lightXml
                if (!mSeenOutputWithFormat)
                    mParser = (TestOutputFormat) currentFlag;
                break;
            case 6: // -silent
            case 8: // -v1
            case 9: // -v2
            case 10: // -vs
            case 11: // -vb
                mVerbosity = (TestVerbosity) (currentFlag - 7);
                break;
            case 12: // -functions
                if (mOutput == NormalOutput) {
                    mParser = NoneFormat;
                    mOutput = FunctionsOutput;
                }
                break;
            case 13: // -datatags
                if (mOutput == NormalOutput) {
                    mParser = NoneFormat;
                    mOutput = DataTagsOutput;
                }
                break;
            case 14: // -help
                if (mOutput == NormalOutput) {
                    mParser = NoneFormat;
                    mOutput = HelpOutput;
                }
                break;
            case 15: // -perfcounterlist
                if (mOutput == NormalOutput) {
                    mParser = NoneFormat;
                    mOutput = PerfCounterNamesOutput;
                }
                break;
            case 16: // -eventdelay
            case 17: // -keydelay
            case 18: // -mousedelay
            case 19: // -maxwarnings
                continue;
            case 20: // -nocrashhandler
                mCrashHandlerEnabled = false;
                break;
            default:
                qWarning() << "Benchmark flags are not supported yet";
                break;
            }
        } else {
            // Token is a flag argument
            switch(currentFlag) {
            case 0: // -o
                parseOutput(token);
                break;
            case 16: // -eventdelay
                mEventDelay = parseInteger(token, "-eventdelay", mEventDelay);
                break;
            case 17: // -keydelay
                mKeyDelay = parseInteger(token, "-keydelay", mKeyDelay);
                break;
            case 18: // -mousedelay
                mMouseDelay = parseInteger(token, "-mousedelay", mMouseDelay);
                break;
            case 19: // -maxwarnings
                mMaxWarnings = parseUnsignedInteger(token, "-maxwarnings", mMaxWarnings);
                break;
            }
        }
        currentFlag = -1;
    }

    if (currentFlag != -1) {
        mError = PrematureEndError;
        mErrorString = QCoreApplication::translate("QTestLibPlugin::Internal::QTestLibArgsParser", "String of command line arguments ended prematurely");
        mUnknownFlags << "-" + knownFlags[currentFlag];
    }
}

void QTestLibArgsParser::parseSelectedTest(const QString& token)
{
    QString selectedTestClass = token;
    QString selectedTestCase;
    QString selectedTestData;

    if (!cleanTestSpecification(selectedTestClass, true))
        return;

    int paamayimNekudotayim = selectedTestClass.indexOf(QLatin1String("::"), 0);
    if (paamayimNekudotayim != -1) {
        selectedTestCase = selectedTestClass.mid(paamayimNekudotayim + 2);
        selectedTestClass = selectedTestClass.left(paamayimNekudotayim);
    } else {
        selectedTestCase = selectedTestClass;
        selectedTestClass.clear();
    }
    if (!cleanTestSpecification(selectedTestCase))
        return;

    int colon = selectedTestCase.indexOf(QLatin1Char(':'), 0);
    if (colon != -1) {
        selectedTestData = selectedTestCase.mid(colon + 1);
        selectedTestCase = selectedTestCase.left(colon);
    }
    if (!cleanTestSpecification(selectedTestData))
        return;

    QRegExp nameRegexp(QLatin1String("[a-zA-Z_][a-zA-Z0-9_]*"));
    if (!selectedTestClass.isEmpty() && !nameRegexp.exactMatch(selectedTestClass)) {
        mError = InvalidTestClassError;
        mErrorString = QCoreApplication::translate("QTestLibPlugin::Internal::QTestLibArgsParser", "The given argument \"%1\" is neither a flag nor a test specification (bad test class)").arg(token);
        mUnknownFlags << token;
        return;
    }
    if (!selectedTestCase.isEmpty() && !nameRegexp.exactMatch(selectedTestCase)) {
        mError = InvalidTestCaseError;
        mErrorString = QCoreApplication::translate("QTestLibPlugin::Internal::QTestLibArgsParser", "The given argument \"%1\" is neither a flag nor a test specification (bad test case)").arg(token);
        mUnknownFlags << token;
        return;
    }

    qDebug() << selectedTestClass << selectedTestCase << selectedTestData;

    addSelectedTestClass(selectedTestClass, selectedTestCase, selectedTestData);
}

void QTestLibArgsParser::addSelectedTestClass(const QString& selectedTestClass, const QString& selectedTestCase, const QString& selectedTestData)
{
    TestClassList::iterator classIt;
    for (classIt = mSelectedTestCases.begin(); classIt != mSelectedTestCases.end(); classIt++)
        if (QString::compare(classIt->first, selectedTestClass, Qt::CaseSensitive) == 0)
            break;

    TestCaseList selectedTestCases;

    if (!selectedTestCase.isEmpty()) {
        if (classIt != mSelectedTestCases.end()) {
            selectedTestCases = classIt->second;
            if (selectedTestCases.isEmpty())
                return;
        }
        TestCaseList::iterator caseIt;
        for (caseIt = selectedTestCases.begin(); caseIt != selectedTestCases.end(); caseIt++)
            if (QString::compare(caseIt->first, selectedTestCase, Qt::CaseSensitive) == 0)
                break;

        TestRowList selectedTestRows;

        if (!selectedTestData.isEmpty()) {
            if (caseIt != selectedTestCases.end()) {
                selectedTestRows = caseIt->second;
                if (selectedTestRows.isEmpty())
                    return;
            }
            if (!selectedTestRows.contains(selectedTestData))
                selectedTestRows.append(selectedTestData);
        }

        caseIt = selectedTestCases.erase(caseIt);
        caseIt = selectedTestCases.insert(caseIt, qMakePair(selectedTestCase, selectedTestRows));
    }

    classIt = mSelectedTestCases.erase(classIt);
    classIt = mSelectedTestCases.insert(classIt, qMakePair(selectedTestClass, selectedTestCases));
}

void QTestLibArgsParser::removeSelectedTestClass(const QString& selectedTestClass, const QString& selectedTestCase, const QString& selectedTestData)
{
    TestClassList::iterator classIt;
    for (classIt = mSelectedTestCases.begin(); classIt != mSelectedTestCases.end(); classIt++)
        if (QString::compare(classIt->first, selectedTestClass, Qt::CaseSensitive) == 0)
            break;
    if (classIt != mSelectedTestCases.end())
        return;
    if (selectedTestCase.isEmpty()) {
        classIt = mSelectedTestCases.erase(classIt);
        return;
    }

    TestCaseList selectedTestCases = classIt->second;
    TestCaseList::iterator caseIt;
    for (caseIt = selectedTestCases.begin(); caseIt != selectedTestCases.end(); caseIt++)
        if (QString::compare(caseIt->first, selectedTestCase, Qt::CaseSensitive) == 0)
            break;
    if (caseIt != selectedTestCases.end())
        return;
    if (selectedTestData.isEmpty()) {
        caseIt = selectedTestCases.erase(caseIt);
    } else {
        TestRowList selectedTestRows = caseIt->second;
        TestRowList::iterator rowIt;

        for (rowIt = selectedTestRows.begin(); rowIt != selectedTestRows.end();) {
            if (QString::compare(*rowIt, selectedTestData, Qt::CaseSensitive) == 0)
                rowIt = selectedTestRows.erase(rowIt);
            else
                rowIt++;
        }

        caseIt = selectedTestCases.erase(caseIt);
        caseIt = selectedTestCases.insert(caseIt, qMakePair(selectedTestCase, selectedTestRows));
    }

    classIt = mSelectedTestCases.erase(classIt);
    classIt = mSelectedTestCases.insert(classIt, qMakePair(selectedTestClass, selectedTestCases));
}

bool QTestLibArgsParser::cleanTestSpecification(QString& spec, bool escaped)
{
    if (spec.startsWith(QLatin1Char('"'))) {
        // Unescape spec
        int escape = 0;
        while (escaped && ((escape = spec.indexOf(QLatin1Char('\\'), escape)) != -1)) {
            if (escape + 1 >= spec.length()) {
                mError = PrematureEndError;
                mErrorString = QCoreApplication::translate("QTestLibPlugin::Internal::QTestLibArgsParser", "Given test specification is badly terminated (escape)");
                mUnknownFlags << spec;
                return false;
            }
            spec.remove(escape, 1);
            escape++;
        }
        // Unquote spec
        if (!spec.endsWith(QLatin1Char('"'))) {
            mError = PrematureEndError;
            mErrorString = QCoreApplication::translate("QTestLibPlugin::Internal::QTestLibArgsParser", "Given test specification is badly terminated (missing quote)");
            mUnknownFlags << spec;
            return false;
        }
        spec.remove(0, 1);
        spec.chop(1);
    }

    return true;
}

void QTestLibArgsParser::parseOutput(const QString& token)
{
    QStringList parserFormats;
    parserFormats << QLatin1String("txt") << QLatin1String("csv") << QLatin1String("xunitxml") << QLatin1String("xml") << QLatin1String("lightxml");

    QString fileName;
    QString format;

    if (token.startsWith(QLatin1String("-,"))) {
        format = token.mid(2);
        mSeenOutputWithFormat = true;
    } else {
        int pos = 0;
        bool quoted = false;
        bool escaped = false;

        while(pos < token.length()) {
            if (!escaped && !quoted && (token.at(pos) == QLatin1Char(',')))
                break;
            if (!escaped && (token.at(pos) == QLatin1Char('"')))
                quoted = !quoted;
            if (token.at(pos) == QLatin1Char('\\'))
                escaped = !escaped;
            else
                escaped = false;
            pos++;
        }

        if (pos == token.length()) {
            fileName = parseString(token);
        } else {
            fileName = parseString(token.left(pos));
            format = token.mid(pos + 1);
            mSeenOutputWithFormat = true;
        }
    }

    if (fileName == QLatin1String("-"))
        fileName.clear();

    if (!fileName.isNull())
        mOutFileName = Utils::FilePath::fromString(fileName);
    if (!format.isNull()) {
        mParser = (TestOutputFormat) (parserFormats.indexOf(format) + 1);
        if (mParser == NoneFormat) {
            mError = InvalidArgumentError;
            mErrorString = QCoreApplication::translate("QTestLibPlugin::Internal::QTestLibArgsParser", "Got \"%1\" where output format was expected.").arg(format);
            mUnknownFlags << "-o" << token;
        }
    }
}

unsigned int QTestLibArgsParser::parseUnsignedInteger(const QString& token, const QString& cmd, unsigned int defaultValue)
{
    bool ok;
    unsigned int ret = token.toUInt(&ok, 10);
    if (ok)
        return ret;

    mError = InvalidArgumentError;
    mErrorString = QCoreApplication::translate("QTestLibPlugin::Internal::QTestLibArgsParser", "Got \"%1\" where unsigned integer was expected (command \"%2\").").arg(token).arg(cmd);
    mUnknownFlags << cmd << token;
    return defaultValue;
}

int QTestLibArgsParser::parseInteger(const QString& token, const QString& cmd, int defaultValue)
{
    bool ok;
    int ret = token.toInt(&ok, 10);
    if (ok)
        return ret;

    mError = InvalidArgumentError;
    mErrorString = QCoreApplication::translate("QTestLibPlugin::Internal::QTestLibArgsParser", "Got \"%1\" where integer was expected (command \"%2\").").arg(token).arg(cmd);
    mUnknownFlags << cmd << token;
    return defaultValue;
}

QString QTestLibArgsParser::parseString(const QString& token)
{
    QString ret(token.length(), QLatin1Char(' '));
    int i = 0;
    int j = 0;
    bool escaped = false;

    for (; i < token.length(); i++) {
        if (escaped || ((token.at(i) != QLatin1Char('"')) && (token.at(i) != QLatin1Char('\\'))))
            ret.replace(j++, 1, token.at(i));
        escaped = (token.at(i) == QLatin1Char('\\'));
    }

    return ret.trimmed();
}

void QTestLibArgsParser::incrementalParse(void)
{
    if (mError == NoError)
        parse(true);
    else if (mError == PrematureEndError)
        parse(false);
}

void QTestLibArgsParser::initDefaults(void)
{
    mPos = 0;

    mError = NoError;
    mErrorString.clear();

    mUnknownFlags.clear();
    mSelectedTestCases.clear();

    mOutFileName = Utils::FilePath();
    mParser = TxtFormat;
    mVerbosity = NormalVerbosity;
    mOutput = NormalOutput;
    mCrashHandlerEnabled = true;
    mMaxWarnings = 2000;
    // TODO these should be set to their defaults values.
    mEventDelay = -1;
    mKeyDelay = -1;
    mMouseDelay = -1;

    mSeenOutputWithFormat = false;
}

QString QTestLibArgsParser::nextToken(void)
{
    // Skip space
    while((mPos < mArgs.length()) && mArgs.at(mPos).isSpace())
        mPos++;

    // End of string
    if (mPos == mArgs.length())
        return QString();

    int b = mPos;
    bool quoted = false;
    bool escaped = false;

    while(mPos < mArgs.length()) {
        if (!escaped && !quoted && mArgs.at(mPos).isSpace())
            break;
        if (!escaped && (mArgs.at(mPos) == QLatin1Char('"')))
            quoted = !quoted;
        if (mArgs.at(mPos) == QLatin1Char('\\'))
            escaped = !escaped;
        else
            escaped = false;
        mPos++;
    }

    return mArgs.mid(b, mPos - b);
}

} // namespace Internal
} // namespace QTestLibPlugin
