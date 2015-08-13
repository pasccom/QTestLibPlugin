#include "testmodelfactory.h"
#include "plaintextqtestlibparser.h"

#include <projectexplorer/runconfiguration.h>

#include <QtDebug>

namespace QTestLibPlugin {
namespace Internal {

TestModelFactory::TestModelFactory(ProjectExplorer::RunControl *runControl, QObject *parent)
    : QObject(parent), mModelFound(false)
{
    qDebug() << "Run control started:" << runControl->displayName();

    mParsers.append(new PlainTextQTestLibParser(this));

    connect(runControl, SIGNAL(appendMessage(ProjectExplorer::RunControl*, const QString&, Utils::OutputFormat)),
            this, SLOT(parseTestOutput(ProjectExplorer::RunControl*, const QString&, Utils::OutputFormat)));
    connect(runControl, SIGNAL(finished()),
            this, SLOT(runControlFinished()));
}

void TestModelFactory::parseTestOutput(ProjectExplorer::RunControl * runControl, const QString & msg, Utils::OutputFormat format)
{
    foreach (QString line, msg.split(QRegExp(QLatin1String("[\n\r]")))) {
        if (line.isEmpty())
            continue;

        switch (format) {
        case Utils::OutputFormat::NormalMessageFormat:
            qDebug() << "Normal message" << line;
            break;
        case Utils::OutputFormat::ErrorMessageFormat:
            qDebug() << "Error message" << line;
            break;
        case Utils::OutputFormat::DebugFormat:
            qDebug() << "Debug message" << line;
            break;
        case Utils::OutputFormat::StdOutFormat:
            parseStdoutLine(runControl, line);
            break;
        case Utils::OutputFormat::StdErrFormat:
            qDebug() << "Stderr message" << line;
            break;
        case Utils::OutputFormat::StdOutFormatSameLine:
            parseStdoutLine(runControl, line);
            break;
        case Utils::OutputFormat::StdErrFormatSameLine:
            qDebug() << "Stderr same line message" << line;
            break;
        default:
            break;
        }
    }
}

void TestModelFactory::parseStdoutLine(ProjectExplorer::RunControl* runControl, const QString& line)
{
    if (mModelFound) {
        mParsers.first()->parseStdoutLine(runControl, line);
    } else {
        QLinkedList<AbstractTestParser *>::iterator it;
        ParseResult currentResult;

        for (it = mParsers.begin(); it != mParsers.end();) {
            currentResult = (*it)->parseStdoutLine(runControl, line);

            if (currentResult == MagicFound)
                break;
            else if (currentResult == MagicNotFound)
                it = mParsers.erase(it);
            else
                it++;
        }

        if (currentResult == MagicFound) {
            AbstractTestParser *current = *it;
            mModelFound = true;
            for (it = mParsers.begin(); it != mParsers.end();) {
                if (*it != current) {
                    delete *it;
                    it = mParsers.erase(it);
                } else {
                    it++;
                }
            }
            Q_ASSERT(mParsers.size() == 1);
            emit modelFound(current->getModel());
        }
    }
}

void TestModelFactory::runControlFinished(void)
{
    if (mModelFound) {
        Q_ASSERT(mParsers.size() == 1);
        emit modelPopulated(mParsers.first()->getModel());
    }
    deleteLater();
}


} // namespace Internal
} // namespace QTestLibPlugin
