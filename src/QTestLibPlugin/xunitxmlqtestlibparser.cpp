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

#include "xunitxmlqtestlibparser.h"

#include <projectexplorer/runconfiguration.h>

#include <QXmlStreamReader>
#include <QtDebug>

namespace QTestLibPlugin {
namespace Internal {

TestModelFactory::ParseResult XUnitXMLQTestLibParser::startElementParsed(ProjectExplorer::RunControl* runControl, const QStringRef& tag)
{
    Q_UNUSED(runControl);

    if (QStringRef::compare(tag, QLatin1String("property"), Qt::CaseSensitive) == 0) {
        if (QStringRef::compare(mReader->attributes().value(QLatin1String("name")), QLatin1String("QTestVersion"), Qt::CaseSensitive) == 0)
            mQTestLibVersion = decodeXMLEntities(mReader->attributes().value(QLatin1String("value")).toString());
        else if (QStringRef::compare(mReader->attributes().value(QLatin1String("name")), QLatin1String("QtVersion"), Qt::CaseSensitive) == 0)
            mQtVersion = decodeXMLEntities(mReader->attributes().value(QLatin1String("value")).toString());
        else if (QStringRef::compare(mReader->attributes().value(QLatin1String("name")), QLatin1String("QtBuild"), Qt::CaseSensitive) == 0)
            mQtBuild = decodeXMLEntities(mReader->attributes().value(QLatin1String("value")).toString());
        else
            qWarning() << "Unknown build property:" << mReader->attributes().value(QLatin1String("name"));
        return TestModelFactory::Unsure;
    }

    if (QStringRef::compare(tag, QLatin1String("testsuite"), Qt::CaseSensitive) == 0)
        mCurrentClass = mReader->attributes().value(QLatin1String("name")).toString();
    if (QStringRef::compare(tag, QLatin1String("testcase"), Qt::CaseSensitive) == 0) {
        mCurrentFunction = mReader->attributes().value(QLatin1String("name")).toString();
        mCurrentMessageType = currentMessageType();
    }
    if (QStringRef::compare(tag, QLatin1String("failure"), Qt::CaseInsensitive) == 0) {
        if (mModel != NULL) {
            if (mReader->attributes().hasAttribute(QLatin1String("tag")))
                mModel->addTestItem(runControl, currentMessageType(), mCurrentClass, mCurrentFunction, mReader->attributes().value(QLatin1String("tag")).toString(), decodeXMLEntities(mReader->attributes().value(QLatin1String("message")).toString()));
            else
                mModel->addTestItem(runControl, currentMessageType(), mCurrentClass, mCurrentFunction, QString::null, decodeXMLEntities(mReader->attributes().value(QLatin1String("message")).toString()));
        }
        if (mCurrentMessageType == currentMessageType())
            mCurrentMessageType = QTestLibModel::Unknown;
    }

    return TestModelFactory::Unsure;
}

TestModelFactory::ParseResult XUnitXMLQTestLibParser::endElementParsed(ProjectExplorer::RunControl* runControl, const QStringRef& tag)
{
    if (QStringRef::compare(tag, QLatin1String("properties"), Qt::CaseSensitive) == 0) {
        if (!mQtVersion.isNull() && !mQtBuild.isNull() && !mQTestLibVersion.isNull()) {
            qDebug() << "Create QTestLibModel" << mQtVersion << mQtBuild << mQTestLibVersion;
            if (mModel ==  NULL)
                mModel = new QTestLibModel(runControl);
            return TestModelFactory::MagicFound;
        } else {
            return TestModelFactory::MagicNotFound;
        }
    }

    if (QStringRef::compare(tag, QLatin1String("testsuite"), Qt::CaseSensitive) == 0)
        mCurrentClass = QString::null;
    if (QStringRef::compare(tag, QLatin1String("testcase"), Qt::CaseSensitive) == 0) {
        if ((mCurrentMessageType != QTestLibModel::Unknown) && (mModel != NULL))
            mModel->addTestItem(runControl, mCurrentMessageType, mCurrentClass, mCurrentFunction, QString::null, QString::null);
        mCurrentFunction = QString::null;
        mCurrentMessageType = QTestLibModel::Unknown;
    }

    return TestModelFactory::Unsure;
}

TestModelFactory::ParseResult XUnitXMLQTestLibParser::commentParsed(ProjectExplorer::RunControl* runControl)
{
    QString message = QString::null;
    QString tag = QString::null;
    QTestLibModel::MessageType type = QTestLibModel::Unknown;

    QRegExp messageRegExp(QLatin1String("message=\"([^\"]*)\""), Qt::CaseSensitive);
    QRegExp tagRegExp(QLatin1String("tag=\"([^\"]*)\""), Qt::CaseSensitive);
    QRegExp typeRegExp(QLatin1String("type=\"(result|qdebug|info|warn|qwarn|system|qfatal|\\?\\?\\?\\?\\?\\?|skip|pass|bpass|xpass|xfail|bfail|fail)\""), Qt::CaseSensitive);

    if (messageRegExp.indexIn(mReader->text().toString()) != -1)
        message = decodeXMLEntities(messageRegExp.capturedTexts().at(1));
    if (tagRegExp.indexIn(mReader->text().toString()) != -1)
        tag = tagRegExp.capturedTexts().at(1);
    if (typeRegExp.indexIn(mReader->text().toString()) != -1)
        type = messageType(typeRegExp.capturedTexts().at(1));

    qDebug() << "Comment:" << message << type << tag;

    if (mModel != NULL)
        mModel->addTestItem(runControl, type, mCurrentClass, mCurrentFunction, tag, message);

    return TestModelFactory::Unsure;
}

} // namespace Internal
} // namespace QTestLibPlugin


