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

#include <qtestlibargsparser.h>
using namespace QTestLibPlugin::Internal;
#include <qtestlibpluginconstants.h>

#include "../common/qttestsubfunction.h"

#include <QtTest>

class QTestLibArgsParserTest : public QObject
{
    Q_OBJECT
    HAS_SUB_TEST_FUNCTIONS
public:
    inline QTestLibArgsParserTest(void) {qsrand(QDateTime::currentMSecsSinceEpoch());}
private Q_SLOTS:
    void verbosity_data(void);
    void verbosity(void);
    void outputFormatV1_data(void);
    void outputFormatV1(void);
    void outputFileNameV1_data(void);
    void outputFileNameV1(void);
    void outputV1_data(void);
    void outputV1(void);
    void outputV2_data(void);
    void outputV2(void);
    void outputV1vsV2_data(void);
    void outputV1vsV2(void);
    void outputVerbosityV1_data(void);
    void outputVerbosityV1(void);
    void outputVerbosityV2_data(void);
    void outputVerbosityV2(void);
    void outputV2Error_data(void);
    void outputV2Error(void);

    void outputMode_data(void);
    void outputMode(void);
    void maxWarnings_data(void);
    void maxWarnings(void);
    void delays_data(void);
    void delays(void);
    void crashHandler(void);

    void parse_data(void);
    void parse(void);
    inline void copyParse_data(void) {parse_data();}
    void copyParse(void);
    void incrementalParse_data(void);
    void incrementalParse(void);

    void testCases_data(void);
    void testCases(void);
    void removeTestCases_data(void);
    void removeTestCases(void);

    void flagError_data(void);
    void flagError(void);
    void invalidArgumentError_data(void);
    void invalidArgumentError(void);
    void invalidTestError_data(void);
    void invalidTestError(void);
    void prematureEndError_data(void);
    void prematureEndError(void);

    void toStringOutput_data(void);
    void toStringOutput(void);
    void toStringFormatV1_data(void);
    void toStringFormatV1(void);
    void toStringFormatV2_data(void);
    void toStringFormatV2(void);
    void toStringFormat_data(void);
    void toStringFormat(void);
    void toStringOutFileV1_data(void);
    void toStringOutFileV1(void);
    void toStringOutFileV2_data(void);
    void toStringOutFileV2(void);
    void toStringOutFile_data(void);
    void toStringOutFile(void);
    void toStringVerbosity_data(void);
    void toStringVerbosity(void);
    void toStringOther_data(void);
    void toStringOther(void);
    void toStringMixedV1_data(void);
    void toStringMixedV1(void);
    inline void copyToStringMixedV1_data(void) {toStringMixedV1_data();}
    void copyToStringMixedV1(void);
    void toStringMixedV2_data(void);
    void toStringMixedV2(void);
    inline void copyToStringMixedV2_data(void) {toStringMixedV2_data();}
    void copyToStringMixedV2(void);
    void toStringMixed_data(void);
    void toStringMixed(void);
    inline void copyToStringMixed_data(void) {toStringMixed_data();}
    void copyToStringMixed(void);

    void mapFormat_data(void);
    void mapFormat(void);
    void mapVerbosity_data(void);
    void mapVerbosity(void);
    void mapOutFile(void);
    void mapOther_data(void);
    void mapOther(void);
    void mapMixed_data(void);
    void mapMixed(void);
    inline void copyMapMixed_data(void) {mapMixed_data();}
    void copyMapMixed(void);
private:
    void toStringMixed_row(QTestLibArgsParser::TestOutputFormat format, QTestLibArgsParser::TestVerbosity verbosity, int version = 2);
    void mapMixed_row(QTestLibArgsParser::TestOutputFormat format, QTestLibArgsParser::TestVerbosity verbosity);

    void checkError(const QTestLibArgsParser& parser, QTestLibArgsParser::Error error = QTestLibArgsParser::NoError, const QString& errorString = QString::null);
    void checkOutput(const QTestLibArgsParser& parser, QTestLibArgsParser::TestVerbosity verb, QTestLibArgsParser::TestOutputFormat format, const QString& filename = QString::null);
    void checkDelays(const QTestLibArgsParser& parser, int event = -1, int key = -1, int mouse = -1);
    void checkOutputMode(const QTestLibArgsParser& parser, QTestLibArgsParser::Output mode = QTestLibArgsParser::NormalOutput);
    void checkSelectedTestCases(const QTestLibArgsParser& parser, const QTestLibArgsParser::TestCaseList& list);
    void checkArguments(const QStringList& args, const QStringList& expected);

    QTestLibArgsParser mParser;
};

Q_DECLARE_METATYPE(QTestLibArgsParser::Output)
Q_DECLARE_METATYPE(QTestLibArgsParser::Error)

void QTestLibArgsParserTest::verbosity_data(void)
{
    QTest::addColumn<QString>("args");
    QTest::addColumn<QTestLibArgsParser::TestVerbosity>("verb");

    QTest::newRow("Silent") << "-silent" << QTestLibArgsParser::Silent;
    QTest::newRow("Normal") << "" << QTestLibArgsParser::NormalVerbosity;
    QTest::newRow("Verbose1") << "-v1" << QTestLibArgsParser::Verbose1;
    QTest::newRow("Verbose2") << "-v2" << QTestLibArgsParser::Verbose2;
    QTest::newRow("VerboseSignal") << "-vs" << QTestLibArgsParser::VerboseSignal;
    QTest::newRow("VerboseBenchmark") << "-vb" << QTestLibArgsParser::VerboseBenchmark;
}

void QTestLibArgsParserTest::verbosity(void)
{
    QFETCH(QString, args);
    QFETCH(QTestLibArgsParser::TestVerbosity, verb);

    QTestLibArgsParser parser(args);

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkOutput(parser, verb, QTestLibArgsParser::TxtFormat));
    SUB_TEST_FUNCTION(checkDelays(parser));
    SUB_TEST_FUNCTION(checkOutputMode(parser));

    QVERIFY(parser.crashHandlerEnabled());
    QVERIFY(parser.maxWarnings() == 2000);
}

void QTestLibArgsParserTest::outputFormatV1_data(void)
{
    QTest::addColumn<QString>("args");
    QTest::addColumn<QTestLibArgsParser::TestOutputFormat>("format");

    QTest::newRow("Default") << "" << QTestLibArgsParser::TxtFormat;
    QTest::newRow("txt") << "-txt" << QTestLibArgsParser::TxtFormat;
    QTest::newRow("csv") << "-csv" << QTestLibArgsParser::CsvFormat;
    QTest::newRow("xunitxml") << "-xunitxml" << QTestLibArgsParser::XUnitXmlFormat;
    QTest::newRow("xml") << "-xml" << QTestLibArgsParser::XmlFormat;
    QTest::newRow("lightxml") << "-lightxml" << QTestLibArgsParser::LightXmlFormat;
}

void QTestLibArgsParserTest::outputFormatV1(void)
{
    QFETCH(QString, args);
    QFETCH(QTestLibArgsParser::TestOutputFormat, format);

    QTestLibArgsParser parser(args);

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkOutput(parser, QTestLibArgsParser::NormalVerbosity, format));
    SUB_TEST_FUNCTION(checkDelays(parser));
    SUB_TEST_FUNCTION(checkOutputMode(parser));

    QVERIFY(parser.crashHandlerEnabled());
    QVERIFY(parser.maxWarnings() == 2000);
}

void QTestLibArgsParserTest::outputFileNameV1_data(void)
{
    QTest::addColumn<QString>("args");
    QTest::addColumn<QString>("filename");

    QTest::newRow("Default") << "" << QString();
    QTest::newRow("-") << "-o -" << QString();
    QTest::newRow("\"-\"") << "-o \"-\"" << QString();

    QTest::newRow("testResult") << "-o testResult" << "testResult";
    QTest::newRow("testResult.log") << "-o testResult.log" << "testResult.log";
    QTest::newRow("test\\ result.log") << "-o test\\ result.log" << "test result.log";

    QTest::newRow("\"testResult\"") << "-o \"testResult\"" <<  "testResult";
    QTest::newRow("\"testResult.log\"") << "-o \"testResult.log\"" <<  "testResult.log";
    QTest::newRow("\"test result.log\"") << "-o \"test result.log\"" <<  "test result.log";
    QTest::newRow("\"test\\\" \\\"result.log\"") << "-o \"test\\\" \\\"result.log\"" <<  "test\" \"result.log";
}

void QTestLibArgsParserTest::outputFileNameV1(void)
{
    QFETCH(QString, args);
    QFETCH(QString, filename);

    QTestLibArgsParser parser(args);

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkOutput(parser, QTestLibArgsParser::NormalVerbosity, QTestLibArgsParser::TxtFormat, filename));
    SUB_TEST_FUNCTION(checkDelays(parser));
    SUB_TEST_FUNCTION(checkOutputMode(parser));

    QVERIFY(parser.crashHandlerEnabled());
    QVERIFY(parser.maxWarnings() == 2000);
}

void QTestLibArgsParserTest::outputV1_data(void)
{
    QTest::addColumn<QString>("args");
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QTestLibArgsParser::TestOutputFormat>("format");

    QTest::newRow("-o - -xunitxml") << "-o - -xunitxml" << QString() << QTestLibArgsParser::XUnitXmlFormat;
    QTest::newRow("-o \"-\",xunitxml") << "-o \"-\" -xunitxml" << QString() << QTestLibArgsParser::XUnitXmlFormat;
    QTest::newRow("-o testResult -xunitxml") << "-o testResult -xunitxml" << "testResult" << QTestLibArgsParser::XUnitXmlFormat;
    QTest::newRow("-o testResult.log -xunitxml") << "-o testResult.log -xunitxml" << "testResult.log" << QTestLibArgsParser::XUnitXmlFormat;
    QTest::newRow("-o test\\ result.log -xunitxml") << "-o test\\ result.log -xunitxml" << "test result.log" << QTestLibArgsParser::XUnitXmlFormat;
    QTest::newRow("-o \"testResult\" -xunitxml") << "-o \"testResult\" -xunitxml" <<  "testResult" << QTestLibArgsParser::XUnitXmlFormat;
    QTest::newRow("-o \"testResult.log\" -xunitxml") << "-o \"testResult.log\" -xunitxml" <<  "testResult.log" << QTestLibArgsParser::XUnitXmlFormat;
    QTest::newRow("-o \"test result.log\" -xunitxml") << "-o \"test result.log\" -xunitxml" <<  "test result.log" << QTestLibArgsParser::XUnitXmlFormat;
    QTest::newRow("-o \"test\\\" \\\"result.log\" -xunitxml") << "-o \"test\\\" \\\"result.log\" -xunitxml" <<  "test\" \"result.log" << QTestLibArgsParser::XUnitXmlFormat;

    QTest::newRow("-xml -o -") << "-xml -o -" << QString() << QTestLibArgsParser::XmlFormat;
    QTest::newRow("-xml -o \"-\"") << "-xml -o \"-\"" << QString() << QTestLibArgsParser::XmlFormat;
    QTest::newRow("-xml -o testResult") << "-xml -o testResult" << "testResult" << QTestLibArgsParser::XmlFormat;
    QTest::newRow("-xml -o testResult.log") << "-xml -o testResult.log" << "testResult.log" << QTestLibArgsParser::XmlFormat;
    QTest::newRow("-xml -o test\\ result.log") << "-xml -o test\\ result.log" << "test result.log" << QTestLibArgsParser::XmlFormat;
    QTest::newRow("-xml -o \"testResult\"") << "-xml -o \"testResult\"" <<  "testResult" << QTestLibArgsParser::XmlFormat;
    QTest::newRow("-xml -o \"testResult.log\"") << "-xml -o \"testResult.log\"" <<  "testResult.log" << QTestLibArgsParser::XmlFormat;
    QTest::newRow("-xml -o \"test result.log\"") << "-xml -o \"test result.log\"" <<  "test result.log" << QTestLibArgsParser::XmlFormat;
    QTest::newRow("-xml -o \"test\\\" \\\"result.log\"") << "-xml -o \"test\\\" \\\"result.log\"" <<  "test\" \"result.log" << QTestLibArgsParser::XmlFormat;

}

void QTestLibArgsParserTest::outputV1(void)
{
    QFETCH(QString, args);
    QFETCH(QString, filename);
    QFETCH(QTestLibArgsParser::TestOutputFormat, format);

    QTestLibArgsParser parser(args);

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkOutput(parser, QTestLibArgsParser::NormalVerbosity, format, filename));
    SUB_TEST_FUNCTION(checkDelays(parser));
    SUB_TEST_FUNCTION(checkOutputMode(parser));

    QVERIFY(parser.crashHandlerEnabled());
    QVERIFY(parser.maxWarnings() == 2000);
}

void QTestLibArgsParserTest::outputV2_data(void)
{
    QTest::addColumn<QString>("args");
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QTestLibArgsParser::TestOutputFormat>("format");

    QTest::newRow("Default") << "" << QString() << QTestLibArgsParser::TxtFormat;
    QTest::newRow("-,txt") << "-o -,txt" << QString() << QTestLibArgsParser::TxtFormat;
    QTest::newRow("\"-\",txt") << "-o \"-\",txt" << QString() << QTestLibArgsParser::TxtFormat;
    QTest::newRow("testResult,txt") << "-o testResult,txt" << "testResult" << QTestLibArgsParser::TxtFormat;
    QTest::newRow("testResult.log,txt") << "-o testResult.log,txt" << "testResult.log" << QTestLibArgsParser::TxtFormat;
    QTest::newRow("test\\ result.log,txt") << "-o test\\ result.log,txt" << "test result.log" << QTestLibArgsParser::TxtFormat;
    QTest::newRow("\"testResult\",txt") << "-o \"testResult\",txt" <<  "testResult" << QTestLibArgsParser::TxtFormat;
    QTest::newRow("\"testResult.log\",txt") << "-o \"testResult.log\",txt" <<  "testResult.log" << QTestLibArgsParser::TxtFormat;
    QTest::newRow("\"test result.log\",txt") << "-o \"test result.log\",txt" <<  "test result.log" << QTestLibArgsParser::TxtFormat;
    QTest::newRow("\"test\\\" \\\"result.log\",txt") << "-o \"test\\\" \\\"result.log\",txt" <<  "test\" \"result.log" << QTestLibArgsParser::TxtFormat;

    QTest::newRow("-,csv") << "-o -,csv" << QString() << QTestLibArgsParser::CsvFormat;
    QTest::newRow("\"-\",csv") << "-o \"-\",csv" << QString() << QTestLibArgsParser::CsvFormat;
    QTest::newRow("testResult,csv") << "-o testResult,csv" << "testResult" << QTestLibArgsParser::CsvFormat;
    QTest::newRow("testResult.log,csv") << "-o testResult.log,csv" << "testResult.log" << QTestLibArgsParser::CsvFormat;
    QTest::newRow("test\\ result.log,csv") << "-o test\\ result.log,csv" << "test result.log" << QTestLibArgsParser::CsvFormat;
    QTest::newRow("\"testResult\",csv") << "-o \"testResult\",csv" <<  "testResult" << QTestLibArgsParser::CsvFormat;
    QTest::newRow("\"testResult.log\",csv") << "-o \"testResult.log\",csv" <<  "testResult.log" << QTestLibArgsParser::CsvFormat;
    QTest::newRow("\"test result.log\",csv") << "-o \"test result.log\",csv" <<  "test result.log" << QTestLibArgsParser::CsvFormat;
    QTest::newRow("\"test\\\" \\\"result.log\",csv") << "-o \"test\\\" \\\"result.log\",csv" <<  "test\" \"result.log" << QTestLibArgsParser::CsvFormat;

    QTest::newRow("-,xunitxml") << "-o -,xunitxml" << QString() << QTestLibArgsParser::XUnitXmlFormat;
    QTest::newRow("\"-\",xunitxml") << "-o \"-\",xunitxml" << QString() << QTestLibArgsParser::XUnitXmlFormat;
    QTest::newRow("testResult,xunitxml") << "-o testResult,xunitxml" << "testResult" << QTestLibArgsParser::XUnitXmlFormat;
    QTest::newRow("testResult.log,xunitxml") << "-o testResult.log,xunitxml" << "testResult.log" << QTestLibArgsParser::XUnitXmlFormat;
    QTest::newRow("test\\ result.log,xunitxml") << "-o test\\ result.log,xunitxml" << "test result.log" << QTestLibArgsParser::XUnitXmlFormat;
    QTest::newRow("\"testResult\",xunitxml") << "-o \"testResult\",xunitxml" <<  "testResult" << QTestLibArgsParser::XUnitXmlFormat;
    QTest::newRow("\"testResult.log\",xunitxml") << "-o \"testResult.log\",xunitxml" <<  "testResult.log" << QTestLibArgsParser::XUnitXmlFormat;
    QTest::newRow("\"test result.log\",xunitxml") << "-o \"test result.log\",xunitxml" <<  "test result.log" << QTestLibArgsParser::XUnitXmlFormat;
    QTest::newRow("\"test\\\" \\\"result.log\",xunitxml") << "-o \"test\\\" \\\"result.log\",xunitxml" <<  "test\" \"result.log" << QTestLibArgsParser::XUnitXmlFormat;

    QTest::newRow("-,xml") << "-o -,xml" << QString() << QTestLibArgsParser::XmlFormat;
    QTest::newRow("\"-\",xml") << "-o \"-\",xml" << QString() << QTestLibArgsParser::XmlFormat;
    QTest::newRow("testResult,xml") << "-o testResult,xml" << "testResult" << QTestLibArgsParser::XmlFormat;
    QTest::newRow("testResult.log,xml") << "-o testResult.log,xml" << "testResult.log" << QTestLibArgsParser::XmlFormat;
    QTest::newRow("test\\ result.log,xml") << "-o test\\ result.log,xml" << "test result.log" << QTestLibArgsParser::XmlFormat;
    QTest::newRow("\"testResult\",xml") << "-o \"testResult\",xml" <<  "testResult" << QTestLibArgsParser::XmlFormat;
    QTest::newRow("\"testResult.log\",xml") << "-o \"testResult.log\",xml" <<  "testResult.log" << QTestLibArgsParser::XmlFormat;
    QTest::newRow("\"test result.log\",xml") << "-o \"test result.log\",xml" <<  "test result.log" << QTestLibArgsParser::XmlFormat;
    QTest::newRow("\"test\\\" \\\"result.log\",xml") << "-o \"test\\\" \\\"result.log\",xml" <<  "test\" \"result.log" << QTestLibArgsParser::XmlFormat;

    QTest::newRow("-,lightxml") << "-o -,lightxml" << QString() << QTestLibArgsParser::LightXmlFormat;
    QTest::newRow("\"-\",lightxml") << "-o \"-\",lightxml" << QString() << QTestLibArgsParser::LightXmlFormat;
    QTest::newRow("testResult,lightxml") << "-o testResult,lightxml" << "testResult" << QTestLibArgsParser::LightXmlFormat;
    QTest::newRow("testResult.log,lightxml") << "-o testResult.log,lightxml" << "testResult.log" << QTestLibArgsParser::LightXmlFormat;
    QTest::newRow("test\\ result.log,lightxml") << "-o test\\ result.log,lightxml" << "test result.log" << QTestLibArgsParser::LightXmlFormat;
    QTest::newRow("\"testResult\",lightxml") << "-o \"testResult\",lightxml" <<  "testResult" << QTestLibArgsParser::LightXmlFormat;
    QTest::newRow("\"testResult.log\",lightxml") << "-o \"testResult.log\",lightxml" <<  "testResult.log" << QTestLibArgsParser::LightXmlFormat;
    QTest::newRow("\"test result.log\",lightxml") << "-o \"test result.log\",lightxml" <<  "test result.log" << QTestLibArgsParser::LightXmlFormat;
    QTest::newRow("\"test\\\" \\\"result.log\",lightxml") << "-o \"test\\\" \\\"result.log\",lightxml" <<  "test\" \"result.log" << QTestLibArgsParser::LightXmlFormat;
}

