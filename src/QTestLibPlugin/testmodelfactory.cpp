/* Copyright 2015-2016 Pascal COMBES <pascom@orange.fr>
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

#include "testmodelfactory.h"

#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/runcontrol.h>

#include <extensionsystem/pluginmanager.h>

#include <QtDebug>

namespace QTestLibPlugin {
namespace Internal {

QLinkedList<AbstractTestParserFactory *> TestModelFactory::mParserFactories;

AbstractTestParserFactory::AbstractTestParserFactory(void)
{
    TestModelFactory::mParserFactories.append(this);
}

TestModelFactory::TestModelFactory(ProjectExplorer::RunControl *runControl, QObject *parent)
    : QObject(parent), mModelFound(false)
{
    qDebug() << "Run control started:" << runControl->displayName();

    foreach (AbstractTestParserFactory* factory, mParserFactories) {
        if (!factory->canParse(runControl->runConfiguration()))
            continue;

        mParsers.append(factory->getParserInstance(runControl->runConfiguration()));
        Q_ASSERT(mParsers.last() != NULL);
    }

    connect(runControl, SIGNAL(appendMessage(const QString&, Utils::OutputFormat)),
            this, SLOT(parseTestOutput(const QString&, Utils::OutputFormat)));
    connect(runControl, SIGNAL(stopped()),
            this, SLOT(runControlStopped()));
}

QLinkedList<AbstractTestParserFactory*> TestModelFactory::parserFactories(Core::Id id)
{
    if (!id.isValid())
        return mParserFactories;

    QLinkedList<AbstractTestParserFactory*> parserFactories;
    foreach (AbstractTestParserFactory* factory, mParserFactories) {
        if (factory->id() == id)
            parserFactories << factory;
    }

    if (parserFactories.isEmpty()) {
        foreach (AbstractTestParserFactory* factory, mParserFactories) {
            if (!factory->id().suffixAfter(id).isNull())
                parserFactories << factory;
        }
    }

    return parserFactories;
}

void TestModelFactory::parseTestOutput(const QString& msg, Utils::OutputFormat format)
{
    foreach (QString line, msg.split(QRegExp(QLatin1String("[\n\r]")))) {
        if (line.isEmpty())
            continue;

        callParsers(qobject_cast<ProjectExplorer::RunControl*>(sender()), line, format);
    }
}

void TestModelFactory::runControlStopped(void)
{
    if (mModelFound) {
        Q_ASSERT(mParsers.size() == 1);
        emit modelPopulated(mParsers.first()->getModel());
    }
    deleteLater();
}

void TestModelFactory::callParsers(ProjectExplorer::RunControl * runControl, const QString & line, Utils::OutputFormat format)
{
    if (mModelFound) {
        callParser(mParsers.first(), runControl, line, format);
    } else {
        QLinkedList<AbstractTestParser *>::iterator it;
        ParseResult currentResult;

        for (it = mParsers.begin(); it != mParsers.end();) {
            currentResult = callParser(*it, runControl, line, format);

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

TestModelFactory::ParseResult TestModelFactory::callParser(AbstractTestParser* parser, ProjectExplorer::RunControl* runControl, const QString& line, Utils::OutputFormat format)
{
    switch (format) {
    case Utils::OutputFormat::NormalMessageFormat:
        qDebug() << "Normal message" << line;
        break;
    case Utils::OutputFormat::ErrorMessageFormat:
        qDebug() << "Error message" << line;
        break;
    case Utils::OutputFormat::DebugFormat:
        return parser->parseStdoutLine(runControl, line);
        break;
    case Utils::OutputFormat::StdOutFormat:
        return parser->parseStdoutLine(runControl, line);
    case Utils::OutputFormat::StdErrFormat:
        return parser->parseStderrLine(runControl, line);
    case Utils::OutputFormat::StdOutFormatSameLine:
        return parser->parseStdoutLine(runControl, line);
    case Utils::OutputFormat::StdErrFormatSameLine:
        return parser->parseStderrLine(runControl, line);
    default:
        break;
    }

    return Unsure;
}

} // namespace Internal
} // namespace QTestLibPlugin
