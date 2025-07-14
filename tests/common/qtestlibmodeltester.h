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

#ifndef MODELTESTER_H
#define MODELTESTER_H

#include <QObject>
#include <qtestlibmodel.h>
#include <testmodelfactory.h>
#include "qttestsubfunction.h"

class QAbstractItemModel;
class QDomDocument;
class QDomElement;

class QtVersion;

class QTestLibModelTester : public QObject
{
    Q_OBJECT
    HAS_SUB_TEST_FUNCTIONS
public:
    // WARNING Order is important!
    typedef enum {
        Silent = -1,
        Normal,
        Verbose1,
        Verbose2,
        VerboseSignal,
        VerbosityCountMinusOne // WARNING must be the last one!
    } Verbosity;
    QTestLibModelTester(const QAbstractItemModel *model, Verbosity verbosity = Normal, const QString& format = QLatin1String("txt"));

    inline void setVerbosity(Verbosity verbosity) {mVerbosity = verbosity;}
    inline Verbosity verbosity(void) const {return mVerbosity;}
    inline void setFormat(const QString& format) {mParserFormat = format;}
    inline QString format(void) const {return mParserFormat;}

    inline void setResultsFile(const QString& path) {mResultFilePath = path;}
    inline QString resultFile(void) const {return mResultFilePath;}

    inline void enableMessageType(QTestLibPlugin::Internal::QTestLibModel::MessageType type) {mFilters[(int) type] = true;}
    inline void disableMessageType(QTestLibPlugin::Internal::QTestLibModel::MessageType type) {mFilters[(int) type] = false;}
    inline void toogleMessageType(QTestLibPlugin::Internal::QTestLibModel::MessageType type) {mFilters[(int) type] = !mFilters.at((int) type);}
    inline bool isMessageTypeEnabled(QTestLibPlugin::Internal::QTestLibModel::MessageType type) const {return mFilters.at((int) type);}

    bool checkResults(const QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult>& results);
    bool checkIndex(const QModelIndex& index);
    inline QString error(void) const {return mError;}
private:
    void loadTestResult(QDomDocument& dom);
    void checkResultsInternal(const QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult>& results);
    void checkIndexInternal(const QModelIndex& model);

    void isOutput(const QDomElement& element, bool *ret, bool filter = true);
    void isOutputFormat(const QDomElement& element, bool *ret);
    void isOutputVerbosity(const QDomElement& element, bool *ret);
    void isOutputQt(const QDomElement& element, bool *ret);
    bool isOutputMinQt(const int qt);
    bool isOutputMaxQt(const int qt);
    void isOutputType(const QDomElement& element, bool* ret);

    void parseResults(const QDomElement& element, QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult>& results);
    void parseRoot(const QModelIndex& index, const QDomElement& element);
    void parseClass(const QModelIndex& index, const QDomElement& element);
    void parseFunction(const QModelIndex& index, const QDomElement& element);
    void parseRow(const QModelIndex& index, const QDomElement& element);
    void parseMessage(const QModelIndex& index, const QDomElement& element);
    void checkLocation(const QModelIndex& index, const QDomElement& element);

    unsigned int qtVersionStrToInt(const QString& qtVersionStr);

    const QAbstractItemModel *mModel;
    QString mResultFilePath;
    QString mError;
    QString mParserFormat;
    Verbosity mVerbosity;
    QVector<bool> mFilters;
};

QDebug operator<<(QDebug dbg, const QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult>& results);

Q_DECLARE_METATYPE(QTestLibModelTester::Verbosity)

#endif // MODELTESTER_H