void QTestLibArgsParserTest::outputV2(void)
{
    QFETCH(QString, args);
    QFETCH(QString, filename);
    QFETCH(QTestLibArgsParser::TestOutputFormat, format);

    QTestLibArgsParser parser(args);

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkOutput(parser, QTestLibArgsParser::NormalVerbosity, format, filename));
    SUB_TEST_FUNCTION(checkDelays(parser));
    SUB_TEST_FUNCTION(checkOutputMode(parser));

    QVERIFY(parser.crashHandlerEnabled());
    QVERIFY(parser.maxWarnings() == 2000);
}


void QTestLibArgsParserTest::outputV1vsV2_data(void)
{
    QTest::addColumn<QString>("args");
    QTest::addColumn<QTestLibArgsParser::TestOutputFormat>("format");

    QTest::newRow("-txt -xml") << "-txt -xml" << QTestLibArgsParser::XmlFormat;
    QTest::newRow("-xml -txt") << "-xml -txt" << QTestLibArgsParser::TxtFormat;

    QTest::newRow("-txt -o -,xml") << "-txt -o -,xml" << QTestLibArgsParser::XmlFormat;
    QTest::newRow("-xml -o -,txt") << "-xml -o -,txt" << QTestLibArgsParser::TxtFormat;
    QTest::newRow("-o -,txt -xml") << "-o -,txt -xml" << QTestLibArgsParser::TxtFormat;
    QTest::newRow("-o -,xml -txt") << "-o -,xml -txt" << QTestLibArgsParser::XmlFormat;
}

void QTestLibArgsParserTest::outputV1vsV2(void)
{
    QFETCH(QString, args);
    QFETCH(QTestLibArgsParser::TestOutputFormat, format);

    QTestLibArgsParser parser(args);

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkOutput(parser, QTestLibArgsParser::NormalVerbosity, format));
    SUB_TEST_FUNCTION(checkDelays(parser));
    SUB_TEST_FUNCTION(checkOutputMode(parser));

    QVERIFY(parser.crashHandlerEnabled());
    QVERIFY(parser.maxWarnings() == 2000);
}


void QTestLibArgsParserTest::outputVerbosityV1_data(void)
{
    QTest::addColumn<QString>("args");
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QTestLibArgsParser::TestOutputFormat>("format");
    QTest::addColumn<QTestLibArgsParser::TestVerbosity>("verb");

    QLinkedList< QPair<QString, QString> > fileNames;
    fileNames << QPair<QString, QString>("-o -", QString::null);
    fileNames << QPair<QString, QString>("-o \"-\"", QString::null);
    fileNames << QPair<QString, QString>("-o testResults.log", "testResults.log");
    fileNames << QPair<QString, QString>("-o \"testResults.log\"", "testResults.log");
    fileNames << QPair<QString, QString>("-o \"test results.log\"", "test results.log");
    fileNames << QPair<QString, QString>("-o test\\ results.log", "test results.log");

    QLinkedList < QPair<QString, QTestLibArgsParser::TestOutputFormat> > formats;
    formats << QPair<QString, QTestLibArgsParser::TestOutputFormat>("-txt", QTestLibArgsParser::TxtFormat);
    formats << QPair<QString, QTestLibArgsParser::TestOutputFormat>("-csv", QTestLibArgsParser::CsvFormat);
    formats << QPair<QString, QTestLibArgsParser::TestOutputFormat>("-xunitxml", QTestLibArgsParser::XUnitXmlFormat);
    formats << QPair<QString, QTestLibArgsParser::TestOutputFormat>("-xml", QTestLibArgsParser::XmlFormat);
    formats << QPair<QString, QTestLibArgsParser::TestOutputFormat>("-lightxml", QTestLibArgsParser::LightXmlFormat);

    QLinkedList < QPair<QString, QTestLibArgsParser::TestVerbosity> > verbosities;
    verbosities << QPair<QString, QTestLibArgsParser::TestVerbosity>("-silent", QTestLibArgsParser::Silent);
    verbosities << QPair<QString, QTestLibArgsParser::TestVerbosity>("", QTestLibArgsParser::NormalVerbosity);
    verbosities << QPair<QString, QTestLibArgsParser::TestVerbosity>("-v1", QTestLibArgsParser::Verbose1);
    verbosities << QPair<QString, QTestLibArgsParser::TestVerbosity>("-v2", QTestLibArgsParser::Verbose2);
    verbosities << QPair<QString, QTestLibArgsParser::TestVerbosity>("-vs", QTestLibArgsParser::VerboseSignal);
    verbosities << QPair<QString, QTestLibArgsParser::TestVerbosity>("-vb", QTestLibArgsParser::VerboseBenchmark);

    for (QLinkedList< QPair<QString, QTestLibArgsParser::TestVerbosity> >::const_iterator v = verbosities.constBegin(); v != verbosities.constEnd(); v++) {
        for (QLinkedList< QPair<QString, QTestLibArgsParser::TestOutputFormat> >::const_iterator f = formats.constBegin(); f != formats.constEnd(); f++) {
            for (QLinkedList< QPair<QString, QString> >::const_iterator n = fileNames.constBegin(); n != fileNames.constEnd(); n++) {
                QTest::newRow(qPrintable((*v).first + " " + (*f).first + " " + (*n).first)) << QString("%1 %2 %3").arg((*v).first).arg((*f).first).arg((*n).first) << (*n).second << (*f).second << (*v).second;
                QTest::newRow(qPrintable((*f).first + " " + (*v).first + " " + (*n).first)) << QString("%1 %2 %3").arg((*f).first).arg((*v).first).arg((*n).first) << (*n).second << (*f).second << (*v).second;
                QTest::newRow(qPrintable((*v).first + " " + (*n).first + " " + (*f).first)) << QString("%1 %2 %3").arg((*v).first).arg((*n).first).arg((*f).first) << (*n).second << (*f).second << (*v).second;
                QTest::newRow(qPrintable((*f).first + " " + (*n).first + " " + (*v).first)) << QString("%1 %2 %3").arg((*f).first).arg((*n).first).arg((*v).first) << (*n).second << (*f).second << (*v).second;
                QTest::newRow(qPrintable((*n).first + " " + (*v).first + " " + (*f).first)) << QString("%1 %2 %3").arg((*n).first).arg((*v).first).arg((*f).first) << (*n).second << (*f).second << (*v).second;
                QTest::newRow(qPrintable((*n).first + " " + (*f).first + " " + (*v).first)) << QString("%1 %2 %3").arg((*n).first).arg((*f).first).arg((*v).first) << (*n).second << (*f).second << (*v).second;
            }
        }
    }
}

void QTestLibArgsParserTest::outputVerbosityV1(void)
{
    QFETCH(QString, args);
    QFETCH(QString, filename);
    QFETCH(QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(QTestLibArgsParser::TestVerbosity, verb);

    QTestLibArgsParser parser(args);

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkOutput(parser, verb, format, filename));
    SUB_TEST_FUNCTION(checkDelays(parser));
    SUB_TEST_FUNCTION(checkOutputMode(parser));

    QVERIFY(parser.crashHandlerEnabled());
    QVERIFY(parser.maxWarnings() == 2000);
}

void QTestLibArgsParserTest::outputVerbosityV2_data(void)
{
    QTest::addColumn<QString>("args");
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QTestLibArgsParser::TestOutputFormat>("format");
    QTest::addColumn<QTestLibArgsParser::TestVerbosity>("verb");

    QLinkedList< QPair<QString, QString> > fileNames;
    fileNames << QPair<QString, QString>("-o -", QString::null);
    fileNames << QPair<QString, QString>("-o \"-\"", QString::null);
    fileNames << QPair<QString, QString>("-o testResults.log", "testResults.log");
    fileNames << QPair<QString, QString>("-o \"testResults.log\"", "testResults.log");
    fileNames << QPair<QString, QString>("-o \"test results.log\"", "test results.log");
    fileNames << QPair<QString, QString>("-o test\\ results.log", "test results.log");

    QLinkedList < QPair<QString, QTestLibArgsParser::TestOutputFormat> > formats;
    formats << QPair<QString, QTestLibArgsParser::TestOutputFormat>("txt", QTestLibArgsParser::TxtFormat);
    formats << QPair<QString, QTestLibArgsParser::TestOutputFormat>("csv", QTestLibArgsParser::CsvFormat);
    formats << QPair<QString, QTestLibArgsParser::TestOutputFormat>("xunitxml", QTestLibArgsParser::XUnitXmlFormat);
    formats << QPair<QString, QTestLibArgsParser::TestOutputFormat>("xml", QTestLibArgsParser::XmlFormat);
    formats << QPair<QString, QTestLibArgsParser::TestOutputFormat>("lightxml", QTestLibArgsParser::LightXmlFormat);

    QLinkedList < QPair<QString, QTestLibArgsParser::TestVerbosity> > verbosities;
    verbosities << QPair<QString, QTestLibArgsParser::TestVerbosity>("-silent", QTestLibArgsParser::Silent);
    verbosities << QPair<QString, QTestLibArgsParser::TestVerbosity>("", QTestLibArgsParser::NormalVerbosity);
    verbosities << QPair<QString, QTestLibArgsParser::TestVerbosity>("-v1", QTestLibArgsParser::Verbose1);
    verbosities << QPair<QString, QTestLibArgsParser::TestVerbosity>("-v2", QTestLibArgsParser::Verbose2);
    verbosities << QPair<QString, QTestLibArgsParser::TestVerbosity>("-vs", QTestLibArgsParser::VerboseSignal);
    verbosities << QPair<QString, QTestLibArgsParser::TestVerbosity>("-vb", QTestLibArgsParser::VerboseBenchmark);

    for (QLinkedList< QPair<QString, QTestLibArgsParser::TestVerbosity> >::const_iterator v = verbosities.constBegin(); v != verbosities.constEnd(); v++) {
        for (QLinkedList< QPair<QString, QTestLibArgsParser::TestOutputFormat> >::const_iterator f = formats.constBegin(); f != formats.constEnd(); f++) {
            for (QLinkedList< QPair<QString, QString> >::const_iterator n = fileNames.constBegin(); n != fileNames.constEnd(); n++) {
                QTest::newRow(qPrintable((*v).first + " " + (*n).first + "," + (*f).first)) << QString("%1 %2,%3").arg((*v).first).arg((*n).first).arg((*f).first) << (*n).second << (*f).second << (*v).second;
                QTest::newRow(qPrintable((*n).first + "," + (*f).first + " " + (*v).first)) << QString("%1,%2 %3").arg((*n).first).arg((*f).first).arg((*v).first) << (*n).second << (*f).second << (*v).second;
            }
        }
    }
}

void QTestLibArgsParserTest::outputVerbosityV2(void)
{
    QFETCH(QString, args);
    QFETCH(QString, filename);
    QFETCH(QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(QTestLibArgsParser::TestVerbosity, verb);

    QTestLibArgsParser parser(args);

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkOutput(parser, verb, format, filename));
    SUB_TEST_FUNCTION(checkDelays(parser));
    SUB_TEST_FUNCTION(checkOutputMode(parser));

    QVERIFY(parser.crashHandlerEnabled());
    QVERIFY(parser.maxWarnings() == 2000);
}

void QTestLibArgsParserTest::outputV2Error_data(void)
{
    QTest::addColumn<QString>("args");
    QTest::addColumn<QTestLibArgsParser::Error>("err");
    QTest::addColumn<QString>("errStr");

    QTest::newRow("-o -,xxx") << "-o -,xxx" << QTestLibArgsParser::InvalidArgumentError << "Got \"xxx\" where output format was expected.";
    QTest::newRow("-o -,\"txt\"") << "-o -,\"txt\"" << QTestLibArgsParser::InvalidArgumentError << "Got \"\"txt\"\" where output format was expected.";
    QTest::newRow("-o -,,txt") << "-o -,,txt" << QTestLibArgsParser::InvalidArgumentError << "Got \",txt\" where output format was expected.";

    QTest::newRow("-o testResult.log,xxx") << "-o testResult.log,xxx" << QTestLibArgsParser::InvalidArgumentError << "Got \"xxx\" where output format was expected.";
    QTest::newRow("-o testResult.log,\"txt\"") << "-o testResult.log,\"txt\"" << QTestLibArgsParser::InvalidArgumentError << "Got \"\"txt\"\" where output format was expected.";
    QTest::newRow("-o testResult.log,,txt") << "-o testResult.log,,txt" << QTestLibArgsParser::InvalidArgumentError << "Got \",txt\" where output format was expected.";

    QTest::newRow("-o test\\ result.log,xxx") << "-o test\\ result.log,xxx" << QTestLibArgsParser::InvalidArgumentError << "Got \"xxx\" where output format was expected.";
    QTest::newRow("-o test\\ result.log,\"txt\"") << "-o test\\ result.log,\"txt\"" << QTestLibArgsParser::InvalidArgumentError << "Got \"\"txt\"\" where output format was expected.";
    QTest::newRow("-o test\\ result.log,,txt") << "-o test\\ result.log,,txt" << QTestLibArgsParser::InvalidArgumentError << "Got \",txt\" where output format was expected.";

    QTest::newRow("-o \"test result.log\",xxx") << "-o \"test result.log\",xxx" << QTestLibArgsParser::InvalidArgumentError << "Got \"xxx\" where output format was expected.";
    QTest::newRow("-o \"test result.log\",\"txt\"") << "-o \"test result.log\",\"txt\"" << QTestLibArgsParser::InvalidArgumentError << "Got \"\"txt\"\" where output format was expected.";
    QTest::newRow("-o \"test result.log\",,txt") << "-o \"test result.log\",,txt" << QTestLibArgsParser::InvalidArgumentError << "Got \",txt\" where output format was expected.";

    QTest::newRow("-o \"test,result.log\",xxx") << "-o \"test,result.log\",xxx" << QTestLibArgsParser::InvalidArgumentError << "Got \"xxx\" where output format was expected.";
    QTest::newRow("-o \"test,result.log\",\"txt\"") << "-o \"test,result.log\",\"txt\"" << QTestLibArgsParser::InvalidArgumentError << "Got \"\"txt\"\" where output format was expected.";
    QTest::newRow("-o \"test,result.log\",,txt") << "-o \"test,result.log\",,txt" << QTestLibArgsParser::InvalidArgumentError << "Got \",txt\" where output format was expected.";
}

