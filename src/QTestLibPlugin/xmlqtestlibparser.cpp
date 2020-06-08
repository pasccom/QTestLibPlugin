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

#include "xmlqtestlibparser.h"

#include <projectexplorer/runconfiguration.h>

#include <QXmlStreamReader>
#include <QtDebug>

namespace QTestLibPlugin {
namespace Internal {

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
        mCurrentClass.clear();
    if (QStringRef::compare(tag, QLatin1String("TestFunction"), Qt::CaseSensitive) == 0)
        mCurrentFunction.clear();

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

        mCurrentRow.clear();
        mCurrentDescription.clear();
        mCurrentAttributes.clear();
    }
    if (QStringRef::compare(tag, QLatin1String("Duration"), Qt::CaseInsensitive) == 0) {
        if ((mModel != NULL) && mCurrentAttributes.contains(QLatin1String("msecs")))
            mModel->addTestItem(runControl,
                                QTestLibModel::Duration,
                                mCurrentClass,
                                mCurrentFunction,
                                mCurrentRow,
                                tr("%1ms").arg(mCurrentAttributes.value(QLatin1String("msecs"))));
        mCurrentAttributes.clear();
    }
    if (QStringRef::compare(tag, QLatin1String("BenchmarkResult"), Qt::CaseInsensitive) == 0) {
        if (mModel != NULL)
            mModel->addTestItem(runControl,
                                QTestLibModel::BenchmarkResult,
                                mCurrentClass,
                                mCurrentFunction,
                                mCurrentRow,
                                tr("%1 msecs per iteration (iterations: %2)").arg(mCurrentAttributes.value(QLatin1String("value")))
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

} // namespace Internal
} // namespace QTestLibPlugin


