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

#include "qtestlibargsparser.h"
#include "qtestlibpluginconstants.h"

#include <utils/qtcassert.h>

#include <QCoreApplication>
#include <QLinkedList>
#include <QtDebug>

namespace QTestLibPlugin {
namespace Internal {

void QTestLibArgsParser::toMap(QVariantMap& map) const
{
    QTC_ASSERT(mOutput == NormalOutput, );

    if (mParser != TxtFormat)
        map.insert(Constants::FormatKey, (int) mParser);
    if (mVerbosity != NormalVerbosity)
        map.insert(Constants::VerbosityKey, (int) mVerbosity);
    if (!mOutFileName.isEmpty())
        map.insert(Constants::OutputFileKey, mOutFileName.toString());
    if (mMaxWarnings != 2000)
        map.insert(Constants::MaxWarningKey, mMaxWarnings);
    if (mEventDelay > 0)
        map.insert(Constants::EventDelayKey, mEventDelay);
    if (mKeyDelay > 0)
        map.insert(Constants::KeyDelayKey, mKeyDelay);
    if (mMouseDelay > 0)
        map.insert(Constants::MouseDelayKey, mMouseDelay);
}

void QTestLibArgsParser::fromMap(const QVariantMap& map)
{
    QTC_ASSERT(mOutput == NormalOutput, );

    mParser = (TestOutputFormat) map.value(Constants::FormatKey, (int) TxtFormat).toInt();
    mVerbosity = (TestVerbosity) map.value(Constants::VerbosityKey, (int) NormalVerbosity).toInt();
    mOutFileName = Utils::FileName::fromString(map.value(Constants::OutputFileKey).toString());
    mMaxWarnings = map.value(Constants::MaxWarningKey, 2000).toUInt();
    mEventDelay = map.value(Constants::EventDelayKey, -1).toInt();
    mKeyDelay = map.value(Constants::KeyDelayKey, -1).toInt();
    mMouseDelay = map.value(Constants::MouseDelayKey, -1).toInt();

    qDebug() << mMaxWarnings;
    qDebug() << mEventDelay;
    qDebug() << mKeyDelay;
    qDebug() << mMouseDelay;
}


QStringList QTestLibArgsParser::toStringList(uint version) const
{
    Q_ASSERT((version == 1) || (version == 2));

    QStringList ret;
    QStringList formats;
    QStringList verbosities;
    QStringList outputs;
    QString outFile(mOutFileName.toString());
    outFile.replace(QLatin1Char(' '), QLatin1String("\\ "));

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
                    ret << outFile + QLatin1String(",") + formats.at((int) mParser);
            }
        }

        if (mVerbosity != NormalVerbosity)
            ret << verbosities.at((int) mVerbosity + 1);
        if (!mCrashHandlerEnabled)
            ret << QLatin1String("-noccrashhandler");
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
            currentFlag = knownFlags.indexOf(token.mid(1));
            qDebug() << currentFlag;
            switch (currentFlag) {
            case -1: // Unknown flag
            case 7: // Impossible
                if (!token.startsWith(QLatin1Char('-'))) {
                    parseSelectedTest(token);
                } else {
                    mError = UnknownFlagError;
                    mErrorString = QCoreApplication::translate("QTestLibPlugin::Internal::QTestLibArgsParser", "Unknown flag \"%1\"").arg(token);
                }
                break;
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
                mEventDelay = parseInteger(token, mEventDelay);
                break;
            case 17: // -keydelay
                mKeyDelay = parseInteger(token, mKeyDelay);
                break;
            case 18: // -mousedelay
                mMouseDelay = parseInteger(token, mMouseDelay);
                break;
            case 19: // -maxwarnings
                mMaxWarnings = parseUnsignedInteger(token, mMaxWarnings);
                break;
            }
        }
        currentFlag = -1;
    }

    if (currentFlag != -1) {
        mError = PrematureEndError;
        mErrorString = QCoreApplication::translate("QTestLibPlugin::Internal::QTestLibArgsParser", "String of command line arguments ended prematurely");
    }
}