void QTestLibArgsParserTest::outputV2Error(void)
{
    QFETCH(QString, args);
    QFETCH(QTestLibArgsParser::Error, err);
    QFETCH(QString, errStr);

    QTestLibArgsParser parser(args);

    QVERIFY(parser.error() == err);
    QVERIFY(QString::compare(parser.errorString(), errStr, Qt::CaseSensitive) == 0);
}

void QTestLibArgsParserTest::outputMode_data(void)
{
    QTest::addColumn<QString>("args");
    QTest::addColumn<QTestLibArgsParser::Output>("mode");

    QTest::newRow("Normal") << "" << QTestLibArgsParser::NormalOutput;
    QTest::newRow("Help") << "-help" << QTestLibArgsParser::HelpOutput;
    QTest::newRow("Functions") << "-functions" << QTestLibArgsParser::FunctionsOutput;
    QTest::newRow("Datatags") << "-datatags" << QTestLibArgsParser::DataTagsOutput;
    QTest::newRow("PerfCounters") << "-perfcounterlist" << QTestLibArgsParser::PerfCounterNamesOutput;
}

void QTestLibArgsParserTest::outputMode(void)
{
    QFETCH(QString, args);
    QFETCH(QTestLibArgsParser::Output, mode);

    QTestLibArgsParser parser(args);

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkOutput(parser, QTestLibArgsParser::NormalVerbosity, mode == QTestLibArgsParser::NormalOutput ? QTestLibArgsParser::TxtFormat : QTestLibArgsParser::NoneFormat));
    SUB_TEST_FUNCTION(checkDelays(parser));
    SUB_TEST_FUNCTION(checkOutputMode(parser, mode));

    QVERIFY(parser.crashHandlerEnabled());
    QVERIFY(parser.maxWarnings() == 2000);
}

void QTestLibArgsParserTest::maxWarnings_data(void)
{
    QTest::addColumn<QString>("args");
    QTest::addColumn<unsigned int>("maxWarn");

    QTest::newRow("Default") << "" << 2000u;
    QTest::newRow("0") << "-maxwarnings 0" << 0u;
    QTest::newRow("1") << "-maxwarnings 1" << 1u;
    QTest::newRow("2") << "-maxwarnings 2" << 2u;
    QTest::newRow("3") << "-maxwarnings 3" << 3u;
    QTest::newRow("4") << "-maxwarnings 4" << 4u;
    QTest::newRow("5") << "-maxwarnings 5" << 5u;
    QTest::newRow("6") << "-maxwarnings 6" << 6u;
    QTest::newRow("7") << "-maxwarnings 7" << 7u;
    QTest::newRow("8") << "-maxwarnings 8" << 8u;
    QTest::newRow("9") << "-maxwarnings 9" << 9u;
    QTest::newRow("10") << "-maxwarnings 10" << 10u;
    QTest::newRow("100") << "-maxwarnings 100" << 100u;
    QTest::newRow("1000") << "-maxwarnings 1000" << 1000u;
    QTest::newRow("10000") << "-maxwarnings 10000" << 10000u;
}

void QTestLibArgsParserTest::maxWarnings(void)
{
    QFETCH(QString, args);
    QFETCH(unsigned int, maxWarn);

    QTestLibArgsParser parser(args);

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkOutput(parser, QTestLibArgsParser::NormalVerbosity, QTestLibArgsParser::TxtFormat));
    SUB_TEST_FUNCTION(checkDelays(parser));
    SUB_TEST_FUNCTION(checkOutputMode(parser));

    QVERIFY(parser.crashHandlerEnabled());
    QVERIFY(parser.maxWarnings() == maxWarn);
}

void QTestLibArgsParserTest::delays_data(void)
{
    QTest::addColumn<QString>("args");
    QTest::addColumn<int>("event");
    QTest::addColumn<int>("key");
    QTest::addColumn<int>("mouse");

    QTest::newRow("Default") << "" << -1  << -1 << -1;
    QTest::newRow("0") << "-eventdelay 0" << 0 << -1 << -1;
    QTest::newRow("1") << "-eventdelay 1" << 1 << -1 << -1;
    QTest::newRow("2") << "-eventdelay 2" << 2 << -1 << -1;
    QTest::newRow("3") << "-eventdelay 3" << 3 << -1 << -1;
    QTest::newRow("4") << "-eventdelay 4" << 4 << -1 << -1;
    QTest::newRow("5") << "-eventdelay 5" << 5 << -1 << -1;
    QTest::newRow("6") << "-eventdelay 6" << 6 << -1 << -1;
    QTest::newRow("7") << "-eventdelay 7" << 7 << -1 << -1;
    QTest::newRow("8") << "-eventdelay 8" << 8 << -1 << -1;
    QTest::newRow("9") << "-eventdelay 9" << 9 << -1 << -1;
    QTest::newRow("10") << "-eventdelay 10" << 10 << -1 << -1;
    QTest::newRow("100") << "-eventdelay 100" << 100 << -1 << -1;
    QTest::newRow("1000") << "-eventdelay 1000" << 1000 << -1 << -1;
    QTest::newRow("10000") << "-eventdelay 10000" << 10000 << -1 << -1;

    QTest::newRow("0") << "-keydelay 0" << - 1 << 0 << -1;
    QTest::newRow("1") << "-keydelay 1" << - 1 << 1 << -1;
    QTest::newRow("2") << "-keydelay 2" << - 1 << 2 << -1;
    QTest::newRow("3") << "-keydelay 3" << - 1 << 3 << -1;
    QTest::newRow("4") << "-keydelay 4" << - 1 << 4 << -1;
    QTest::newRow("5") << "-keydelay 5" << - 1 << 5 << -1;
    QTest::newRow("6") << "-keydelay 6" << - 1 << 6 << -1;
    QTest::newRow("7") << "-keydelay 7" << - 1 << 7 << -1;
    QTest::newRow("8") << "-keydelay 8" << - 1 << 8 << -1;
    QTest::newRow("9") << "-keydelay 9" << - 1 << 9 << -1;
    QTest::newRow("10") << "-keydelay 10" << - 1 << 10 << -1;
    QTest::newRow("100") << "-keydelay 100" << - 1 << 100 << -1;
    QTest::newRow("1000") << "-keydelay 1000" << - 1 << 1000 << -1;
    QTest::newRow("10000") << "-keydelay 10000" << - 1 << 10000 << -1;

    QTest::newRow("0") << "-mousedelay 0" << -1 << -1 << 0;
    QTest::newRow("1") << "-mousedelay 1" << -1 << -1 << 1;
    QTest::newRow("2") << "-mousedelay 2" << -1 << -1 << 2;
    QTest::newRow("3") << "-mousedelay 3" << -1 << -1 << 3;
    QTest::newRow("4") << "-mousedelay 4" << -1 << -1 << 4;
    QTest::newRow("5") << "-mousedelay 5" << -1 << -1 << 5;
    QTest::newRow("6") << "-mousedelay 6" << -1 << -1 << 6;
    QTest::newRow("7") << "-mousedelay 7" << -1 << -1 << 7;
    QTest::newRow("8") << "-mousedelay 8" << -1 << -1 << 8;
    QTest::newRow("9") << "-mousedelay 9" << -1 << -1 << 9;
    QTest::newRow("10") << "-mousedelay 10" << -1 << -1 << 10;
    QTest::newRow("100") << "-mousedelay 100" << -1 << -1 << 100;
    QTest::newRow("1000") << "-mousedelay 1000" << -1 << -1 << 1000;
    QTest::newRow("10000") << "-mousedelay 10000" << -1 << -1 << 10000;

    QTest::newRow("18 36 .") << "-eventdelay 18 -keydelay 36" << 18 << 36 << -1;
    QTest::newRow("18 . 36") << "-eventdelay 18 -mousedelay 36" << 18 << -1 << 36;
    QTest::newRow(". 18 36") << "-keydelay 18 -mousedelay 36" << -1 << 18 << 36;
    QTest::newRow("18 36 54") << "-eventdelay 18 -keydelay 36 -mousedelay 54" << 18 << 36 << 54;
}

void QTestLibArgsParserTest::delays(void)
{
    QFETCH(QString, args);
    QFETCH(int, event);
    QFETCH(int, key);
    QFETCH(int, mouse);

    QTestLibArgsParser parser(args);

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkOutput(parser, QTestLibArgsParser::NormalVerbosity, QTestLibArgsParser::TxtFormat));
    SUB_TEST_FUNCTION(checkDelays(parser, event, key, mouse));
    SUB_TEST_FUNCTION(checkOutputMode(parser));

    QVERIFY(parser.crashHandlerEnabled());
    QVERIFY(parser.maxWarnings() == 2000);
}

void QTestLibArgsParserTest::crashHandler(void)
{
    QTestLibArgsParser parser("-nocrashhandler");

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkOutput(parser, QTestLibArgsParser::NormalVerbosity, QTestLibArgsParser::TxtFormat));
    SUB_TEST_FUNCTION(checkDelays(parser));
    SUB_TEST_FUNCTION(checkOutputMode(parser));

    QVERIFY(!parser.crashHandlerEnabled());
    QVERIFY(parser.maxWarnings() == 2000);
}

void QTestLibArgsParserTest::parse_data(void)
{
    QStringList args;
    int argsVersion;

    QTest::addColumn<QString>("args");
    QTest::addColumn<QTestLibArgsParser::TestOutputFormat>("format");
    QTest::addColumn<QTestLibArgsParser::TestVerbosity>("verbosity");
    QTest::addColumn<unsigned int>("maxWarings");
    QTest::addColumn<int>("eventDelay");
    QTest::addColumn<int>("keyDelay");
    QTest::addColumn<int>("mouseDelay");
    QTest::addColumn<bool>("crashHandler");

    for(int v = (int) QTestLibArgsParser::Silent; v <= (int) QTestLibArgsParser::VerboseSignal; v++) {
        for (int f = (int) QTestLibArgsParser::TxtFormat; f <= (int) QTestLibArgsParser::LightXmlFormat; f++) {
            args.clear();

            switch ((QTestLibArgsParser::TestVerbosity) v) {
            case QTestLibArgsParser::Silent:
                args << "-silent";
                break;
            case QTestLibArgsParser::NormalVerbosity:
                break;
            case QTestLibArgsParser::Verbose1:
                args << "-v1";
                break;
            case QTestLibArgsParser::Verbose2:
                args << "-v2";
                break;
            case QTestLibArgsParser::VerboseSignal:
                args << "-vs";
                break;
            default:
                qWarning() << "Unhandled verbosity value used:" << v;
                break;
            }

            switch ((QTestLibArgsParser::TestOutputFormat) f) {
            case QTestLibArgsParser::TxtFormat:
                argsVersion = qrand() % 3;
                if (argsVersion == 1)
                    args << "-txt";
                else if (argsVersion == 2)
                    args << "-o" << "-,txt";
                break;
            case QTestLibArgsParser::CsvFormat:
                argsVersion = qrand() % 2;
                if (argsVersion == 1)
                    args << "-csv";
                else
                    args << "-o" << "-,csv";
                break;
            case QTestLibArgsParser::XmlFormat:
                argsVersion = qrand() % 2;
                if (argsVersion == 1)
                    args << "-xml";
                else
                    args << "-o" << "-,xml";
                break;
            case QTestLibArgsParser::XUnitXmlFormat:
                argsVersion = qrand() % 2;
                if (argsVersion == 1)
                    args << "-xunitxml";
                else
                    args << "-o" << "-,xunitxml";
                break;
            case QTestLibArgsParser::LightXmlFormat:
                argsVersion = qrand() % 2;
                if (argsVersion == 1)
                    args << "-lightxml";
                else
                    args << "-o" << "-,lightxml";
                break;
            default:
                break;
            }

            unsigned int maxWarnings = 1000*(qrand() % 1000);
            int eventDelay = 10*(qrand() % 1000);
            int keyDelay = 10*(qrand() % 1000);
            int mouseDelay = 10*(qrand() % 1000);
            bool crashHandler = qrand() % 2;
            args << "-maxwarnings" << QString::number(maxWarnings, 10);
            args << "-eventdelay" << QString::number(eventDelay, 10);
            args << "-keydelay" << QString::number(keyDelay, 10);
            args << "-mousedelay" << QString::number(mouseDelay, 10);
            if (!crashHandler)
                args << "-nocrashhandler";

            QTest::newRow(qPrintable(args.join(' '))) << args.join(' ') << (QTestLibArgsParser::TestOutputFormat) f << (QTestLibArgsParser::TestVerbosity) v << maxWarnings << eventDelay << keyDelay << mouseDelay << crashHandler;
        }
    }
}

void QTestLibArgsParserTest::parse(void)
{
    QFETCH(QString, args);
    QFETCH(QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(QTestLibArgsParser::TestVerbosity, verbosity);
    QFETCH(unsigned int, maxWarings);
    QFETCH(int, eventDelay);
    QFETCH(int, keyDelay);
    QFETCH(int, mouseDelay);
    QFETCH(bool, crashHandler);

    QTestLibArgsParser parser(args);

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkOutput(parser, verbosity, format));
    SUB_TEST_FUNCTION(checkDelays(parser, eventDelay, keyDelay, mouseDelay));
    SUB_TEST_FUNCTION(checkOutputMode(parser));

    QVERIFY(parser.maxWarnings() == maxWarings);
    QVERIFY(parser.crashHandlerEnabled() == crashHandler);
}

