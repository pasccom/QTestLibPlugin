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

#include "plaintextqtestlibparser.h"

#include <projectexplorer/runconfiguration.h>

#include <QtDebug>

namespace QTestLibPlugin {
namespace Internal {

TestModelFactory::ParseResult PlainTextQTestLibParser::parseStdoutLine(ProjectExplorer::RunControl* runControl, const QString& line)
{
    // Test for special lines at beginning and end.
    QRegExp stdoutIgnoreRegexp(QLatin1String("^\\*"));
    if (stdoutIgnoreRegexp.indexIn(line) == 0)
        return TestModelFactory::Unsure;

    if (!mParserActive) {
        // Test for begin magic line
        QRegExp stdoutMagicRegexp1(QLatin1String("Config: Using Qt?Test library (\\d+\\.\\d+\\.\\d+), Qt (\\d+\\.\\d+\\.\\d+).*"));
        QRegExp stdoutMagicRegexp2(QLatin1String("Testing ([A-Za-z_][A-Za-z0-9_]*)"));
        if (stdoutMagicRegexp1.exactMatch(line)) {
            if (mModel == NULL)
                mModel = new QTestLibModel(runControl);
            mParserActive = true;
            return TestModelFactory::MagicFound;
        } else if (stdoutMagicRegexp2.exactMatch(line)) {
            if (mModel == NULL)
                mModel = new QTestLibModel(runControl);
            mModel->addClass(stdoutMagicRegexp2.capturedTexts().at(1));
            mParserActive = true;
            return TestModelFactory::MagicFound;
        }else {
            return TestModelFactory::Unsure;
        }
    } else {
        // Test for result line
        QRegExp stdoutResultRegexp(QLatin1String("Totals: (\\d+) passed, (\\d+) failed, (\\d+) skipped(?:, (\\d+) blacklisted)?(?:, (\\d+)ms)?"));
        if (stdoutResultRegexp.exactMatch(line)) {
            mParserActive = false;
            return TestModelFactory::Unsure;
        }
    }

    bool ok = false;
    QString file;
    unsigned int fileLine = 0;
    QRegExp stdoutLocationRegexp1(QLatin1String("   Loc: \\[(.*)\\(([1-9][0-9]+)\\)\\]"));
    QRegExp stdoutLocationRegexp2(QLatin1String("(.*)\\(([1-9][0-9]+)\\) : failure location"));

    if (stdoutLocationRegexp1.exactMatch(line)) {
        // For location lines (new style):
        file = stdoutLocationRegexp1.capturedTexts().at(1);
        fileLine = stdoutLocationRegexp1.capturedTexts().at(2).toUInt(&ok, 10);
    } else if (stdoutLocationRegexp2.exactMatch(line)) {
        // For location lines (old style):
        file = stdoutLocationRegexp2.capturedTexts().at(1);
        fileLine = stdoutLocationRegexp2.capturedTexts().at(2).toUInt(&ok, 10);
    } else {
        // For normal lines:
        QTestLibModel::MessageType messageType = QTestLibModel::Unknown;
        if (!isMessageBeginning(line, &messageType)) {
            /* NOTE the message is supposed to be part of the previous one ... */
            mModel->appendTestItemMessage(runControl, line);
        } else {
            if (!processMessageBeginning(runControl, line.mid(9), messageType))
                /* NOTE the message is supposed to be part of the previous one ... */
                mModel->appendTestItemMessage(runControl, line);
        }
    }

    // For location lines:
    if (!ok)
        fileLine = 0;
    if (!file.isEmpty() && (fileLine != 0)) {
        mModel->appendTestLocation(runControl, file, fileLine);
    }

    return TestModelFactory::Unsure;
}

TestModelFactory::ParseResult PlainTextQTestLibParser::parseStderrLine(ProjectExplorer::RunControl* runControl, const QString& line)
{
    Q_UNUSED(runControl);
    Q_UNUSED(line);

    return TestModelFactory::Unsure;
}

bool PlainTextQTestLibParser::isMessageBeginning(const QString& line, QTestLibModel::MessageType *type)
{
    int t = -1;
    const QString messages(QLatin1String("RESULT  "
                                         "QDEBUG  "
                                         "INFO    "
                                         "WARNING "
                                         "QWARN   "
                                         "QSYSTEM "
                                         "QFATAL  "
                                         "??????  "
                                         "SKIP    "
                                         "PASS    "
                                         "BPASS   "
                                         "XPASS   "
                                         "XFAIL   "
                                         "BFAIL   "
                                         "FAIL!   "));

    // Find the type of the message
    if (line.length() < 8)
        return false;
    if (line.at(7) != QLatin1Char(':'))
        return false;
    if (!line.left(7).trimmed().isEmpty())
        t = messages.indexOf(line.left(7).trimmed(), 0, Qt::CaseSensitive);
    if (t == -1)
        return false;
    t>>=3;
    t+=3;

    if (type != NULL)
        *type = (QTestLibModel::MessageType) t;
    return true;
}

bool PlainTextQTestLibParser::processMessageBeginning(ProjectExplorer::RunControl* runControl, const QString& line, QTestLibModel::MessageType type)
{
    // Find class and function:
    int p = line.indexOf(QLatin1Char('('));
    int c = line.lastIndexOf(QLatin1String("::"), p);

    if ((c < 1) || (p < c + 3))
        return false;

    QString className = line.left(c);
    QString functionName = line.mid(c + 2, p - c - 2);
    //qDebug() << className << functionName;

    // Find row title
    int e = matchBracket(line, p);
    while ((e < line.length() - 1) && (line.at(e + 1) != QLatin1Char(':')) && (line.at(e + 1) != QLatin1Char(' ')))
        e = matchBracket(line, e);
    if (line.at(e) != QLatin1Char(')'))
        return false;

    QString rowTitle = line.mid(p + 1, e - p - 1);
    QString message = line.mid(e + 2);
    //qDebug() << rowTitle << message;

    mModel->addTestItem(runControl, type, className, functionName, rowTitle, message);
    return true;
}

int PlainTextQTestLibParser::matchBracket(const QString& line, int b) const
{
    int l = 0;
    b++;
    while ((b < line.length()) && ((l != 0) || (line.at(b) != QLatin1Char(')')))) {
        if (line.at(b) == QLatin1Char('('))
            l++;
        if (line.at(b) == QLatin1Char(')'))
            l--;
        b++;
    }

    return b;
}

} // namespace Internal
} // namespace QTestLibPlugin


