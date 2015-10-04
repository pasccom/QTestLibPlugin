#include "xmlqtestlibparser.h"

#include <projectexplorer/runconfiguration.h>

#include <QXmlStreamReader>
#include <QtDebug>

namespace QTestLibPlugin {
namespace Internal {

XMLQTestLibParser::XMLQTestLibParser(QObject *parent) :
    AbstractTestParser(parent), mModel(NULL)
{
   mReader = new QXmlStreamReader();
}

TestModelFactory::ParseResult XMLQTestLibParser::parseStdoutLine(ProjectExplorer::RunControl* runControl, const QString& line)
{
    TestModelFactory::ParseResult result = TestModelFactory::Unsure;

    mReader->addData(line.trimmed());

    while(!mReader->atEnd()) {
        QXmlStreamReader::TokenType currentToken = mReader->readNext();

        if (currentToken == QXmlStreamReader::Invalid) {
            if (mReader->error() == QXmlStreamReader::PrematureEndOfDocumentError)
                return result;
            else
                return TestModelFactory::MagicNotFound;
        }

        if (mCurrentElement.isEmpty()) {
            if ((currentToken != QXmlStreamReader::StartDocument)
             && (currentToken != QXmlStreamReader::EndDocument)
             && (currentToken != QXmlStreamReader::StartElement))
                return TestModelFactory::MagicNotFound;
        }

        switch(currentToken) {
        case QXmlStreamReader::NoToken:
            break;
        case QXmlStreamReader::Invalid:
            Q_ASSERT_X(false, "Unreachable place", "The program cannot reach this point");
        case QXmlStreamReader::StartDocument:
            break;
        case QXmlStreamReader::EndDocument:
            mReader->clear();
            qDebug() << "End of document";
            break;
        case QXmlStreamReader::StartElement:
            mCurrentElement.push(mReader->name().toString());
            result = qMax(result, startElementParsed(runControl, mReader->name()));
            break;
        case QXmlStreamReader::EndElement:
            if (QStringRef::compare(mReader->name(), mCurrentElement.pop(), Qt::CaseSensitive) != 0)
                return TestModelFactory::MagicNotFound;
            result = qMax(result, endElementParsed(runControl, mReader->name()));
            break;
        case QXmlStreamReader::Characters:
            result = qMax(result, textParsed(runControl));
            break;
        case QXmlStreamReader::Comment:
            qDebug() << "Comment";
            break;
        case QXmlStreamReader::DTD:
            qDebug() << "Seen DTD";
            break;
        case QXmlStreamReader::EntityReference:
            qDebug() << "Entity reference";
            break;
        case QXmlStreamReader::ProcessingInstruction:
            qDebug() << "processing instruction";
            break;
        }
    }

    return result;
}

TestModelFactory::ParseResult XMLQTestLibParser::parseStderrLine(ProjectExplorer::RunControl* runControl, const QString& line)
{
    Q_UNUSED(runControl);
    Q_UNUSED(line);

    return TestModelFactory::Unsure;
}

TestModelFactory::ParseResult XMLQTestLibParser::startElementParsed(ProjectExplorer::RunControl* runControl, const QStringRef& tag)
{
    if (QStringRef::compare(tag, QLatin1String("TestCase"), Qt::CaseSensitive) == 0)
        mCurrentClass = mReader->attributes().value(QLatin1String("name")).toString();
    if (QStringRef::compare(tag, QLatin1String("TestFunction"), Qt::CaseSensitive) == 0)
        mCurrentFunction = mReader->attributes().value(QLatin1String("name")).toString();

    if ((QStringRef::compare(tag, QLatin1String("Incident"), Qt::CaseInsensitive) == 0)
     || (QStringRef::compare(tag, QLatin1String("Message"), Qt::CaseInsensitive) == 0))
        saveAttributes(mReader->attributes());
    if (QStringRef::compare(tag, QLatin1String("Duration"), Qt::CaseInsensitive) == 0)
        saveAttributes(mReader->attributes());
    if (QStringRef::compare(tag, QLatin1String("BenchmarkResult"), Qt::CaseInsensitive) == 0)
        saveAttributes(mReader->attributes());

    return TestModelFactory::Unsure;
}

TestModelFactory::ParseResult XMLQTestLibParser::endElementParsed(ProjectExplorer::RunControl* runControl, const QStringRef& tag)
{
    if (QStringRef::compare(tag, QLatin1String("Environment"), Qt::CaseSensitive) == 0) {
        if (!mQtVersion.isNull() && !mQtBuild.isNull() && !mQTestLibVersion.isNull()) {
            qDebug() << "Create QTestLibMOdel" << mQtVersion << mQtBuild << mQTestLibVersion;
            if (mModel ==  NULL)
                mModel = new QTestLibModel(runControl);
            return TestModelFactory::MagicFound;
        } else {
            return TestModelFactory::MagicNotFound;
        }
    }

    if (QStringRef::compare(tag, QLatin1String("TestCase"), Qt::CaseSensitive) == 0)
        mCurrentClass = QString::null;
    if (QStringRef::compare(tag, QLatin1String("TestFunction"), Qt::CaseSensitive) == 0)
        mCurrentFunction = QString::null;

    if ((QStringRef::compare(tag, QLatin1String("Incident"), Qt::CaseInsensitive) == 0)
     || (QStringRef::compare(tag, QLatin1String("Message"), Qt::CaseInsensitive) == 0)) {
        if (mModel != NULL)
            mModel->addTestItem(runControl,
                                currentMessageType(),
                                mCurrentClass,
                                mCurrentFunction,
                                mCurrentRow,
                                mCurrentDescription);
        if (!mCurrentAttributes.value(QLatin1String("file")).isNull() && (QString::compare(mCurrentAttributes.value(QLatin1String("line"), QLatin1String("0")), "0") != 0))
            mModel->appendTestItemMessage(runControl,
                                          QString(QLatin1String("Loc: [%1(%2)]")).arg(mCurrentAttributes.value("file"))
                                                                                 .arg(mCurrentAttributes.value("line")));
        mCurrentRow = QString::null;
        mCurrentDescription = QString::null;
        mCurrentAttributes.clear();
    }
    if (QStringRef::compare(tag, QLatin1String("Duration"), Qt::CaseInsensitive) == 0) {
        if ((mModel != NULL) && mCurrentAttributes.contains(QLatin1String("msecs")))
            mModel->addTestItem(runControl,
                                QTestLibModel::Info,
                                mCurrentClass,
                                mCurrentFunction,
                                mCurrentRow,
                                trUtf8("Duration: %1ms.").arg(mCurrentAttributes.value(QLatin1String("msecs"))));
        mCurrentAttributes.clear();
    }
    if (QStringRef::compare(tag, QLatin1String("BenchmarkResult"), Qt::CaseInsensitive) == 0) {
        if (mModel != NULL)
            mModel->addTestItem(runControl,
                                QTestLibModel::BenchmarkResult,
                                mCurrentClass,
                                mCurrentFunction,
                                mCurrentRow,
                                trUtf8("%1 msecs per iteration (iterations: %2)").arg(mCurrentAttributes.value(QLatin1String("value")))
                                                                                 .arg(mCurrentAttributes.value(QLatin1String("iterations"))));
        mCurrentAttributes.clear();
    }

    return TestModelFactory::Unsure;
}

TestModelFactory::ParseResult XMLQTestLibParser::textParsed(ProjectExplorer::RunControl* runControl)
{
    if (QString::compare(mCurrentElement.top(), QLatin1String("QtVersion"), Qt::CaseInsensitive) == 0)
        mQtVersion = mReader->text().toString();
    if (QString::compare(mCurrentElement.top(), QLatin1String("QtBuild"), Qt::CaseInsensitive) == 0)
        mQtBuild = mReader->text().toString();
    if (QString::compare(mCurrentElement.top(), QLatin1String("QTestVersion"), Qt::CaseInsensitive) == 0)
        mQTestLibVersion = mReader->text().toString();

    if (QString::compare(mCurrentElement.top(), QLatin1String("DataTag"), Qt::CaseInsensitive) == 0)
        mCurrentRow = mReader->text().toString();
    if (QString::compare(mCurrentElement.top(), QLatin1String("Description"), Qt::CaseInsensitive) == 0)
        mCurrentDescription = mReader->text().toString();

    return TestModelFactory::Unsure;
}

void XMLQTestLibParser::saveAttributes(const QXmlStreamAttributes& attrs)
{
    Q_ASSERT(mCurrentAttributes.isEmpty());

    foreach (QXmlStreamAttribute attr, attrs) {
        Q_ASSERT(!mCurrentAttributes.contains(attr.name().toString()));
        mCurrentAttributes.insert(attr.name().toString(), attr.value().toString());
    }
}

QTestLibModel::MessageType XMLQTestLibParser::currentMessageType(void)
{
    int type = -1;
    const QString messageType = mCurrentAttributes.value(QLatin1String("type"));
    const QString messages(QLatin1String("result  "
                                         "qdebug  "
                                         "info    "
                                         "warn    "
                                         "qwarn   "
                                         "system  "
                                         "qfatal  "
                                         "????????"
                                         "skip    "
                                         "pass    "
                                         "bpass   "
                                         "xpass   "
                                         "xfail   "
                                         "bfail   "
                                         "fail    "));

    Q_ASSERT(messageType.length() <= 8);

    // Find the type of the message
    if (!messageType.isEmpty())
        type = messages.indexOf(messageType.leftJustified(8, QLatin1Char(' '), false), 0, Qt::CaseSensitive);
    if (type == -1)
        type = (int) QTestLibModel::Unknown;
    else
        type>>=3;

    return (QTestLibModel::MessageType) type;
}

} // namespace Internal
} // namespace QTestLibPlugin


