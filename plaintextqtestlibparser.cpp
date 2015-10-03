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

    QRegExp stdoutMagicRegexp1(QLatin1String("Config: Using Qt?Test library (\\d+\\.\\d+\\.\\d+), Qt (\\d+\\.\\d+\\.\\d+)(?: \\((.*)\\))?"));
    QRegExp stdoutMagicRegexp2(QLatin1String("Testing ([A-Za-z_][A-Za-z0-9_]*)"));
    if (mModel == NULL) {
        // Test for begin magic line
        if (stdoutMagicRegexp1.exactMatch(line) || stdoutMagicRegexp2.exactMatch(line)) {
            mModel = new QTestLibModel(runControl);
            return TestModelFactory::MagicFound;
        } else {
            return TestModelFactory::Unsure;
        }
    } else {
        // Test for result line
        QRegExp stdoutResultRegexp(QLatin1String("Totals: (\\d+) passed, (\\d+) failed, (\\d+) skipped(?:, (\\d+) blacklisted)?"));
        if (stdoutMagicRegexp1.exactMatch(line) || stdoutMagicRegexp2.exactMatch(line))
            return TestModelFactory::MagicFound;
        if (stdoutResultRegexp.exactMatch(line))
            return TestModelFactory::Unsure;
    }

    // Tests for normal lines:
    //QRegExp stdoutLineRegexp(QLatin1String("^([^:]):\\s+([A-Za-z_][A-Za-z0-9_])::([A-Za-z_][A-Za-z0-9_])\\((.*)\\)(\\s.*)?$"));
    QRegExp stdoutLineRegexpSignal(QLatin1String("([^:]*):\\s+([A-Za-z_][A-Za-z0-9_]*)::([A-Za-z_][A-Za-z0-9_]*)\\((.*)\\)\\sSignal:\\s(.*)"));
    QRegExp stdoutLineRegexp1(QLatin1String("([^:]*):\\s+([A-Za-z_][A-Za-z0-9_]*)::([A-Za-z_][A-Za-z0-9_]*)\\((.*)\\)(?:\\s|:)(.*)"));
    QRegExp stdoutLineRegexp2(QLatin1String("([^:]*):\\s+([A-Za-z_][A-Za-z0-9_]*)::([A-Za-z_][A-Za-z0-9_]*)\\((.*)\\)"));

    if (stdoutLineRegexpSignal.exactMatch(line))
        mModel->addTestItem(runControl,
                            messageType(stdoutLineRegexpSignal.capturedTexts().at(1).trimmed()),    /* Message type */
                            stdoutLineRegexpSignal.capturedTexts().at(2),                           /* Test class */
                            stdoutLineRegexpSignal.capturedTexts().at(3),                           /* Test function */
                            stdoutLineRegexpSignal.capturedTexts().at(4),                           /* Test case */
                            stdoutLineRegexpSignal.capturedTexts().at(5).trimmed());                /* Message */
    else if (stdoutLineRegexp1.exactMatch(line))
        mModel->addTestItem(runControl,
                            messageType(stdoutLineRegexp1.capturedTexts().at(1).trimmed()), /* Message type */
                            stdoutLineRegexp1.capturedTexts().at(2),                        /* Test class */
                            stdoutLineRegexp1.capturedTexts().at(3),                        /* Test function */
                            stdoutLineRegexp1.capturedTexts().at(4),                        /* Test case */
                            stdoutLineRegexp1.capturedTexts().at(5).trimmed());             /* Message */
    else if (stdoutLineRegexp2.exactMatch(line))
        mModel->addTestItem(runControl,
                            messageType(stdoutLineRegexp2.capturedTexts().at(1).trimmed()), /* Message type */
                            stdoutLineRegexp2.capturedTexts().at(2),                        /* Test class */
                            stdoutLineRegexp2.capturedTexts().at(3),                        /* Test function */
                            stdoutLineRegexp2.capturedTexts().at(4),                        /* Test case */
                            QString::null);
    else
        /* NOTE the message is supposed to be part of the previous one ... */
        mModel->appendTestItemMessage(runControl, line.trimmed());

    return TestModelFactory::Unsure;
}

TestModelFactory::ParseResult PlainTextQTestLibParser::parseStderrLine(ProjectExplorer::RunControl* runControl, const QString& line)
{
    Q_UNUSED(runControl);
    Q_UNUSED(line);

    return TestModelFactory::Unsure;
}

QTestLibModel::MessageType PlainTextQTestLibParser::messageType(const QString& messageType)
{
    int type = -1;
    const QString messages(QLatin1String("RESULT  "
                                         "QDEBUG  "
                                         "INFO    "
                                         "WARNING "
                                         "QWARN   "
                                         "QSYSTEM "
                                         "QFATAL  "
                                         "????????"
                                         "SKIP    "
                                         "PASS    "
                                         "BPASS   "
                                         "XPASS   "
                                         "XFAIL   "
                                         "BFAIL   "
                                         "FAIL!   "));


    // Find the type of the message
    if (!messageType.isEmpty())
        type = messages.indexOf(messageType, 0, Qt::CaseSensitive);
    if (type == -1)
        type = (int) QTestLibModel::Unknown;
    else
        type>>=3;

    return (QTestLibModel::MessageType) type;
}

} // namespace Internal
} // namespace QTestLibPlugin


