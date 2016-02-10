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

#include "xmlqtestlibparser.h"

#include <projectexplorer/runconfiguration.h>

#include <QXmlStreamReader>
#include <QtDebug>

namespace QTestLibPlugin {
namespace Internal {

XMLQTestLibParser::XMLQTestLibParser(QObject *parent) :
    AbstractTestParser(parent), mModel(NULL), mParserActive(false)
{
   mReader = new QXmlStreamReader();
}

TestModelFactory::ParseResult XMLQTestLibParser::parseStdoutLine(ProjectExplorer::RunControl* runControl, const QString& line)
{
    TestModelFactory::ParseResult result = TestModelFactory::Unsure;

    QString cleanedLine = line.trimmed() + QLatin1Char('\n');
    qDebug() << cleanedLine;
    if (!mParserActive && !cleanedLine.contains(QLatin1String("<?xml")))
        return result;
    if (!mParserActive)
        cleanedLine = cleanedLine.mid(cleanedLine.indexOf(QLatin1String("<?xml"), 0));
    mParserActive = true;

    mReader->addData(cleanedLine);

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
            mParserActive = false;
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
    Q_UNUSED(runControl);

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
            mModel->addTestItem(runControl, currentMessageType(), mCurrentClass, mCurrentFunction, mCurrentRow, mCurrentDescription);

        QString file = mCurrentAttributes.value(QLatin1String("file"));
        bool ok = false;
        unsigned int fileLine = mCurrentAttributes.value(QLatin1String("line"), QLatin1String("0")).toUInt(&ok, 10);
        if (!ok)
            fileLine = 0;
        if ((mModel != NULL) && !file.isEmpty() && (fileLine != 0))
            mModel->appendTestLocation(runControl, file, fileLine);

        mCurrentRow = QString::null;
        mCurrentDescription = QString::null;
        mCurrentAttributes.clear();
    }
    if (QStringRef::compare(tag, QLatin1String("Duration"), Qt::CaseInsensitive) == 0) {
        if ((mModel != NULL) && mCurrentAttributes.contains(QLatin1String("msecs")))
            mModel->addTestItem(runControl,
                                QTestLibModel::Duration,
                                mCurrentClass,
                                mCurrentFunction,
                                mCurrentRow,
                                trUtf8("%1ms").arg(mCurrentAttributes.value(QLatin1String("msecs"))));
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
    Q_UNUSED(runControl);

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
                                         "??????  "
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
        type = (type >> 3) + 3;

    return (QTestLibModel::MessageType) type;
}

} // namespace Internal
} // namespace QTestLibPlugin