void QTestLibArgsParserTest::copyParse(void)
{
    QFETCH(QString, args);
    QFETCH(QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(QTestLibArgsParser::TestVerbosity, verbosity);
    QFETCH(unsigned int, maxWarings);
    QFETCH(int, eventDelay);
    QFETCH(int, keyDelay);
    QFETCH(int, mouseDelay);
    QFETCH(bool, crashHandler);

    QTestLibArgsParser parser(args);
    QTestLibArgsParser copy(parser);

    SUB_TEST_FUNCTION(checkError(copy));
    SUB_TEST_FUNCTION(checkOutput(copy, verbosity, format));
    SUB_TEST_FUNCTION(checkDelays(copy, eventDelay, keyDelay, mouseDelay));
    SUB_TEST_FUNCTION(checkOutputMode(copy));

    QVERIFY(copy.maxWarnings() == maxWarings);
    QVERIFY(copy.crashHandlerEnabled() == crashHandler);
}


void QTestLibArgsParserTest::incrementalParse_data(void)
{
    int argsVersion;
    QStringList args;
    QLinkedList<QTestLibArgsParser::Error> errs;
    QStringList::const_iterator argsIt;
    QLinkedList<QTestLibArgsParser::Error>::const_iterator errsIt;

    QTest::addColumn<QString>("args");
    QTest::addColumn<QTestLibArgsParser::Error>("err");
    QTest::addColumn<QTestLibArgsParser::TestOutputFormat>("format");
    QTest::addColumn<QTestLibArgsParser::TestVerbosity>("verbosity");
    QTest::addColumn<unsigned int>("maxWarings");
    QTest::addColumn<int>("eventDelay");
    QTest::addColumn<int>("keyDelay");
    QTest::addColumn<int>("mouseDelay");
    QTest::addColumn<bool>("crashHandler");

    QTestLibArgsParser::TestVerbosity verbosity = (QTestLibArgsParser::TestVerbosity) ((int) QTestLibArgsParser::Silent + (qrand() % 5));
    QTestLibArgsParser::TestOutputFormat format = (QTestLibArgsParser::TestOutputFormat) ((int) QTestLibArgsParser::TxtFormat + (qrand() % 5));
    unsigned int maxWarnings = 1000*(qrand() % 1000);
    int eventDelay = 10*(qrand() % 1000);
    int keyDelay = 10*(qrand() % 1000);
    int mouseDelay = 10*(qrand() % 1000);
    bool crashHandler = qrand() % 2;

    QTest::newRow("[none]") << "" << QTestLibArgsParser::NoError << QTestLibArgsParser::TxtFormat << QTestLibArgsParser::NormalVerbosity << 2000u << -1 << -1 << -1 << true;
    args.clear();
    errs.clear();

    switch (verbosity) {
    case QTestLibArgsParser::Silent:
        args << "-silent";
        errs << QTestLibArgsParser::NoError;
        break;
    case QTestLibArgsParser::NormalVerbosity:
        break;
    case QTestLibArgsParser::Verbose1:
        args << "-v1";
        errs << QTestLibArgsParser::NoError;
        break;
    case QTestLibArgsParser::Verbose2:
        args << "-v2";
        errs << QTestLibArgsParser::NoError;
        break;
    case QTestLibArgsParser::VerboseSignal:
        args << "-vs";
        errs << QTestLibArgsParser::NoError;
        break;
    default:
        qWarning() << "Unhandled verbosity value used:" << verbosity;
        break;
    }

    for (argsIt = args.constBegin(), errsIt = errs.constBegin(); (argsIt != args.constEnd()) && (errsIt != errs.constEnd()); argsIt++, errsIt++) {
        if (*errsIt == QTestLibArgsParser::NoError)
            QTest::newRow(qPrintable(*argsIt)) << *argsIt << QTestLibArgsParser::NoError << QTestLibArgsParser::TxtFormat << verbosity << 2000u << -1 << -1 << -1 << true;
        else
            QTest::newRow(qPrintable(*argsIt)) << *argsIt << QTestLibArgsParser::PrematureEndError << QTestLibArgsParser::TxtFormat << QTestLibArgsParser::NormalVerbosity << 2000u << -1 << -1 << -1 << true;
    }
    args.clear();
    errs.clear();

    switch (format) {
    case QTestLibArgsParser::TxtFormat:
        argsVersion = qrand() % 3;
        if (argsVersion == 1) {
            args << "-txt";
            errs << QTestLibArgsParser::NoError;
        } else if (argsVersion == 2) {
            args << "-o" << "-,txt";
            errs << QTestLibArgsParser::PrematureEndError << QTestLibArgsParser::NoError;
        }
        break;
    case QTestLibArgsParser::CsvFormat:
        argsVersion = qrand() % 2;
        if (argsVersion == 1) {
            args << "-csv";
            errs << QTestLibArgsParser::NoError;
        } else {
            args << "-o" << "-,csv";
            errs << QTestLibArgsParser::PrematureEndError << QTestLibArgsParser::NoError;
        }
        break;
    case QTestLibArgsParser::XmlFormat:
        argsVersion = qrand() % 2;
        if (argsVersion == 1) {
            args << "-xml";
            errs << QTestLibArgsParser::NoError;
        } else {
            args << "-o" << "-,xml";
            errs << QTestLibArgsParser::PrematureEndError << QTestLibArgsParser::NoError;
        }
        break;
    case QTestLibArgsParser::XUnitXmlFormat:
        argsVersion = qrand() % 2;
        if (argsVersion == 1) {
            args << "-xunitxml";
            errs << QTestLibArgsParser::NoError;
        } else {
            args << "-o" << "-,xunitxml";
            errs << QTestLibArgsParser::PrematureEndError << QTestLibArgsParser::NoError;
        }
        break;
    case QTestLibArgsParser::LightXmlFormat:
        argsVersion = qrand() % 2;
        if (argsVersion == 1) {
            args << "-lightxml";
            errs << QTestLibArgsParser::NoError;
        } else {
            args << "-o" << "-,lightxml";
            errs << QTestLibArgsParser::PrematureEndError << QTestLibArgsParser::NoError;
        }
        break;
    default:
        break;
    }

    for (argsIt = args.constBegin(), errsIt = errs.constBegin(); (argsIt != args.constEnd()) && (errsIt != errs.constEnd()); argsIt++, errsIt++) {
        if (*errsIt == QTestLibArgsParser::NoError)
            QTest::newRow(qPrintable(*argsIt)) << *argsIt << QTestLibArgsParser::NoError << format << verbosity << 2000u << -1 << -1 << -1 << true;
        else
            QTest::newRow(qPrintable(*argsIt)) << *argsIt << QTestLibArgsParser::PrematureEndError << QTestLibArgsParser::TxtFormat << verbosity << 2000u << -1 << -1 << -1 << true;
    }
    args.clear();
    errs.clear();

    args << "-maxwarnings" << QString::number(maxWarnings, 10);
    errs << QTestLibArgsParser::PrematureEndError << QTestLibArgsParser::NoError;
    for (argsIt = args.constBegin(), errsIt = errs.constBegin(); (argsIt != args.constEnd()) && (errsIt != errs.constEnd()); argsIt++, errsIt++) {
        if (*errsIt == QTestLibArgsParser::NoError)
            QTest::newRow(qPrintable(*argsIt)) << *argsIt << QTestLibArgsParser::NoError << format << verbosity << maxWarnings << -1 << -1 << -1 << true;
        else
            QTest::newRow(qPrintable(*argsIt)) << *argsIt << QTestLibArgsParser::PrematureEndError << format << verbosity << 2000u << -1 << -1 << -1 << true;
    }
    args.clear();
    errs.clear();

    args << "-eventdelay" << QString::number(eventDelay, 10);
    errs << QTestLibArgsParser::PrematureEndError << QTestLibArgsParser::NoError;
    for (argsIt = args.constBegin(), errsIt = errs.constBegin(); (argsIt != args.constEnd()) && (errsIt != errs.constEnd()); argsIt++, errsIt++) {
        if (*errsIt == QTestLibArgsParser::NoError)
            QTest::newRow(qPrintable(*argsIt)) << *argsIt << QTestLibArgsParser::NoError << format << verbosity << maxWarnings << eventDelay << -1 << -1 << true;
        else
            QTest::newRow(qPrintable(*argsIt)) << *argsIt << QTestLibArgsParser::PrematureEndError << format << verbosity << maxWarnings << -1 << -1 << -1 << true;
    }
    args.clear();
    errs.clear();

    args << "-keydelay" << QString::number(keyDelay, 10);
    errs << QTestLibArgsParser::PrematureEndError << QTestLibArgsParser::NoError;
    for (argsIt = args.constBegin(), errsIt = errs.constBegin(); (argsIt != args.constEnd()) && (errsIt != errs.constEnd()); argsIt++, errsIt++) {
        if (*errsIt == QTestLibArgsParser::NoError)
            QTest::newRow(qPrintable(*argsIt)) << *argsIt << QTestLibArgsParser::NoError << format << verbosity << maxWarnings << eventDelay << keyDelay << -1 << true;
        else
            QTest::newRow(qPrintable(*argsIt)) << *argsIt << QTestLibArgsParser::PrematureEndError << format << verbosity << maxWarnings << eventDelay << -1 << -1 << true;
    }
    args.clear();
    errs.clear();

    args << "-mousedelay" << QString::number(mouseDelay, 10);
    errs << QTestLibArgsParser::PrematureEndError << QTestLibArgsParser::NoError;
    for (argsIt = args.constBegin(), errsIt = errs.constBegin(); (argsIt != args.constEnd()) && (errsIt != errs.constEnd()); argsIt++, errsIt++) {
        if (*errsIt == QTestLibArgsParser::NoError)
            QTest::newRow(qPrintable(*argsIt)) << *argsIt << QTestLibArgsParser::NoError << format << verbosity << maxWarnings << eventDelay << keyDelay << mouseDelay << true;
        else
            QTest::newRow(qPrintable(*argsIt)) << *argsIt << QTestLibArgsParser::PrematureEndError << format << verbosity << maxWarnings << eventDelay << keyDelay << -1 << true;
    }
    args.clear();
    errs.clear();

    if (!crashHandler) {
        args << "-nocrashhandler";
        errs << QTestLibArgsParser::NoError;
    }
    for (argsIt = args.constBegin(), errsIt = errs.constBegin(); (argsIt != args.constEnd()) && (errsIt != errs.constEnd()); argsIt++, errsIt++) {
        if (*errsIt == QTestLibArgsParser::NoError)
            QTest::newRow(qPrintable(*argsIt)) << *argsIt << QTestLibArgsParser::NoError << format << verbosity << maxWarnings << eventDelay << keyDelay << mouseDelay << crashHandler;
        else
            QTest::newRow(qPrintable(*argsIt)) << *argsIt << QTestLibArgsParser::PrematureEndError << format << verbosity << maxWarnings << eventDelay << keyDelay << mouseDelay << true;
    }
    args.clear();
    errs.clear();
}

void QTestLibArgsParserTest::incrementalParse(void)
{
    QFETCH(QString, args);
    QFETCH(QTestLibArgsParser::Error, err);
    QFETCH(QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(QTestLibArgsParser::TestVerbosity, verbosity);
    QFETCH(unsigned int, maxWarings);
    QFETCH(int, eventDelay);
    QFETCH(int, keyDelay);
    QFETCH(int, mouseDelay);
    QFETCH(bool, crashHandler);

    mParser.appendArgs(args);

    QString errStr;
    if (err == QTestLibArgsParser::PrematureEndError)
        errStr = "String of command line arguments ended prematurely";

    SUB_TEST_FUNCTION(checkError(mParser, err, errStr));
    SUB_TEST_FUNCTION(checkOutput(mParser, verbosity, format));
    SUB_TEST_FUNCTION(checkDelays(mParser, eventDelay, keyDelay, mouseDelay));
    SUB_TEST_FUNCTION(checkOutputMode(mParser));

    QVERIFY(mParser.maxWarnings() == maxWarings);
    QVERIFY(mParser.crashHandlerEnabled() == crashHandler);
}

void QTestLibArgsParserTest::checkError(const QTestLibArgsParser& parser, QTestLibArgsParser::Error error, const QString& errorString)
{
    BEGIN_SUB_TEST_FUNCTION

    QVERIFY(parser.error() == error);
    if (errorString.isNull())
        QVERIFY(parser.errorString().isNull());
    else
        QVERIFY(QString::compare(parser.errorString(), errorString, Qt::CaseSensitive) == 0);

    END_SUB_TEST_FUNCTION
}

void QTestLibArgsParserTest::checkOutput(const QTestLibArgsParser& parser, QTestLibArgsParser::TestVerbosity verb, QTestLibArgsParser::TestOutputFormat format, const QString& filename)
{
    BEGIN_SUB_TEST_FUNCTION

    QVERIFY(parser.verbosity() == verb);
    QVERIFY(parser.outputFormat() == format);
    if (filename.isNull())
        QVERIFY(parser.outFileName().toString().isEmpty());
    else
        QVERIFY(QString::compare(parser.outFileName().toString(), filename, Qt::CaseSensitive) == 0);

    END_SUB_TEST_FUNCTION
}

void QTestLibArgsParserTest::checkDelays(const QTestLibArgsParser& parser, int event, int key, int mouse)
{
    BEGIN_SUB_TEST_FUNCTION

    QVERIFY(parser.eventDelay() ==event);
    QVERIFY(parser.keyDelay() == key);
    QVERIFY(parser.mouseDelay() == mouse);

    END_SUB_TEST_FUNCTION
}

void QTestLibArgsParserTest::checkOutputMode(const QTestLibArgsParser& parser, QTestLibArgsParser::Output mode)
{
    BEGIN_SUB_TEST_FUNCTION

    switch (mode) {
    case QTestLibArgsParser::NormalOutput:
        QVERIFY(parser.isTestOutput());
        QVERIFY(!parser.isHelpOutput());
        QVERIFY(!parser.areFunctionsOutput());
        QVERIFY(!parser.areDatatagsOutput());
        break;
    case QTestLibArgsParser::HelpOutput:
        QVERIFY(!parser.isTestOutput());
        QVERIFY(parser.isHelpOutput());
        QVERIFY(!parser.areFunctionsOutput());
        QVERIFY(!parser.areDatatagsOutput());
        break;
    case QTestLibArgsParser::FunctionsOutput:
        QVERIFY(!parser.isTestOutput());
        QVERIFY(!parser.isHelpOutput());
        QVERIFY(parser.areFunctionsOutput());
        QVERIFY(!parser.areDatatagsOutput());
        break;
    case QTestLibArgsParser::DataTagsOutput:
        QVERIFY(!parser.isTestOutput());
        QVERIFY(!parser.isHelpOutput());
        QVERIFY(!parser.areFunctionsOutput());
        QVERIFY(parser.areDatatagsOutput());
        break;
    default:
        QVERIFY(!parser.isTestOutput());
        QVERIFY(!parser.isHelpOutput());
        QVERIFY(!parser.areFunctionsOutput());
        QVERIFY(!parser.areDatatagsOutput());
        break;
    }

    END_SUB_TEST_FUNCTION
}


void QTestLibArgsParserTest::testCases_data(void)
{
    QTest::addColumn<QString>("args");
    QTest::addColumn<QTestLibArgsParser::TestCaseList>("list");

    QTestLibArgsParser::TestCaseList list;

    list.clear();
    list << qMakePair(QString("test"), QStringList());
    QTest::newRow("test") << "test" << list;
    list.clear();
    list << qMakePair(QString("test1"), QStringList());
    QTest::newRow("test1") << "test1" << list;
    list.clear();
    list << qMakePair(QString("test_"), QStringList());
    QTest::newRow("test_") << "test_" << list;
    list.clear();
    list << qMakePair(QString("_test"), QStringList());
    QTest::newRow("_test") << "_test" << list;
    list.clear();
    list << qMakePair(QString("Test"), QStringList());
    QTest::newRow("Test") << "Test" << list;

    list.clear();
    list << qMakePair(QString("test"), QStringList() << "case");
    QTest::newRow("test:case") << "test:case" << list;
    list.clear();
    list << qMakePair(QString("test"), QStringList() << "case1");
    QTest::newRow("test:case1") << "test:case1" << list;
    list.clear();
    list << qMakePair(QString("test"), QStringList() << "case_");
    QTest::newRow("test:case1") << "test:case_" << list;
    list.clear();
    list << qMakePair(QString("test"), QStringList() << "Case");
    QTest::newRow("test:Case") << "test:Case" << list;
    list.clear();
    list << qMakePair(QString("test"), QStringList() << "case 1");
    QTest::newRow("\"test:case 1\"") << "\"test:case 1\"" << list;
    list.clear();
    list << qMakePair(QString("test"), QStringList() << "case 1");
    QTest::newRow("test:\"case 1\"") << "test:\"case 1\"" << list;
    list.clear();
    list << qMakePair(QString("test"), QStringList() << "case:desc");
    QTest::newRow("test:case:desc") << "test:case:desc" << list;
    list.clear();
    list << qMakePair(QString("test"), QStringList() << "1,2,3");
    QTest::newRow("test:1,2,3") << "test:1,2,3" << list;
    list.clear();
    list << qMakePair(QString("test"), QStringList() << "(1,2,3)");
    QTest::newRow("test:(1,2,3)") << "test:(1,2,3)" << list;

    list.clear();
    list << qMakePair(QString("test"), QStringList() << "subtest1" << "subtest2");
    QTest::newRow("test:subtest1 test:subtest2") << "test:subtest1 test:subtest2" << list;
    list.clear();
    list << qMakePair(QString("test1"), QStringList() << "subtest1");
    list << qMakePair(QString("test2"), QStringList() << "subtest2");
    QTest::newRow("test1:subtest1 test2:subtest2") << "test1:subtest1 test2:subtest2" << list;
    list.clear();
    list << qMakePair(QString("test1"), QStringList() << "subtest11" << "subtest12");
    list << qMakePair(QString("test2"), QStringList() << "subtest21" << "subtest22");
    QTest::newRow("test1:subtest11 test2:subtest21 test1:subtest12 test2:subtest22") << "test1:subtest11 test2:subtest21 test1:subtest12 test2:subtest22" << list;
    list.clear();
    list << qMakePair(QString("test"), QStringList() << "subtest");
    QTest::newRow("test:subtest test:subtest") << "test:subtest test:subtest" << list;
    list.clear();
    list << qMakePair(QString("test1"), QStringList() << "subtest1");
    list << qMakePair(QString("test2"), QStringList() << "subtest2");
    QTest::newRow("test1:subtest1 test2:subtest2 test1:subtest1") << "test1:subtest1 test2:subtest2 test1:subtest1" << list;
    list.clear();
    list << qMakePair(QString("test"), QStringList());
    QTest::newRow("test:subtest test") << "test:subtest test" << list;
    list.clear();
    list << qMakePair(QString("test1"), QStringList());
    list << qMakePair(QString("test2"), QStringList() << "subtest2");
    QTest::newRow("test1:subtest1 test2:subtest2 test1") << "test1:subtest1 test2:subtest2 test1" << list;
}

void QTestLibArgsParserTest::testCases(void)
{
    QFETCH(QString, args);
    QFETCH(QTestLibArgsParser::TestCaseList, list);

    QTestLibArgsParser parser(args);

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkSelectedTestCases(parser, list));
}

void QTestLibArgsParserTest::removeTestCases_data()
{
    QTest::addColumn<QString>("args");
    QTest::addColumn<QTestLibArgsParser::TestCaseList>("listBefore");
    QTest::addColumn<QString>("test");
    QTest::addColumn<QString>("data");
    QTest::addColumn<QTestLibArgsParser::TestCaseList>("listAfter");

    QTestLibArgsParser::TestCaseList listBefore;
    QTestLibArgsParser::TestCaseList listAfter;

    listBefore.clear();
    listBefore << qMakePair(QString("test"), QStringList());
    listAfter.clear();
    QTest::newRow("test/test") << "test" << listBefore << "test" << QString() << listAfter;
    listAfter.clear();
    listAfter << qMakePair(QString("test"), QStringList());
    QTest::newRow("test/case") << "test" << listBefore << "case" << QString() << listAfter;
    listAfter.clear();
    listAfter << qMakePair(QString("test"), QStringList());
    QTest::newRow("test/test:case") << "test" << listBefore << "test" << "case" << listAfter;

    listBefore.clear();
    listBefore << qMakePair(QString("test"), QStringList() << "subtest");
    listAfter.clear();
    QTest::newRow("test:subtest/test") << "test:subtest" << listBefore << "test" << QString() << listAfter;
    listAfter.clear();
    QTest::newRow("test:subtest/test:subtest") << "test:subtest" << listBefore << "test" << "subtest" << listAfter;
    listAfter.clear();
    listAfter << qMakePair(QString("test"), QStringList() << "subtest");
    QTest::newRow("test:subtest/case") << "test:subtest" << listBefore << "case" << QString() << listAfter;
    listAfter.clear();
    listAfter << qMakePair(QString("test"), QStringList() << "subtest");
    QTest::newRow("test:subtest/test:case") << "test:subtest" << listBefore << "test" << "case" << listAfter;

    listBefore.clear();
    listBefore << qMakePair(QString("test"), QStringList() << "subtest1" << "subtest2");
    listAfter.clear();
    QTest::newRow("test:subtest1 test:subtest2/test") << "test:subtest1 test:subtest2" << listBefore << "test" << QString() << listAfter;
    listAfter.clear();
    listAfter << qMakePair(QString("test"), QStringList() << "subtest1");
    QTest::newRow("test:subtest1 test:subtest2/test:subtest2") << "test:subtest1 test:subtest2" << listBefore << "test" << "subtest2" << listAfter;
    listAfter.clear();
    listAfter << qMakePair(QString("test"), QStringList() << "subtest2");
    QTest::newRow("test:subtest1 test:subtest2/test:subtest1") << "test:subtest1 test:subtest2" << listBefore << "test" << "subtest1" << listAfter;
    listAfter.clear();
    listAfter << qMakePair(QString("test"), QStringList() << "subtest1" << "subtest2");
    QTest::newRow("test:subtest1 test:subtest2/test:subtest") << "test:subtest1 test:subtest2" << listBefore << "test" << "subtest" << listAfter;
    listAfter.clear();
    listAfter << qMakePair(QString("test"), QStringList() << "subtest1" << "subtest2");
    QTest::newRow("test:subtest1 test:subtest2/case") << "test:subtest1 test:subtest2" << listBefore << "case" << QString() << listAfter;
    listAfter.clear();
    listAfter << qMakePair(QString("test"), QStringList() << "subtest1" << "subtest2");
    QTest::newRow("test:subtest1 test:subtest2/case:subtest2") << "test:subtest1 test:subtest2" << listBefore << "case" << "subtest2" << listAfter;
    listAfter.clear();
    listAfter << qMakePair(QString("test"), QStringList() << "subtest1" << "subtest2");
    QTest::newRow("test:subtest1 test:subtest2/case:subtest1") << "test:subtest1 test:subtest2" << listBefore << "case" << "subtest1" << listAfter;

    listBefore.clear();
    listBefore << qMakePair(QString("test1"), QStringList() << "subtest1");
    listBefore << qMakePair(QString("test2"), QStringList() << "subtest2");
    listAfter.clear();
    listAfter << qMakePair(QString("test1"), QStringList() << "subtest1");
    QTest::newRow("test1:subtest1 test2:subtest2/test2") << "test1:subtest1 test2:subtest2" << listBefore << "test2" << QString() << listAfter;
    listAfter.clear();
    listAfter << qMakePair(QString("test2"), QStringList() << "subtest2");
    QTest::newRow("test1:subtest1 test2:subtest2/test1") << "test1:subtest1 test2:subtest2" << listBefore << "test1" << QString() << listAfter;
    listAfter.clear();
    listAfter << qMakePair(QString("test1"), QStringList() << "subtest1");
    listAfter << qMakePair(QString("test2"), QStringList() << "subtest2");
    QTest::newRow("test1:subtest1 test2:subtest2/test") << "test1:subtest1 test2:subtest2" << listBefore << "test" << QString() << listAfter;
    listAfter.clear();
    listAfter << qMakePair(QString("test1"), QStringList() << "subtest1");
    QTest::newRow("test1:subtest1 test2:subtest2/test2:subtest2") << "test1:subtest1 test2:subtest2" << listBefore << "test2" << "subtest2" << listAfter;
    listAfter.clear();
    listAfter << qMakePair(QString("test2"), QStringList() << "subtest2");
    QTest::newRow("test1:subtest1 test2:subtest2/test1:subtest1") << "test1:subtest1 test2:subtest2" << listBefore << "test1" << "subtest1" << listAfter;
    listAfter.clear();
    listAfter << qMakePair(QString("test1"), QStringList() << "subtest1");
    listAfter << qMakePair(QString("test2"), QStringList() << "subtest2");
    QTest::newRow("test1:subtest1 test2:subtest2/test:subtest1") << "test1:subtest1 test2:subtest2" << listBefore << "test" << "subtest1" << listAfter;
    listAfter.clear();
    listAfter << qMakePair(QString("test1"), QStringList() << "subtest1");
    listAfter << qMakePair(QString("test2"), QStringList() << "subtest2");
    QTest::newRow("test1:subtest1 test2:subtest2/test:subtest1") << "test1:subtest1 test2:subtest2" << listBefore << "test" << "subtest2" << listAfter;
    listAfter.clear();
    listAfter << qMakePair(QString("test1"), QStringList() << "subtest1");
    listAfter << qMakePair(QString("test2"), QStringList() << "subtest2");
    QTest::newRow("test1:subtest1 test2:subtest2/test2:subtest") << "test1:subtest1 test2:subtest2" << listBefore << "test2" << "subtest" << listAfter;
    listAfter.clear();
    listAfter << qMakePair(QString("test1"), QStringList() << "subtest1");
    listAfter << qMakePair(QString("test2"), QStringList() << "subtest2");
    QTest::newRow("test1:subtest1 test2:subtest2/test1:subtest") << "test1:subtest1 test2:subtest2" << listBefore << "test1" << "subtest" << listAfter;
    listAfter.clear();
    listAfter << qMakePair(QString("test1"), QStringList() << "subtest1");
    listAfter << qMakePair(QString("test2"), QStringList() << "subtest2");
    QTest::newRow("test1:subtest1 test2:subtest2/test:subtest") << "test1:subtest1 test2:subtest2" << listBefore << "test" << "subtest" << listAfter;
}

void QTestLibArgsParserTest::removeTestCases(void)
{
    QFETCH(QString, args);
    QFETCH(QTestLibArgsParser::TestCaseList, listBefore);
    QFETCH(QString, test);
    QFETCH(QString, data);
    QFETCH(QTestLibArgsParser::TestCaseList, listAfter);

    QTestLibArgsParser parser(args);

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkSelectedTestCases(parser, listBefore));

    parser.removeTestCases(test, data);

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkSelectedTestCases(parser, listAfter));
}

