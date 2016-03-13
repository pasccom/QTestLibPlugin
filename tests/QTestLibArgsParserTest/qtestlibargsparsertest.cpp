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

#include <qtestlibargsparser.h>
using namespace QTestLibPlugin::Internal;

#include "../common/qttestsubfunction.h"

#include <QtTest>

class QTestLibArgsParserTest : public QObject
{
    Q_OBJECT
    HAS_SUB_TEST_FUNCTIONS
public:
    inline QTestLibArgsParserTest(void) {}
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

    void flagError_data(void);
    void flagError(void);
    void prematureEndError_data(void);
    void prematureEndError(void);


private:
    void checkError(const QTestLibArgsParser& parser, QTestLibArgsParser::Error error = QTestLibArgsParser::NoError, const QString& errorString = QString::null);
    void checkOutput(const QTestLibArgsParser& parser, QTestLibArgsParser::TestVerbosity verb, QTestLibArgsParser::TestOutputFormat format, const QString& filename = QString::null);
    void checkDelays(const QTestLibArgsParser& parser, int event = -1, int key = -1, int mouse = -1);
    void checkOutputMode(const QTestLibArgsParser& parser, QTestLibArgsParser::Output mode = QTestLibArgsParser::NormalOutput);
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
                QTest::newRow(qPrintable((*v).first + " " + (*f).first + " " + (*n).first)) << (*v).first + " " + (*f).first + " " + (*n).first << (*n).second << (*f).second << (*v).second;
                QTest::newRow(qPrintable((*v).first + " " + (*n).first + " " + (*f).first)) << (*v).first + " " + (*n).first + " " + (*f).first << (*n).second << (*f).second << (*v).second;
                QTest::newRow(qPrintable((*f).first + " " + (*v).first + " " + (*n).first)) << (*f).first + " " + (*v).first + " " + (*n).first << (*n).second << (*f).second << (*v).second;
                QTest::newRow(qPrintable((*f).first + " " + (*n).first + " " + (*v).first)) << (*f).first + " " + (*n).first + " " + (*v).first << (*n).second << (*f).second << (*v).second;
                QTest::newRow(qPrintable((*n).first + " " + (*v).first + " " + (*f).first)) << (*n).first + " " + (*v).first + " " + (*f).first << (*n).second << (*f).second << (*v).second;
                QTest::newRow(qPrintable((*n).first + " " + (*f).first + " " + (*v).first)) << (*n).first + " " + (*f).first + " " + (*v).first << (*n).second << (*f).second << (*v).second;
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
                QTest::newRow(qPrintable((*v).first + " " + (*n).first + "," + (*f).first)) << (*v).first + " " + (*n).first + "," + (*f).first << (*n).second << (*f).second << (*v).second;
                QTest::newRow(qPrintable((*n).first + "," + (*f).first + " " + (*v).first)) << (*n).first + "," + (*f).first + " " + (*v).first << (*n).second << (*f).second << (*v).second;
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

    QVERIFY(parser.error() == err);
    QVERIFY(QString::compare(parser.errorString(), errStr, Qt::CaseSensitive) == 0);
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

    QVERIFY(parser.error() == err);
    QVERIFY(QString::compare(parser.errorString(), errStr, Qt::CaseSensitive) == 0);
}

QTEST_APPLESS_MAIN(QTestLibArgsParserTest)

#include "qtestlibargsparsertest.moc"
