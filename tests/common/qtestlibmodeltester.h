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

#ifndef MODELTESTER_H
#define MODELTESTER_H

#include <QObject>
#include "../../testmodelfactory.h"
#include "qttestsubfunction.h"

class QAbstractItemModel;
class QDomDocument;
class QDomElement;

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
    inline QTestLibModelTester(const QAbstractItemModel *model, Verbosity verbosity = Normal, const QString& format = "txt"):
        mModel(model), mError(QString::null), mParserFormat(format), mVerbosity(verbosity) {}

    inline void setVerbosity(Verbosity verbosity) {mVerbosity = verbosity;}
    inline Verbosity verbosity(void) const {return mVerbosity;}
    inline void setFormat(const QString& format) {mParserFormat = format;}
    inline QString format(void) const {return mParserFormat;}

    bool checkResults(const QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult>& results, const QString& testName);
    bool checkIndex(const QModelIndex& index, const QString& testName);
    inline QString error(void) const {return mError;}
private:
    void loadTestResult(QDomDocument& dom, const QString& testName);
    void checkResultsInternal(const QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult>& results, const QString& testName);
    void checkIndexInternal(const QModelIndex& model, const QString& testName);

    void isOutput(const QDomElement& element, bool *ret);
    void isOutputFormat(const QDomElement& element, bool *ret);
    void isOutputVerbosity(const QDomElement& element, bool *ret);

    void checkResults(QLinkedList<QTestLibPlugin::Internal::TestModelFactory::ParseResult> results, const QDomElement& expected);

    void parseRoot(const QModelIndex& index, const QDomElement& element);
    void parseClass(const QModelIndex& index, const QDomElement& element);
    void parseFunction(const QModelIndex& index, const QDomElement& element);
    void parseRow(const QModelIndex& index, const QDomElement& element);
    void parseMessage(const QModelIndex& index, const QDomElement& element);
    void checkLocation(const QModelIndex& index, const QDomElement& element);

    const QAbstractItemModel *mModel;
    QString mError;
    QString mParserFormat;
    Verbosity mVerbosity;
};


Q_DECLARE_METATYPE(QTestLibModelTester::Verbosity)

#endif // MODELTESTER_H