void QTestLibArgsParserTest::checkSelectedTestCases(const QTestLibArgsParser& parser, const QTestLibArgsParser::TestCaseList& list)
{
    BEGIN_SUB_TEST_FUNCTION

    QCOMPARE(parser.selectedTestCases().size(), list.size());

    for(QTestLibArgsParser::TestCaseList::const_iterator parserIt = parser.selectedTestCases().constBegin(); parserIt != parser.selectedTestCases().constEnd(); parserIt++) {
        QTestLibArgsParser::TestCaseList::const_iterator listIt;
        for(listIt = list.constBegin(); listIt != list.constEnd(); listIt++) {
            if (QString::compare((*parserIt).first, (*listIt).first, Qt::CaseSensitive) == 0) {
                QCOMPARE((*parserIt).second.size(), (*listIt).second.size());

                for (QStringList::const_iterator parserDataIt = (*parserIt).second.constBegin(); parserDataIt != (*parserIt).second.constEnd(); parserDataIt++) {
                    QStringList::const_iterator listDataIt;
                    for (listDataIt = (*listIt).second.constBegin(); listDataIt != (*listIt).second.constEnd(); listDataIt++) {
                        if (QString::compare(*parserDataIt, *listDataIt, Qt::CaseSensitive) == 0)
                            break;
                    }
                    QVERIFY(listDataIt != (*listIt).second.constEnd());
                }

                break;
            }
        }
        QVERIFY(listIt != list.constEnd());
    }

    END_SUB_TEST_FUNCTION
}


void QTestLibArgsParserTest::flagError_data(void)
{
    QTest::addColumn<QString>("args");
    QTest::addColumn<QTestLibArgsParser::Error>("err");
    QTest::addColumn<QString>("errStr");

    QTest::newRow("-xxx") << "-xxx" << QTestLibArgsParser::UnknownFlagError << "Unknown flag \"-xxx\"";
    QTest::newRow("-,txt") << "-,txt" << QTestLibArgsParser::UnknownFlagError << "Unknown flag \"-,txt\"";
    QTest::newRow("-o-,txt") << "-o-,txt" << QTestLibArgsParser::UnknownFlagError << "Unknown flag \"-o-,txt\"";
    QTest::newRow("-\"txt\"") << "-\"txt\"" << QTestLibArgsParser::UnknownFlagError << "Unknown flag \"-\"txt\"\"";
    QTest::newRow("-silent -xxx") << "-silent -xxx" << QTestLibArgsParser::UnknownFlagError << "Unknown flag \"-xxx\"";
    QTest::newRow("-silent -,txt") << "-silent -,txt" << QTestLibArgsParser::UnknownFlagError << "Unknown flag \"-,txt\"";
    QTest::newRow("-silent -o-,txt") << "-silent -o-,txt" << QTestLibArgsParser::UnknownFlagError << "Unknown flag \"-o-,txt\"";
    QTest::newRow("-maxwarnings 2000 -xxx") << "-maxwarnings 2000 -xxx" << QTestLibArgsParser::UnknownFlagError << "Unknown flag \"-xxx\"";
    QTest::newRow("-maxwarnings 2000 -,txt") << "-maxwarnings 2000 -,txt" << QTestLibArgsParser::UnknownFlagError << "Unknown flag \"-,txt\"";
    QTest::newRow("-maxwarnings 2000 -o-,txt") << "-maxwarnings 2000 -o-,txt" << QTestLibArgsParser::UnknownFlagError << "Unknown flag \"-o-,txt\"";
}

void QTestLibArgsParserTest::flagError(void)
{
    QFETCH(QString, args);
    QFETCH(QTestLibArgsParser::Error, err);
    QFETCH(QString, errStr);

    QTestLibArgsParser parser(args);

    QCOMPARE(parser.error(), err);
    QCOMPARE(parser.errorString(), errStr);
}

void QTestLibArgsParserTest::invalidArgumentError_data(void)
{
    QTest::addColumn<QString>("args");
    QTest::addColumn<QTestLibArgsParser::Error>("err");
    QTest::addColumn<QString>("errStr");

    QTest::newRow("-maxwarnings xxx") << "-maxwarnings xxx" << QTestLibArgsParser::InvalidArgumentError << "Got \"xxx\" where unsigned integer was expected.";
    QTest::newRow("-maxwarnings 10s") << "-maxwarnings 10s" << QTestLibArgsParser::InvalidArgumentError << "Got \"10s\" where unsigned integer was expected.";
    QTest::newRow("-maxwarnings x10") << "-maxwarnings x10" << QTestLibArgsParser::InvalidArgumentError << "Got \"x10\" where unsigned integer was expected.";
    QTest::newRow("-maxwarnings -10") << "-maxwarnings -10" << QTestLibArgsParser::InvalidArgumentError << "Got \"-10\" where unsigned integer was expected.";
    QTest::newRow("-maxwarnings 10.") << "-maxwarnings 10." << QTestLibArgsParser::InvalidArgumentError << "Got \"10.\" where unsigned integer was expected.";
    QTest::newRow("-maxwarnings 1.0") << "-maxwarnings 1.0" << QTestLibArgsParser::InvalidArgumentError << "Got \"1.0\" where unsigned integer was expected.";

    QTest::newRow("-eventdelay xxx") << "-eventdelay xxx" << QTestLibArgsParser::InvalidArgumentError << "Got \"xxx\" where integer was expected.";
    QTest::newRow("-eventdelay 10s") << "-eventdelay 10s" << QTestLibArgsParser::InvalidArgumentError << "Got \"10s\" where integer was expected.";
    QTest::newRow("-eventdelay x10") << "-eventdelay x10" << QTestLibArgsParser::InvalidArgumentError << "Got \"x10\" where integer was expected.";
    QTest::newRow("-eventdelay 10.") << "-eventdelay 10." << QTestLibArgsParser::InvalidArgumentError << "Got \"10.\" where integer was expected.";
    QTest::newRow("-eventdelay 1.0") << "-eventdelay 1.0" << QTestLibArgsParser::InvalidArgumentError << "Got \"1.0\" where integer was expected.";

    QTest::newRow("-keydelay xxx") << "-keydelay xxx" << QTestLibArgsParser::InvalidArgumentError << "Got \"xxx\" where integer was expected.";
    QTest::newRow("-keydelay 10s") << "-keydelay 10s" << QTestLibArgsParser::InvalidArgumentError << "Got \"10s\" where integer was expected.";
    QTest::newRow("-keydelay x10") << "-keydelay x10" << QTestLibArgsParser::InvalidArgumentError << "Got \"x10\" where integer was expected.";
    QTest::newRow("-keydelay 10.") << "-keydelay 10." << QTestLibArgsParser::InvalidArgumentError << "Got \"10.\" where integer was expected.";
    QTest::newRow("-keydelay 1.0") << "-keydelay 1.0" << QTestLibArgsParser::InvalidArgumentError << "Got \"1.0\" where integer was expected.";

    QTest::newRow("-mousedelay xxx") << "-mousedelay xxx" << QTestLibArgsParser::InvalidArgumentError << "Got \"xxx\" where integer was expected.";
    QTest::newRow("-mousedelay 10s") << "-mousedelay 10s" << QTestLibArgsParser::InvalidArgumentError << "Got \"10s\" where integer was expected.";
    QTest::newRow("-mousedelay x10") << "-mousedelay x10" << QTestLibArgsParser::InvalidArgumentError << "Got \"x10\" where integer was expected.";
    QTest::newRow("-mousedelay 10.") << "-mousedelay 10." << QTestLibArgsParser::InvalidArgumentError << "Got \"10.\" where integer was expected.";
    QTest::newRow("-mousedelay 1.0") << "-mousedelay 1.0" << QTestLibArgsParser::InvalidArgumentError << "Got \"1.0\" where integer was expected.";
}

void QTestLibArgsParserTest::invalidArgumentError(void)
{
    QFETCH(QString, args);
    QFETCH(QTestLibArgsParser::Error, err);
    QFETCH(QString, errStr);

    QTestLibArgsParser parser(args);

    QCOMPARE(parser.error(), err);
    QCOMPARE(parser.errorString(), errStr);
}

