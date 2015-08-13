#include "plaintextqtestlibparser.h"

#include <projectexplorer/runconfiguration.h>

#include <QtDebug>

namespace QTestLibPlugin {
namespace Internal {

PlainTextQTestLibParser::PlainTextQTestLibParser(QObject *parent)
    : AbstractTestParser(parent), mModel(NULL)
{

}

TestModelFactory::ParseResult PlainTextQTestLibParser::parseStdoutLine(ProjectExplorer::RunControl* runControl, const QString& line)
{
    // Test for special lines at beginning and end.
    QRegExp stdoutIgnoreRegexp(QLatin1String("^\\*"));
    if (stdoutIgnoreRegexp.indexIn(line) == 0)
        return TestModelFactory::Unsure;

    if (mModel == NULL) {
        // Test for begin magic line
        QRegExp stdoutMagicRegexp(QLatin1String("Config: Using QtTest library (\\d+\\.\\d+\\.\\d+), Qt (\\d+\\.\\d+\\.\\d+) \\((.*)\\)"));
        if (stdoutMagicRegexp.exactMatch(line)) {
            mModel = new QTestLibModel(runControl);
            return TestModelFactory::MagicFound;
        } else {
            return TestModelFactory::Unsure;
        }
    } else {
        // Test for result line
        QRegExp stdoutResultRegexp(QLatin1String("Totals: (\\d+) passed, (\\d+) failed, (\\d+) skipped, (\\d+) blacklisted"));
        if (stdoutResultRegexp.exactMatch(line))
            return TestModelFactory::Unsure;
    }

    // Tests for normal lines:
    //QRegExp stdoutLineRegexp(QLatin1String("^([^:]):\\s+([A-Za-z_][A-Za-z0-9_])::([A-Za-z_][A-Za-z0-9_])\\((.*)\\)(\\s.*)?$"));
    QRegExp stdoutLineRegexp1(QLatin1String("([^:]*):\\s+([A-Za-z_][A-Za-z0-9_]*)::([A-Za-z_][A-Za-z0-9_]*)\\((.*)\\)\\s(.*)"));
    QRegExp stdoutLineRegexp2(QLatin1String("([^:]*):\\s+([A-Za-z_][A-Za-z0-9_]*)::([A-Za-z_][A-Za-z0-9_]*)\\((.*)\\)"));

    if (stdoutLineRegexp1.exactMatch(line))
        mModel->addTestItem(runControl,
                            stdoutLineRegexp1.capturedTexts().at(1).trimmed(),  /* Message type */
                            stdoutLineRegexp1.capturedTexts().at(2),            /* Test class */
                            stdoutLineRegexp1.capturedTexts().at(3),            /* Test function */
                            stdoutLineRegexp1.capturedTexts().at(4),            /* Test case */
                            stdoutLineRegexp1.capturedTexts().at(5).trimmed()); /* Message */
    else if (stdoutLineRegexp2.exactMatch(line))
        mModel->addTestItem(runControl,
                            stdoutLineRegexp2.capturedTexts().at(1).trimmed(),  /* Message type */
                            stdoutLineRegexp2.capturedTexts().at(2),            /* Test class */
                            stdoutLineRegexp2.capturedTexts().at(3),            /* Test function */
                            stdoutLineRegexp2.capturedTexts().at(4),            /* Test case */
                            QString::null);                                     /* Message */
    else
        mModel->addTestItem(runControl,
                            QString::null,  /* Message type */
                            QString::null,  /* Test class */
                            QString::null,  /* Test function */
                            QString::null,  /* Test case */
                            line.trimmed());/* Message */

    return TestModelFactory::Unsure;
}

} // namespace Internal
} // namespace QTestLibPlugin