void QTestLibArgsParser::parseSelectedTest(const QString& token)
{
    int colon = token.indexOf(QLatin1Char(':'), 0);

    QString selectedTestCase;
    QString selectedTestData;

    if (colon == -1) {
        selectedTestCase = token;
        selectedTestData = QString::null;
    } else {
        selectedTestCase = token.left(colon);
        selectedTestData = token.mid(colon + 1);
    }

    TestCaseList::iterator it = mSelectedTestCases.begin();
    for (; it != mSelectedTestCases.end(); it++)
        if (QString::compare((*it).first, selectedTestCase, Qt::CaseSensitive) == 0)
            break;

    if (it == mSelectedTestCases.end()) {
        if (!selectedTestCase.isNull())
            mSelectedTestCases.append(QPair<QString, QStringList>(selectedTestCase, QStringList()));
        else
            mSelectedTestCases.append(QPair<QString, QStringList>(selectedTestCase, QStringList() << selectedTestData));
    } else {
        if (!selectedTestCase.isNull()) {
            (*it).second.clear();
        } else {
            if (!(*it).second.contains(selectedTestData))
                (*it).second.append(selectedTestData);
        }
    }
}

void QTestLibArgsParser::parseOutput(const QString& token)
{
    QStringList parserFormats;
    parserFormats << QLatin1String("txt") << QLatin1String("csv") << QLatin1String("xunitxml") << QLatin1String("xml") << QLatin1String("lightxml");

    QString fileName = QString::null;
    QString format = QString::null;

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
        fileName = QString::null;

    if (!fileName.isNull())
        mOutFileName = Utils::FileName::fromString(fileName);
    if (!format.isNull()) {
        mParser = (TestOutputFormat) (parserFormats.indexOf(format) + 1);
        if (mParser == NoneFormat) {
            mError = InvalidArgumentError;
            mErrorString = QCoreApplication::translate("QTestLibPlugin::Internal::QTestLibArgsParser", "Got \"%1\" where output format was expected.").arg(format);
        }
    }
}

unsigned int QTestLibArgsParser::parseUnsignedInteger(const QString& token, unsigned int defaultValue)
{
    bool ok;
    unsigned int ret = token.toUInt(&ok, 10);
    if (ok)
        return ret;

    mError = InvalidArgumentError;
    mErrorString = QCoreApplication::translate("QTestLibPlugin::Internal::QTestLibArgsParser", "Got \"%1\" where unsigned integer was expected.").arg(token);
    return defaultValue;
}

int QTestLibArgsParser::parseInteger(const QString& token, int defaultValue)
{
    bool ok;
    int ret = token.toInt(&ok, 10);
    if (ok)
        return ret;

    mError = InvalidArgumentError;
    mErrorString = QCoreApplication::translate("QTestLibPlugin::Internal::QTestLibArgsParser", "Got \"%1\" where integer was expected.").arg(token);
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
    mErrorString = QString::null;

    mSelectedTestCases.clear();

    mOutFileName = Utils::FileName();
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
        return QString::null;

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

void QTestLibArgsParser::removeTestCases(const QString& function, const QString& dataTag)
{
    TestCaseList::iterator fIt;

    for (fIt = mSelectedTestCases.begin(); fIt != mSelectedTestCases.end();) {
        if (!function.isNull() && (QString::compare((*fIt).first, function, Qt::CaseInsensitive) == 0)) {
            fIt = mSelectedTestCases.erase(fIt);
        } else {
            if ((*fIt).second.contains(dataTag)) {
                QStringList tags = (*fIt).second;
                QStringList::iterator tIt;

                for (tIt = tags.begin(); tIt != tags.end();) {
                    if (!dataTag.isNull() && (QString::compare(*tIt, dataTag, Qt::CaseSensitive) == 0))
                        tIt = tags.erase(tIt);
                    else
                        tIt++;
                }

                fIt = mSelectedTestCases.insert(fIt, QPair<QString, QStringList>((*fIt).first, tags));
                fIt = mSelectedTestCases.erase(--fIt);
            }

            fIt++;
        }
    }
}

} // namespace Internal
} // namespace QTestLibPlugin