void QTestLibArgsParserTest::invalidTestError_data(void)
{
    QTest::addColumn<QString>("args");
    QTest::addColumn<QTestLibArgsParser::Error>("err");
    QTest::addColumn<QString>("errStr");

    QTest::newRow("TESTARGS=\"-silent\"") << "TESTARGS=\"-silent\"" << QTestLibArgsParser::InvalidTestCaseError << "The given argument \"TESTARGS=\"-silent\"\" is neither a flag nor a test case";
    QTest::newRow("o-") << "o-" << QTestLibArgsParser::InvalidTestCaseError << "The given argument \"o-\" is neither a flag nor a test case";
    QTest::newRow("o-,txt") << "o-,txt" << QTestLibArgsParser::InvalidTestCaseError << "The given argument \"o-,txt\" is neither a flag nor a test case";
    QTest::newRow("0") << "0" << QTestLibArgsParser::InvalidTestCaseError << "The given argument \"0\" is neither a flag nor a test case";
    QTest::newRow("0test") << "0test" << QTestLibArgsParser::InvalidTestCaseError << "The given argument \"0test\" is neither a flag nor a test case";
    QTest::newRow("0:test") << "0:test" << QTestLibArgsParser::InvalidTestCaseError << "The given argument \"0:test\" is neither a flag nor a test case";
    QTest::newRow("0test:data") << "0test:data" << QTestLibArgsParser::InvalidTestCaseError << "The given argument \"0test:data\" is neither a flag nor a test case";

    QTest::newRow("-silent TESTARGS=\"-silent\"") << "-silent TESTARGS=\"-silent\"" << QTestLibArgsParser::InvalidTestCaseError << "The given argument \"TESTARGS=\"-silent\"\" is neither a flag nor a test case";
    QTest::newRow("-silent o-") << "-silent o-" << QTestLibArgsParser::InvalidTestCaseError << "The given argument \"o-\" is neither a flag nor a test case";
    QTest::newRow("-silent o-,txt") << "-silent o-,txt" << QTestLibArgsParser::InvalidTestCaseError << "The given argument \"o-,txt\" is neither a flag nor a test case";
    QTest::newRow("-silent 0") << "-silent 0" << QTestLibArgsParser::InvalidTestCaseError << "The given argument \"0\" is neither a flag nor a test case";
    QTest::newRow("-silent 0test") << "-silent 0test" << QTestLibArgsParser::InvalidTestCaseError << "The given argument \"0test\" is neither a flag nor a test case";
    QTest::newRow("-silent 0:test") << "-silent 0:test" << QTestLibArgsParser::InvalidTestCaseError << "The given argument \"0:test\" is neither a flag nor a test case";
    QTest::newRow("-silent 0test:data") << "-silent 0test:data" << QTestLibArgsParser::InvalidTestCaseError << "The given argument \"0test:data\" is neither a flag nor a test case";

    QTest::newRow("-maxwarnings 2000 TESTARGS=\"-silent\"") << "-maxwarnings 2000 TESTARGS=\"-silent\"" << QTestLibArgsParser::InvalidTestCaseError << "The given argument \"TESTARGS=\"-silent\"\" is neither a flag nor a test case";
    QTest::newRow("-maxwarnings 2000 o-") << "-maxwarnings 2000 o-" << QTestLibArgsParser::InvalidTestCaseError << "The given argument \"o-\" is neither a flag nor a test case";
    QTest::newRow("-maxwarnings 2000 o-,txt") << "-maxwarnings 2000 o-,txt" << QTestLibArgsParser::InvalidTestCaseError << "The given argument \"o-,txt\" is neither a flag nor a test case";
    QTest::newRow("-maxwarnings 2000 0") << "-maxwarnings 2000 0" << QTestLibArgsParser::InvalidTestCaseError << "The given argument \"0\" is neither a flag nor a test case";
    QTest::newRow("-maxwarnings 2000 0test") << "-maxwarnings 2000 0test" << QTestLibArgsParser::InvalidTestCaseError << "The given argument \"0test\" is neither a flag nor a test case";
    QTest::newRow("-maxwarnings 2000 0:test") << "-maxwarnings 2000 0:test" << QTestLibArgsParser::InvalidTestCaseError << "The given argument \"0:test\" is neither a flag nor a test case";
    QTest::newRow("-maxwarnings 2000 0test:data") << "-maxwarnings 2000 0test:data" << QTestLibArgsParser::InvalidTestCaseError << "The given argument \"0test:data\" is neither a flag nor a test case";
}

void QTestLibArgsParserTest::invalidTestError(void)
{
    QFETCH(QString, args);
    QFETCH(QTestLibArgsParser::Error, err);
    QFETCH(QString, errStr);

    QTestLibArgsParser parser(args);

    QCOMPARE(parser.error(), err);
    QCOMPARE(parser.errorString(), errStr);
}

void QTestLibArgsParserTest::prematureEndError_data(void)
{
    QTest::addColumn<QString>("args");
    QTest::addColumn<QTestLibArgsParser::Error>("err");
    QTest::addColumn<QString>("errStr");

    QTest::newRow("-o") << "-o" << QTestLibArgsParser::PrematureEndError << "String of command line arguments ended prematurely";
    QTest::newRow("-silent -o") << "-silent -o" << QTestLibArgsParser::PrematureEndError << "String of command line arguments ended prematurely";
    QTest::newRow("-maxwarnings") << "-maxwarnings" << QTestLibArgsParser::PrematureEndError << "String of command line arguments ended prematurely";
    QTest::newRow("-silent -maxwarnings") << "-silent -maxwarnings" << QTestLibArgsParser::PrematureEndError << "String of command line arguments ended prematurely";
    QTest::newRow("-eventdelay") << "-eventdelay" << QTestLibArgsParser::PrematureEndError << "String of command line arguments ended prematurely";
    QTest::newRow("-silent -eventdelay") << "-silent -eventdelay" << QTestLibArgsParser::PrematureEndError << "String of command line arguments ended prematurely";
    QTest::newRow("-keydelay") << "-keydelay" << QTestLibArgsParser::PrematureEndError << "String of command line arguments ended prematurely";
    QTest::newRow("-silent -keydelay") << "-silent -keydelay" << QTestLibArgsParser::PrematureEndError << "String of command line arguments ended prematurely";
    QTest::newRow("-mousedelay") << "-mousedelay" << QTestLibArgsParser::PrematureEndError << "String of command line arguments ended prematurely";
    QTest::newRow("-silent -mousedelay") << "-silent -mousedelay" << QTestLibArgsParser::PrematureEndError << "String of command line arguments ended prematurely";
}

void QTestLibArgsParserTest::prematureEndError(void)
{
    QFETCH(QString, args);
    QFETCH(QTestLibArgsParser::Error, err);
    QFETCH(QString, errStr);

    QTestLibArgsParser parser(args);

    QCOMPARE(parser.error(), err);
    QCOMPARE(parser.errorString(), errStr);
}

void QTestLibArgsParserTest::toStringOutput_data(void)
{
    QTest::addColumn<QTestLibArgsParser::Output>("type");
    QTest::addColumn<QStringList>("args");

    QStringList args;

    args.clear();
    QTest::newRow("normal") << QTestLibArgsParser::NormalOutput << args;
    args.clear();
    args << "-help";
    QTest::newRow("help") << QTestLibArgsParser::HelpOutput << args;
    args.clear();
    args << "-functions";
    QTest::newRow("functions") << QTestLibArgsParser::FunctionsOutput << args;
    args.clear();
    args << "-datatags";
    QTest::newRow("datatags") << QTestLibArgsParser::DataTagsOutput << args;
    args.clear();
    args << "-perfcounterlist";
    QTest::newRow("perfcounters") << QTestLibArgsParser::PerfCounterNamesOutput << args;
}

void QTestLibArgsParserTest::toStringOutput(void)
{
    QFETCH(QTestLibArgsParser::Output, type);
    QFETCH(QStringList, args);

    QTestLibArgsParser parser;
    parser.setOutput(type);

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkArguments(parser.toStringList(), args));
    QCOMPARE(parser.toString(), parser.toStringList().join(' '));
}

void QTestLibArgsParserTest::toStringFormatV1_data(void)
{
    QTest::addColumn<QTestLibArgsParser::TestOutputFormat>("format");
    QTest::addColumn<QStringList>("args");

    QStringList args;

    args.clear();
    QTest::newRow("txt") << QTestLibArgsParser::TxtFormat << args;
    args.clear();
    args << "-csv";
    QTest::newRow("csv") << QTestLibArgsParser::CsvFormat << args;
    args.clear();
    args << "-xml";
    QTest::newRow("xml") << QTestLibArgsParser::XmlFormat << args;
    args.clear();
    args << "-lightxml";
    QTest::newRow("lightxml") << QTestLibArgsParser::LightXmlFormat << args;
    args.clear();
    args << "-xunitxml";
    QTest::newRow("xunitxml") << QTestLibArgsParser::XUnitXmlFormat << args;
}

void QTestLibArgsParserTest::toStringFormatV1(void)
{
    QFETCH(QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(QStringList, args);

    QTestLibArgsParser parser;
    parser.setOutputFormat(format);

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkArguments(parser.toStringList(1), args));
    QCOMPARE(parser.toString(), parser.toStringList().join(' '));
}

void QTestLibArgsParserTest::toStringFormatV2_data(void)
{
    QTest::addColumn<QTestLibArgsParser::TestOutputFormat>("format");
    QTest::addColumn<QStringList>("args");

    QStringList args;

    args.clear();
    QTest::newRow("txt") << QTestLibArgsParser::TxtFormat << args;
    args.clear();
    args << "-o" << "-,csv";
    QTest::newRow("csv") << QTestLibArgsParser::CsvFormat << args;
    args.clear();
    args << "-o" << "-,xml";
    QTest::newRow("xml") << QTestLibArgsParser::XmlFormat << args;
    args.clear();
    args << "-o" << "-,lightxml";
    QTest::newRow("lightxml") << QTestLibArgsParser::LightXmlFormat << args;
    args.clear();
    args << "-o" << "-,xunitxml";
    QTest::newRow("xunitxml") << QTestLibArgsParser::XUnitXmlFormat << args;
}

void QTestLibArgsParserTest::toStringFormatV2(void)
{
    QFETCH(QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(QStringList, args);

    QTestLibArgsParser parser;
    parser.setOutputFormat(format);

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkArguments(parser.toStringList(2), args));
    QCOMPARE(parser.toString(), parser.toStringList(2).join(' '));
}

void QTestLibArgsParserTest::toStringFormat_data(void)
{
    QTest::addColumn<QTestLibArgsParser::TestOutputFormat>("format");
    QTest::addColumn<QStringList>("args");

    QStringList args;

    args.clear();
    QTest::newRow("txt") << QTestLibArgsParser::TxtFormat << args;
    args.clear();
    args << "-o" << "-,csv";
    QTest::newRow("csv") << QTestLibArgsParser::CsvFormat << args;
    args.clear();
    args << "-o" << "-,xml";
    QTest::newRow("xml") << QTestLibArgsParser::XmlFormat << args;
    args.clear();
    args << "-o" << "-,lightxml";
    QTest::newRow("lightxml") << QTestLibArgsParser::LightXmlFormat << args;
    args.clear();
    args << "-o" << "-,xunitxml";
    QTest::newRow("xunitxml") << QTestLibArgsParser::XUnitXmlFormat << args;
}

void QTestLibArgsParserTest::toStringFormat(void)
{
    QFETCH(QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(QStringList, args);

    QTestLibArgsParser parser;
    parser.setOutputFormat(format);

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkArguments(parser.toStringList(), args));
    QCOMPARE(parser.toString(), parser.toStringList().join(' '));
}

void QTestLibArgsParserTest::toStringOutFileV1_data(void)
{
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QStringList>("args");

    QStringList args;

    args.clear();
    QTest::newRow("Default") << QString() << args;
    args.clear();
    args << "-o" << "\"-\"";
    QTest::newRow("\"-\"") << "-"  << args;
    args.clear();
    args << "-o" << "testresult";
    QTest::newRow("testresult") << "testresult"  << args;
    args.clear();
    args << "-o" << "testResult";
    QTest::newRow("testResult") << "testResult"  << args;
    args.clear();
    args << "-o" << "test_result";
    QTest::newRow("test_result") << "test_result"  << args;
    args.clear();
    args << "-o" << "testresult1";
    QTest::newRow("testresult1") << "testresult1"  << args;
    args.clear();
    args << "-o" << "test-result";
    QTest::newRow("test-result") << "test-result"  << args;
    args.clear();
    args << "-o" << "\"testresult?\"";
    QTest::newRow("testresult?") << "testresult?"  << args;
    args.clear();
    args << "-o" << "\"testresult*\"";
    QTest::newRow("testresult*") << "testresult*"  << args;
    args.clear();
    args << "-o" << "testResult.log";
    QTest::newRow("testResult.log") << "testResult.log"  << args;
    args.clear();
    args << "-o" << "\"test result.log\"";
    QTest::newRow("test result.log") << "test result.log"  << args;
    args.clear();
    args << "-o" << "\"\\\"testResult\\\"\"";
    QTest::newRow("\"testResult\"") << "\"testResult\""  << args;
    args.clear();
    args << "-o" << "\"\\\"testResult.log\\\"\"";
    QTest::newRow("\"testResult.log\"") << "\"testResult.log\""  << args;
    args.clear();
    args << "-o" << "\"\\\"test result.log\\\"\"";
    QTest::newRow("\"test result.log\"") << "\"test result.log\""  << args;
}

void QTestLibArgsParserTest::toStringOutFileV1(void)
{
    QFETCH(QString, filename);
    QFETCH(QStringList, args);

    QTestLibArgsParser parser;
    parser.setOutFileName(Utils::FileName::fromString(filename));

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkArguments(parser.toStringList(1), args));
    QCOMPARE(parser.toString(), parser.toStringList(1).join(' '));
}

void QTestLibArgsParserTest::toStringOutFileV2_data(void)
{
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QStringList>("args");

    QStringList args;

    args.clear();
    QTest::newRow("Default") << QString() << args;
    args.clear();
    args << "-o" << "\"-\",txt";
    QTest::newRow("\"-\"") << "-"  << args;
    args.clear();
    args << "-o" << "testresult,txt";
    QTest::newRow("testresult") << "testresult"  << args;
    args.clear();
    args << "-o" << "testResult,txt";
    QTest::newRow("testResult") << "testResult"  << args;
    args.clear();
    args << "-o" << "test_result,txt";
    QTest::newRow("test_result") << "test_result"  << args;
    args.clear();
    args << "-o" << "testresult1,txt";
    QTest::newRow("testresult1") << "testresult1"  << args;
    args.clear();
    args << "-o" << "test-result,txt";
    QTest::newRow("test-result") << "test-result"  << args;
    args.clear();
    args << "-o" << "\"testresult?\",txt";
    QTest::newRow("testresult?") << "testresult?"  << args;
    args.clear();
    args << "-o" << "\"testresult*\",txt";
    QTest::newRow("testresult*") << "testresult*"  << args;
    args.clear();
    args << "-o" << "testResult.log,txt";
    QTest::newRow("testResult.log") << "testResult.log"  << args;
    args.clear();
    args << "-o" << "\"test result.log\",txt";
    QTest::newRow("test result.log") << "test result.log"  << args;
    args.clear();
    args << "-o" << "\"\\\"testResult\\\"\",txt";
    QTest::newRow("\"testResult\"") << "\"testResult\""  << args;
    args.clear();
    args << "-o" << "\"\\\"testResult.log\\\"\",txt";
    QTest::newRow("\"testResult.log\"") << "\"testResult.log\""  << args;
    args.clear();
    args << "-o" << "\"\\\"test result.log\\\"\",txt";
    QTest::newRow("\"test result.log\"") << "\"test result.log\""  << args;
}

void QTestLibArgsParserTest::toStringOutFileV2(void)
{
    QFETCH(QString, filename);
    QFETCH(QStringList, args);

    QTestLibArgsParser parser;
    parser.setOutFileName(Utils::FileName::fromString(filename));

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkArguments(parser.toStringList(2), args));
    QCOMPARE(parser.toString(), parser.toStringList(2).join(' '));
}

void QTestLibArgsParserTest::toStringOutFile_data(void)
{
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QStringList>("args");

    QStringList args;

    args.clear();
    QTest::newRow("Default") << QString() << args;
    args.clear();
    args << "-o" << "\"-\",txt";
    QTest::newRow("\"-\"") << "-"  << args;
    args.clear();
    args << "-o" << "testresult,txt";
    QTest::newRow("testresult") << "testresult"  << args;
    args.clear();
    args << "-o" << "testResult,txt";
    QTest::newRow("testResult") << "testResult"  << args;
    args.clear();
    args << "-o" << "test_result,txt";
    QTest::newRow("test_result") << "test_result"  << args;
    args.clear();
    args << "-o" << "testresult1,txt";
    QTest::newRow("testresult1") << "testresult1"  << args;
    args.clear();
    args << "-o" << "test-result,txt";
    QTest::newRow("test-result") << "test-result"  << args;
    args.clear();
    args << "-o" << "\"testresult?\",txt";
    QTest::newRow("testresult?") << "testresult?"  << args;
    args.clear();
    args << "-o" << "\"testresult*\",txt";
    QTest::newRow("testresult*") << "testresult*"  << args;
    args.clear();
    args << "-o" << "testResult.log,txt";
    QTest::newRow("testResult.log") << "testResult.log"  << args;
    args.clear();
    args << "-o" << "\"test result.log\",txt";
    QTest::newRow("test result.log") << "test result.log"  << args;
    args.clear();
    args << "-o" << "\"\\\"testResult\\\"\",txt";
    QTest::newRow("\"testResult\"") << "\"testResult\""  << args;
    args.clear();
    args << "-o" << "\"\\\"testResult.log\\\"\",txt";
    QTest::newRow("\"testResult.log\"") << "\"testResult.log\""  << args;
    args.clear();
    args << "-o" << "\"\\\"test result.log\\\"\",txt";
    QTest::newRow("\"test result.log\"") << "\"test result.log\""  << args;
}

void QTestLibArgsParserTest::toStringOutFile(void)
{
    QFETCH(QString, filename);
    QFETCH(QStringList, args);

    QTestLibArgsParser parser;
    parser.setOutFileName(Utils::FileName::fromString(filename));

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkArguments(parser.toStringList(), args));
    QCOMPARE(parser.toString(), parser.toStringList().join(' '));
}

void QTestLibArgsParserTest::toStringVerbosity_data(void)
{
    QTest::addColumn<QTestLibArgsParser::TestVerbosity>("verbosity");
    QTest::addColumn<QStringList>("args");

    QStringList args;

    args.clear();
    QTest::newRow("normal") << QTestLibArgsParser::NormalVerbosity << args;
    args.clear();
    args << "-silent";
    QTest::newRow("silent") << QTestLibArgsParser::Silent << args;
    args.clear();
    args << "-v1";
    QTest::newRow("v1") << QTestLibArgsParser::Verbose1 << args;
    args.clear();
    args << "-v2";
    QTest::newRow("v2") << QTestLibArgsParser::Verbose2 << args;
    args.clear();
    args << "-vs";
    QTest::newRow("vs") << QTestLibArgsParser::VerboseSignal << args;
    args.clear();
    args << "-vb";
    QTest::newRow("vb") << QTestLibArgsParser::VerboseBenchmark << args;
}

void QTestLibArgsParserTest::toStringVerbosity(void)
{
    QFETCH(QTestLibArgsParser::TestVerbosity, verbosity);
    QFETCH(QStringList, args);

    QTestLibArgsParser parser;
    parser.setVerbosity(verbosity);

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkArguments(parser.toStringList(), args));
    QCOMPARE(parser.toString(), parser.toStringList().join(' '));
}

void QTestLibArgsParserTest::toStringOther_data(void)
{
    QTest::addColumn<unsigned int>("warnings");
    QTest::addColumn<int>("eventdelay");
    QTest::addColumn<int>("keydelay");
    QTest::addColumn<int>("mousedelay");
    QTest::addColumn<bool>("crashhandler");
    QTest::addColumn<QStringList>("args");

    QStringList args;
    unsigned int rand;

    rand = 10000*(qrand() % 100);
    args.clear();
    args << "-maxwarnings" << QString::number(rand, 10);
    QTest::newRow("maxwarnings") << rand << -1 << -1 << -1 << true << args;

    rand = 100*(1 + qrand() % 100);
    args.clear();
    args << "-eventdelay" << QString::number(rand, 10);
    QTest::newRow("eventdelay") << 2000u << (int) rand << -1 << -1 << true << args;

    rand = 100*(1 + qrand() % 100);
    args.clear();
    args << "-keydelay" << QString::number(rand, 10);
    QTest::newRow("keydelay") << 2000u << -1 << (int) rand << -1 << true << args;

    rand = 100*(1 + qrand() % 100);
    args.clear();
    args << "-mousedelay" << QString::number(rand, 10);
    QTest::newRow("mousedelay") << 2000u << -1 << -1 << (int) rand << true << args;

    args.clear();
    args << "-nocrashhandler";
    QTest::newRow("crashhandler") << 2000u << -1 << -1 << -1 << false << args;
}

void QTestLibArgsParserTest::toStringOther(void)
{
    QFETCH(unsigned int, warnings);
    QFETCH(int, eventdelay);
    QFETCH(int, keydelay);
    QFETCH(int, mousedelay);
    QFETCH(bool, crashhandler);
    QFETCH(QStringList, args);

    QTestLibArgsParser parser;
    if (warnings != 2000)
        parser.setMaxWarnings(warnings);
    if (eventdelay >= 0)
        parser.setEventDelay(eventdelay);
    if (keydelay >= 0)
        parser.setKeyDelay(keydelay);
    if (mousedelay >= 0)
        parser.setMouseDelay(mousedelay);
    if (!crashhandler)
        parser.enableCrashHandler(false);

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkArguments(parser.toStringList(), args));
    QCOMPARE(parser.toString(), parser.toStringList().join(' '));
}

void QTestLibArgsParserTest::toStringMixed_row(QTestLibArgsParser::TestOutputFormat format, QTestLibArgsParser::TestVerbosity verbosity, int version)
{
    QStringList args;
    unsigned int maxWarnings;
    int eventDelay;
    int keyDelay;
    int mouseDelay;
    bool crashHandler;

    args.clear();

    switch(format) {
    case QTestLibArgsParser::TxtFormat:
        break;
    case QTestLibArgsParser::CsvFormat:
        if (version == 1)
            args << "-csv";
        else
            args << "-o" << "-,csv";
        break;
    case QTestLibArgsParser::XmlFormat:
        if (version == 1)
            args << "-xml";
        else
            args << "-o" << "-,xml";
        break;
    case QTestLibArgsParser::LightXmlFormat:
        if (version == 1)
            args << "-lightxml";
        else
            args << "-o" << "-,lightxml";
        break;
    case QTestLibArgsParser::XUnitXmlFormat:
        if (version == 1)
            args << "-xunitxml";
        else
            args << "-o" << "-,xunitxml";
        break;
    default:
        qWarning() << "Unknown format. Assumed txt.";
        break;
    }

    switch (verbosity) {
    case QTestLibArgsParser::NormalVerbosity:
        break;
    case QTestLibArgsParser::Silent:
        args << "-silent";
        break;
    case QTestLibArgsParser::Verbose1:
        args << "-v1";
        break;
    case QTestLibArgsParser::Verbose2:
        args << "-v2";
        break;
    case QTestLibArgsParser::VerboseSignal:
        args << "-vs";
        break;
    case QTestLibArgsParser::VerboseBenchmark:
        args << "-vb";
        break;
    default:
        qWarning() << "Unknown verbosity. Assumed normal.";
        break;
    }

    QString name = args.join(' ');

    maxWarnings = (qrand() % 2 == 1) ? 1000*(qrand() % 1000) : 2000;
    eventDelay = (qrand() % 2 == 1) ? 100*(1 + qrand() % 100) : -1;
    keyDelay = (qrand() % 2 == 1) ? 100*(1 + qrand() % 100) : -1;
    mouseDelay = (qrand() % 2 == 1) ? 100*(1 + qrand() % 100) : -1;
    crashHandler = (qrand() % 2 == 1);

    if (maxWarnings != 2000)
        args << "-maxwarnings" << QString::number(maxWarnings, 10);
    if (eventDelay > 0)
        args << "-eventdelay" << QString::number(eventDelay, 10);
    if (keyDelay > 0)
        args << "-keydelay" << QString::number(keyDelay, 10);
    if (mouseDelay > 0)
        args << "-mousedelay" << QString::number(mouseDelay, 10);
    if (!crashHandler)
        args << "-nocrashhandler";

    QTest::newRow(qPrintable(name)) << format << verbosity << maxWarnings << eventDelay << keyDelay << mouseDelay << crashHandler << args;
}

void QTestLibArgsParserTest::toStringMixedV1_data(void)
{
    QTest::addColumn<QTestLibArgsParser::TestOutputFormat>("format");
    QTest::addColumn<QTestLibArgsParser::TestVerbosity>("verbosity");
    QTest::addColumn<unsigned int>("maxWarnings");
    QTest::addColumn<int>("eventDelay");
    QTest::addColumn<int>("keyDelay");
    QTest::addColumn<int>("mouseDelay");
    QTest::addColumn<bool>("crashHandler");
    QTest::addColumn<QStringList>("args");

    for (int f = (int) QTestLibArgsParser::TxtFormat; f <= (int) QTestLibArgsParser::LightXmlFormat; f++)
        for (int v = (int) QTestLibArgsParser::Silent; v <= (int) QTestLibArgsParser::VerboseBenchmark; v++)
            toStringMixed_row((QTestLibArgsParser::TestOutputFormat) f, (QTestLibArgsParser::TestVerbosity)v, 1);
}

void QTestLibArgsParserTest::toStringMixedV1(void)
{
    QFETCH(QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(QTestLibArgsParser::TestVerbosity, verbosity);
    QFETCH(unsigned int, maxWarnings);
    QFETCH(int, eventDelay);
    QFETCH(int, keyDelay);
    QFETCH(int, mouseDelay);
    QFETCH(bool, crashHandler);
    QFETCH(QStringList, args);

    QTestLibArgsParser parser;
    parser.setOutputFormat(format);
    parser.setVerbosity(verbosity);
    if (maxWarnings != 2000)
        parser.setMaxWarnings(maxWarnings);
    if (eventDelay >= 0)
        parser.setEventDelay(eventDelay);
    if (keyDelay >= 0)
        parser.setKeyDelay(keyDelay);
    if (mouseDelay >= 0)
        parser.setMouseDelay(mouseDelay);
    if (!crashHandler)
        parser.enableCrashHandler(false);

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkArguments(parser.toStringList(1), args));
    QCOMPARE(parser.toString(), parser.toStringList(1).join(' '));
}

void QTestLibArgsParserTest::copyToStringMixedV1(void)
{
    QFETCH(QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(QTestLibArgsParser::TestVerbosity, verbosity);
    QFETCH(unsigned int, maxWarnings);
    QFETCH(int, eventDelay);
    QFETCH(int, keyDelay);
    QFETCH(int, mouseDelay);
    QFETCH(bool, crashHandler);
    QFETCH(QStringList, args);

    QTestLibArgsParser parser;
    parser.setOutputFormat(format);
    parser.setVerbosity(verbosity);
    if (maxWarnings != 2000)
        parser.setMaxWarnings(maxWarnings);
    if (eventDelay >= 0)
        parser.setEventDelay(eventDelay);
    if (keyDelay >= 0)
        parser.setKeyDelay(keyDelay);
    if (mouseDelay >= 0)
        parser.setMouseDelay(mouseDelay);
    if (!crashHandler)
        parser.enableCrashHandler(false);

    QTestLibArgsParser copy(parser);

    SUB_TEST_FUNCTION(checkError(copy));
    SUB_TEST_FUNCTION(checkArguments(copy.toStringList(1), args));
    QCOMPARE(copy.toString(), copy.toStringList(1).join(' '));
}

void QTestLibArgsParserTest::toStringMixedV2_data(void)
{
    QTest::addColumn<QTestLibArgsParser::TestOutputFormat>("format");
    QTest::addColumn<QTestLibArgsParser::TestVerbosity>("verbosity");
    QTest::addColumn<unsigned int>("maxWarnings");
    QTest::addColumn<int>("eventDelay");
    QTest::addColumn<int>("keyDelay");
    QTest::addColumn<int>("mouseDelay");
    QTest::addColumn<bool>("crashHandler");
    QTest::addColumn<QStringList>("args");

    for (int f = (int) QTestLibArgsParser::TxtFormat; f <= (int) QTestLibArgsParser::LightXmlFormat; f++)
        for (int v = (int) QTestLibArgsParser::Silent; v <= (int) QTestLibArgsParser::VerboseBenchmark; v++)
            toStringMixed_row((QTestLibArgsParser::TestOutputFormat) f, (QTestLibArgsParser::TestVerbosity) v, 2);
}

void QTestLibArgsParserTest::toStringMixedV2(void)
{
    QFETCH(QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(QTestLibArgsParser::TestVerbosity, verbosity);
    QFETCH(unsigned int, maxWarnings);
    QFETCH(int, eventDelay);
    QFETCH(int, keyDelay);
    QFETCH(int, mouseDelay);
    QFETCH(bool, crashHandler);
    QFETCH(QStringList, args);

    QTestLibArgsParser parser;
    parser.setOutputFormat(format);
    parser.setVerbosity(verbosity);
    if (maxWarnings != 2000)
        parser.setMaxWarnings(maxWarnings);
    if (eventDelay >= 0)
        parser.setEventDelay(eventDelay);
    if (keyDelay >= 0)
        parser.setKeyDelay(keyDelay);
    if (mouseDelay >= 0)
        parser.setMouseDelay(mouseDelay);
    if (!crashHandler)
        parser.enableCrashHandler(false);

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkArguments(parser.toStringList(2), args));
    QCOMPARE(parser.toString(), parser.toStringList(2).join(' '));
}

void QTestLibArgsParserTest::copyToStringMixedV2(void)
{
    QFETCH(QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(QTestLibArgsParser::TestVerbosity, verbosity);
    QFETCH(unsigned int, maxWarnings);
    QFETCH(int, eventDelay);
    QFETCH(int, keyDelay);
    QFETCH(int, mouseDelay);
    QFETCH(bool, crashHandler);
    QFETCH(QStringList, args);

    QTestLibArgsParser parser;
    parser.setOutputFormat(format);
    parser.setVerbosity(verbosity);
    if (maxWarnings != 2000)
        parser.setMaxWarnings(maxWarnings);
    if (eventDelay >= 0)
        parser.setEventDelay(eventDelay);
    if (keyDelay >= 0)
        parser.setKeyDelay(keyDelay);
    if (mouseDelay >= 0)
        parser.setMouseDelay(mouseDelay);
    if (!crashHandler)
        parser.enableCrashHandler(false);

    QTestLibArgsParser copy(parser);

    SUB_TEST_FUNCTION(checkError(copy));
    SUB_TEST_FUNCTION(checkArguments(copy.toStringList(2), args));
    QCOMPARE(copy.toString(), copy.toStringList(2).join(' '));
}


void QTestLibArgsParserTest::toStringMixed_data(void)
{
    QTest::addColumn<QTestLibArgsParser::TestOutputFormat>("format");
    QTest::addColumn<QTestLibArgsParser::TestVerbosity>("verbosity");
    QTest::addColumn<unsigned int>("maxWarnings");
    QTest::addColumn<int>("eventDelay");
    QTest::addColumn<int>("keyDelay");
    QTest::addColumn<int>("mouseDelay");
    QTest::addColumn<bool>("crashHandler");
    QTest::addColumn<QStringList>("args");

    for (int f = (int) QTestLibArgsParser::TxtFormat; f <= (int) QTestLibArgsParser::LightXmlFormat; f++)
        for (int v = (int) QTestLibArgsParser::Silent; v <= (int) QTestLibArgsParser::VerboseBenchmark; v++)
            toStringMixed_row((QTestLibArgsParser::TestOutputFormat) f, (QTestLibArgsParser::TestVerbosity)v);
}

void QTestLibArgsParserTest::toStringMixed(void)
{
    QFETCH(QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(QTestLibArgsParser::TestVerbosity, verbosity);
    QFETCH(unsigned int, maxWarnings);
    QFETCH(int, eventDelay);
    QFETCH(int, keyDelay);
    QFETCH(int, mouseDelay);
    QFETCH(bool, crashHandler);
    QFETCH(QStringList, args);

    QTestLibArgsParser parser;
    parser.setOutputFormat(format);
    parser.setVerbosity(verbosity);
    if (maxWarnings != 2000)
        parser.setMaxWarnings(maxWarnings);
    if (eventDelay >= 0)
        parser.setEventDelay(eventDelay);
    if (keyDelay >= 0)
        parser.setKeyDelay(keyDelay);
    if (mouseDelay >= 0)
        parser.setMouseDelay(mouseDelay);
    if (!crashHandler)
        parser.enableCrashHandler(false);

    SUB_TEST_FUNCTION(checkError(parser));
    SUB_TEST_FUNCTION(checkArguments(parser.toStringList(), args));
    QCOMPARE(parser.toString(), parser.toStringList().join(' '));
}

void QTestLibArgsParserTest::copyToStringMixed(void)
{
    QFETCH(QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(QTestLibArgsParser::TestVerbosity, verbosity);
    QFETCH(unsigned int, maxWarnings);
    QFETCH(int, eventDelay);
    QFETCH(int, keyDelay);
    QFETCH(int, mouseDelay);
    QFETCH(bool, crashHandler);
    QFETCH(QStringList, args);

    QTestLibArgsParser parser;
    parser.setOutputFormat(format);
    parser.setVerbosity(verbosity);
    if (maxWarnings != 2000)
        parser.setMaxWarnings(maxWarnings);
    if (eventDelay >= 0)
        parser.setEventDelay(eventDelay);
    if (keyDelay >= 0)
        parser.setKeyDelay(keyDelay);
    if (mouseDelay >= 0)
        parser.setMouseDelay(mouseDelay);
    if (!crashHandler)
        parser.enableCrashHandler(false);

    QTestLibArgsParser copy(parser);

    SUB_TEST_FUNCTION(checkError(copy));
    SUB_TEST_FUNCTION(checkArguments(copy.toStringList(), args));
    QCOMPARE(copy.toString(), copy.toStringList().join(' '));
}

void QTestLibArgsParserTest::checkArguments(const QStringList& args, const QStringList& expected)
{
    QCOMPARE(args.size(), expected.size());

    for (QStringList::const_iterator argsIt = args.constBegin(); argsIt != args.constEnd(); argsIt++) {
        QStringList::const_iterator expectedIt;
        for (expectedIt = expected.constBegin(); expectedIt != expected.constEnd(); expectedIt++) {
            if (QString::compare(*argsIt, *expectedIt, Qt::CaseSensitive) == 0)
                break;
        }
        QVERIFY(expectedIt != expected.constEnd());
    }
}

void QTestLibArgsParserTest::mapFormat_data(void)
{
    QTest::addColumn<QTestLibArgsParser::TestOutputFormat>("format");
    QTest::addColumn<QVariantMap>("map");
    QTest::addColumn<QStringList>("args");

    QVariantMap map;
    QStringList args;

    map.clear();
    args.clear();
    QTest::newRow("txt") << QTestLibArgsParser::TxtFormat << map << args;
    map.clear();
    args.clear();
    map.insert(QTestLibPlugin::Constants::FormatKey, (int) QTestLibArgsParser::CsvFormat);
    args << "-o" << "-,csv";
    QTest::newRow("csv") << QTestLibArgsParser::CsvFormat << map << args;
    map.clear();
    args.clear();
    map.insert(QTestLibPlugin::Constants::FormatKey, (int) QTestLibArgsParser::XmlFormat);
    args << "-o" << "-,xml";
    QTest::newRow("xml") << QTestLibArgsParser::XmlFormat << map << args;
    map.clear();
    args.clear();
    map.insert(QTestLibPlugin::Constants::FormatKey, (int) QTestLibArgsParser::LightXmlFormat);
    args << "-o" << "-,lightxml";
    QTest::newRow("lightxml") << QTestLibArgsParser::LightXmlFormat << map << args;
    map.clear();
    args.clear();
    map.insert(QTestLibPlugin::Constants::FormatKey, (int) QTestLibArgsParser::XUnitXmlFormat);
    args << "-o" << "-,xunitxml";
    QTest::newRow("xunitxml") << QTestLibArgsParser::XUnitXmlFormat << map << args;
}

void QTestLibArgsParserTest::mapFormat(void)
{
    QFETCH(QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(QVariantMap, map);
    QFETCH(QStringList, args);

    QTestLibArgsParser parserTo;
    parserTo.setOutputFormat(format);

    QVariantMap parserToMap;
    parserTo.toMap(parserToMap);
    QCOMPARE(parserToMap, map);

    QTestLibArgsParser parserFrom;
    parserFrom.fromMap(map);

    SUB_TEST_FUNCTION(checkArguments(parserFrom.toStringList(), args));
}

void QTestLibArgsParserTest::mapVerbosity_data(void)
{
    QTest::addColumn<QTestLibArgsParser::TestVerbosity>("verbosity");
    QTest::addColumn<QVariantMap>("map");
    QTest::addColumn<QStringList>("args");

    QVariantMap map;
    QStringList args;

    map.clear();
    args.clear();
    QTest::newRow("normal") << QTestLibArgsParser::NormalVerbosity << map << args;
    map.clear();
    args.clear();
    map.insert(QTestLibPlugin::Constants::VerbosityKey, (int) QTestLibArgsParser::Silent);
    args << "-silent";
    QTest::newRow("silent") << QTestLibArgsParser::Silent << map << args;
    map.clear();
    args.clear();
    map.insert(QTestLibPlugin::Constants::VerbosityKey, (int) QTestLibArgsParser::Verbose1);
    args << "-v1";
    QTest::newRow("v1") << QTestLibArgsParser::Verbose1 << map << args;
    map.clear();
    args.clear();
    map.insert(QTestLibPlugin::Constants::VerbosityKey, (int) QTestLibArgsParser::Verbose2);
    args << "-v2";
    QTest::newRow("v2") << QTestLibArgsParser::Verbose2 << map << args;
    map.clear();
    args.clear();
    map.insert(QTestLibPlugin::Constants::VerbosityKey, (int) QTestLibArgsParser::VerboseSignal);
    args << "-vs";
    QTest::newRow("vs") << QTestLibArgsParser::VerboseSignal << map << args;
    map.clear();
    args.clear();
    map.insert(QTestLibPlugin::Constants::VerbosityKey, (int) QTestLibArgsParser::VerboseBenchmark);
    args << "-vb";
    QTest::newRow("vb") << QTestLibArgsParser::VerboseBenchmark << map << args;
}

void QTestLibArgsParserTest::mapVerbosity(void)
{
    QFETCH(QTestLibArgsParser::TestVerbosity, verbosity);
    QFETCH(QVariantMap, map);
    QFETCH(QStringList, args);

    QTestLibArgsParser parserTo;
    parserTo.setVerbosity(verbosity);

    QVariantMap parserToMap;
    parserTo.toMap(parserToMap);
    QCOMPARE(parserToMap, map);

    QTestLibArgsParser parserFrom;
    parserFrom.fromMap(map);

    SUB_TEST_FUNCTION(checkArguments(parserFrom.toStringList(), args));
}

void QTestLibArgsParserTest::mapOutFile(void)
{
    QString fileName = "testouput.log";
    QVariantMap map;
    map.insert(QTestLibPlugin::Constants::OutputFileKey, fileName);
    QStringList args;
    args << "-o" << fileName + ",txt";

    QTestLibArgsParser parserTo;
    parserTo.setOutFileName(Utils::FileName::fromString(fileName));

    QVariantMap parserToMap;
    parserTo.toMap(parserToMap);
    QCOMPARE(parserToMap, map);

    QTestLibArgsParser parserFrom;
    parserFrom.fromMap(map);

    SUB_TEST_FUNCTION(checkArguments(parserFrom.toStringList(), args));
}

void QTestLibArgsParserTest::mapOther_data(void)
{
    QTest::addColumn<unsigned int>("warnings");
    QTest::addColumn<int>("eventDelay");
    QTest::addColumn<int>("keyDelay");
    QTest::addColumn<int>("mouseDelay");
    QTest::addColumn<bool>("crashHandler");
    QTest::addColumn<QVariantMap>("map");
    QTest::addColumn<QStringList>("args");

    QVariantMap map;
    QStringList args;
    unsigned int rand;

    rand = 10000*(qrand() % 100);
    map.clear();
    args.clear();
    map.insert(QTestLibPlugin::Constants::MaxWarningKey, rand);
    args << "-maxwarnings" << QString::number(rand, 10);
    QTest::newRow("maxwarnings") << rand << -1 << -1 << -1 << true << map << args;

    rand = 100*(1 + qrand() % 100);
    map.clear();
    args.clear();
    map.insert(QTestLibPlugin::Constants::EventDelayKey, (int) rand);
    args << "-eventdelay" << QString::number(rand, 10);
    QTest::newRow("eventdelay") << 2000u << (int) rand << -1 << -1 << true << map << args;

    rand = 100*(1 + qrand() % 100);
    map.clear();
    args.clear();
    map.insert(QTestLibPlugin::Constants::KeyDelayKey, (int) rand);
    args << "-keydelay" << QString::number(rand, 10);
    QTest::newRow("keydelay") << 2000u << -1 << (int) rand << -1 << true << map << args;

    rand = 100*(1 + qrand() % 100);
    map.clear();
    args.clear();
    map.insert(QTestLibPlugin::Constants::MouseDelayKey, (int) rand);
    args << "-mousedelay" << QString::number(rand, 10);
    QTest::newRow("mousedelay") << 2000u << -1 << -1 << (int) rand << true << map << args;

    args.clear();
    map.clear();
    map.insert(QTestLibPlugin::Constants::CrashHandlerEnabledKey, false);
    args << "-nocrashhandler";
    QTest::newRow("crashhandler") << 2000u << -1 << -1 << -1 << false << map << args;
}

void QTestLibArgsParserTest::mapOther(void)
{
    QFETCH(unsigned int, warnings);
    QFETCH(int, eventDelay);
    QFETCH(int, keyDelay);
    QFETCH(int, mouseDelay);
    QFETCH(bool, crashHandler);
    QFETCH(QVariantMap, map);
    QFETCH(QStringList, args);

    QTestLibArgsParser parserTo;
    if (warnings != 2000)
        parserTo.setMaxWarnings(warnings);
    if (eventDelay >= 0)
        parserTo.setEventDelay(eventDelay);
    if (keyDelay >= 0)
        parserTo.setKeyDelay(keyDelay);
    if (mouseDelay >= 0)
        parserTo.setMouseDelay(mouseDelay);
    if (!crashHandler)
        parserTo.enableCrashHandler(false);

    QVariantMap parserToMap;
    parserTo.toMap(parserToMap);
    QCOMPARE(parserToMap, map);

    QTestLibArgsParser parserFrom;
    parserFrom.fromMap(map);

    SUB_TEST_FUNCTION(checkArguments(parserFrom.toStringList(), args));
}

void QTestLibArgsParserTest::mapMixed_row(QTestLibArgsParser::TestOutputFormat format, QTestLibArgsParser::TestVerbosity verbosity)
{
    QVariantMap map;
    QStringList args;

    unsigned int maxWarnings;
    int eventDelay;
    int keyDelay;
    int mouseDelay;
    bool crashHandler;

    args.clear();
    map.clear();

    if (format != QTestLibArgsParser::TxtFormat)
        map.insert(QTestLibPlugin::Constants::FormatKey, (int) format);
    switch(format) {
    case QTestLibArgsParser::TxtFormat:
        break;
    case QTestLibArgsParser::CsvFormat:
        args << "-o" << "-,csv";
        break;
    case QTestLibArgsParser::XmlFormat:
        args << "-o" << "-,xml";
        break;
    case QTestLibArgsParser::LightXmlFormat:
        args << "-o" << "-,lightxml";
        break;
    case QTestLibArgsParser::XUnitXmlFormat:
        args << "-o" << "-,xunitxml";
        break;
    default:
        qWarning() << "Unknown format. Assumed txt.";
        break;
    }

    if (verbosity != QTestLibArgsParser::NormalVerbosity)
        map.insert(QTestLibPlugin::Constants::VerbosityKey, (int) verbosity);
    switch (verbosity) {
    case QTestLibArgsParser::NormalVerbosity:
        break;
    case QTestLibArgsParser::Silent:
        args << "-silent";
        break;
    case QTestLibArgsParser::Verbose1:
        args << "-v1";
        break;
    case QTestLibArgsParser::Verbose2:
        args << "-v2";
        break;
    case QTestLibArgsParser::VerboseSignal:
        args << "-vs";
        break;
    case QTestLibArgsParser::VerboseBenchmark:
        args << "-vb";
        break;
    default:
        qWarning() << "Unknown verbosity. Assumed normal.";
        break;
    }

    QString name = args.join(' ');

    maxWarnings = (qrand() % 2 == 1) ? 1000*(qrand() % 1000) : 2000;
    eventDelay = (qrand() % 2 == 1) ? 100*(1 + qrand() % 100) : -1;
    keyDelay = (qrand() % 2 == 1) ? 100*(1 + qrand() % 100) : -1;
    mouseDelay = (qrand() % 2 == 1) ? 100*(1 + qrand() % 100) : -1;
    crashHandler = (qrand() % 2 == 1);

    if (maxWarnings != 2000) {
        map.insert(QTestLibPlugin::Constants::MaxWarningKey, maxWarnings);
        args << "-maxwarnings" << QString::number(maxWarnings, 10);
    }
    if (eventDelay > 0) {
        map.insert(QTestLibPlugin::Constants::EventDelayKey, eventDelay);
        args << "-eventdelay" << QString::number(eventDelay, 10);
    }
    if (keyDelay > 0) {
        map.insert(QTestLibPlugin::Constants::KeyDelayKey, keyDelay);
        args << "-keydelay" << QString::number(keyDelay, 10);
    }
    if (mouseDelay > 0) {
        map.insert(QTestLibPlugin::Constants::MouseDelayKey, mouseDelay);
        args << "-mousedelay" << QString::number(mouseDelay, 10);
    }
    if (!crashHandler) {
        map.insert(QTestLibPlugin::Constants::CrashHandlerEnabledKey, false);
        args << "-nocrashhandler";
    }

    QTest::newRow(qPrintable(name)) << format << verbosity << maxWarnings << eventDelay << keyDelay << mouseDelay << crashHandler << map << args;
}

void QTestLibArgsParserTest::mapMixed_data(void)
{
    QTest::addColumn<QTestLibArgsParser::TestOutputFormat>("format");
    QTest::addColumn<QTestLibArgsParser::TestVerbosity>("verbosity");
    QTest::addColumn<unsigned int>("maxWarnings");
    QTest::addColumn<int>("eventDelay");
    QTest::addColumn<int>("keyDelay");
    QTest::addColumn<int>("mouseDelay");
    QTest::addColumn<bool>("crashHandler");
    QTest::addColumn<QVariantMap>("map");
    QTest::addColumn<QStringList>("args");

    for (int f = (int) QTestLibArgsParser::TxtFormat; f <= (int) QTestLibArgsParser::LightXmlFormat; f++)
        for (int v = (int) QTestLibArgsParser::Silent; v <= (int) QTestLibArgsParser::VerboseBenchmark; v++)
            mapMixed_row((QTestLibArgsParser::TestOutputFormat) f, (QTestLibArgsParser::TestVerbosity) v);
}

void QTestLibArgsParserTest::mapMixed(void)
{
    QFETCH(QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(QTestLibArgsParser::TestVerbosity, verbosity);
    QFETCH(unsigned int, maxWarnings);
    QFETCH(int, eventDelay);
    QFETCH(int, keyDelay);
    QFETCH(int, mouseDelay);
    QFETCH(bool, crashHandler);
    QFETCH(QVariantMap, map);
    QFETCH(QStringList, args);

    QTestLibArgsParser parserTo;
    parserTo.setOutputFormat(format);
    parserTo.setVerbosity(verbosity);
    if (maxWarnings != 2000)
        parserTo.setMaxWarnings(maxWarnings);
    if (eventDelay >= 0)
        parserTo.setEventDelay(eventDelay);
    if (keyDelay >= 0)
        parserTo.setKeyDelay(keyDelay);
    if (mouseDelay >= 0)
        parserTo.setMouseDelay(mouseDelay);
    if (!crashHandler)
        parserTo.enableCrashHandler(false);

    QVariantMap parserToMap;
    parserTo.toMap(parserToMap);
    QCOMPARE(parserToMap, map);

    QTestLibArgsParser parserFrom;
    parserFrom.fromMap(map);

    SUB_TEST_FUNCTION(checkArguments(parserFrom.toStringList(), args));
}

void QTestLibArgsParserTest::copyMapMixed(void)
{
    QFETCH(QTestLibArgsParser::TestOutputFormat, format);
    QFETCH(QTestLibArgsParser::TestVerbosity, verbosity);
    QFETCH(unsigned int, maxWarnings);
    QFETCH(int, eventDelay);
    QFETCH(int, keyDelay);
    QFETCH(int, mouseDelay);
    QFETCH(bool, crashHandler);
    QFETCH(QVariantMap, map);
    QFETCH(QStringList, args);

    QTestLibArgsParser parserTo;
    parserTo.setOutputFormat(format);
    parserTo.setVerbosity(verbosity);
    if (maxWarnings != 2000)
        parserTo.setMaxWarnings(maxWarnings);
    if (eventDelay >= 0)
        parserTo.setEventDelay(eventDelay);
    if (keyDelay >= 0)
        parserTo.setKeyDelay(keyDelay);
    if (mouseDelay >= 0)
        parserTo.setMouseDelay(mouseDelay);
    if (!crashHandler)
        parserTo.enableCrashHandler(false);

    QTestLibArgsParser copyTo(parserTo);

    QVariantMap copyToMap;
    copyTo.toMap(copyToMap);
    QCOMPARE(copyToMap, map);
}

QTEST_APPLESS_MAIN(QTestLibArgsParserTest)

#include "qtestlibargsparsertest.moc"
