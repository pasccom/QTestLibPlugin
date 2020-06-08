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

#include <testsuitemodel.h>

#include <plaintextqtestlibparserfactory.h>
#include <xmlqtestlibparserfactory.h>
#include <lightxmlqtestlibparserfactory.h>
#include <xunitxmlqtestlibparserfactory.h>

#include "../common/qtestlibmodeltester.h"

#include <utils/outputformat.h>
#include <extensionsystem/pluginmanager.h>
#include <projectexplorer/localapplicationrunconfiguration.h>
#include <projectexplorer/localapplicationruncontrol.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/target.h>
#include <qmakeprojectmanager/qmakeprojectmanager.h>
#include <qmakeprojectmanager/qmakeproject.h>

#include <QtTest>

class TestSuiteModelTest : public QObject
{
    Q_OBJECT
    HAS_SUB_TEST_FUNCTIONS
public:
    TestSuiteModelTest(void);
    inline ~TestSuiteModelTest(void) {qDeleteAll(mManager->allObjects()); delete mManager;}
private Q_SLOTS:
    void zeroRemoveBad(void);
    void zeroClear(void);

    inline void appendOne_data(void) {data();}
    void appendOne(void);
    inline void appendOneRemoveOne_data(void) {data();}
    void appendOneRemoveOne(void);
    inline void removeBadAppendOne_data(void) {data();}
    void removeBadAppendOne(void);
    inline void appendOneRemoveBad_data(void) {data();}
    void appendOneRemoveBad(void);
    inline void appendOneClear_data(void) {data();}
    void appendOneClear(void);

    inline void appendTwo_data(void) {data();}
    void appendTwo(void);
    inline void appendTwoRemoveFirst_data(void) {data();}
    void appendTwoRemoveFirst(void);
    inline void appendTwoRemoveSecond_data(void) {data();}
    void appendTwoRemoveSecond(void);
    inline void appendTwoRemoveBad_data(void) {data();}
    void appendTwoRemoveBad(void);
    inline void appendOneRemoveAppendOne_data(void) {data();}
    void appendOneRemoveAppendOne(void);
    inline void appendOneRemoveBadAppendOne_data(void) {data();}
    void appendOneRemoveBadAppendOne(void);
    inline void appendTwoClear_data(void) {data();}
    void appendTwoClear(void);
    inline void appendOneClearAppendOne_data(void) {data();}
    void appendOneClearAppendOne(void);
private:
    struct TestRunData {
        QString testName;
        QString parserFormat;
        QTestLibModelTester::Verbosity testVerbosity;

        inline TestRunData(const QString& name, const QString& format, QTestLibModelTester::Verbosity verbosity) :
            testName(name), parserFormat(format), testVerbosity(verbosity) {}
    };

    void data(void);

    void parseSuiteRoot(const QAbstractItemModel* model, const QList< TestRunData* >& tests);

    void checkSignalArguments(const QString& signal, const QList<QVariant>& args, int first, int last);
    void appendTest(QTestLibPlugin::Internal::TestSuiteModel *model, const QString& test, const QString& format, QTestLibModelTester::Verbosity verbosity, Utils::OutputFormat outputFormat, Utils::OutputFormat errorFormat);
    void removeTestAt(QTestLibPlugin::Internal::TestSuiteModel *model, int i);
    void clearTests(QTestLibPlugin::Internal::TestSuiteModel *model);

    QStringList mTests;
    QStringList mParserFormats;

    ExtensionSystem::PluginManager *mManager;
};

Q_DECLARE_METATYPE(Utils::OutputFormat)

TestSuiteModelTest::TestSuiteModelTest(void)
{
    qsrand(QDateTime::currentMSecsSinceEpoch());
    QmakeProjectManager::QmakeProjectManager::initialize();

    mManager = new ExtensionSystem::PluginManager();
    QTestLibPlugin::Internal::AbstractTestParserFactory *plainTextFactory = new QTestLibPlugin::Internal::PlainTextQTestLibParserFactory(this);
    ExtensionSystem::PluginManager::addObject(plainTextFactory);
    QTestLibPlugin::Internal::AbstractTestParserFactory *xmlFactory = new QTestLibPlugin::Internal::XMLQTestLibParserFactory(this);
    ExtensionSystem::PluginManager::addObject(xmlFactory);
    QTestLibPlugin::Internal::AbstractTestParserFactory *lightXmlFactory = new QTestLibPlugin::Internal::LightXMLQTestLibParserFactory(this);
    ExtensionSystem::PluginManager::addObject(lightXmlFactory);
    QTestLibPlugin::Internal::AbstractTestParserFactory *xUnitXmlFactory = new QTestLibPlugin::Internal::XUnitXMLQTestLibParserFactory(this);
    ExtensionSystem::PluginManager::addObject(xUnitXmlFactory);

    mTests.clear();
    mTests << "OneClassTest";
    mTests << "AllMessagesTest";
    mTests << "MultipleClassesTest";
    mTests << "SignalsTest";
    mTests << "LimitsTest";

    mParserFormats.clear();
    mParserFormats << "txt";
    mParserFormats << "xml";
    mParserFormats << "lightxml";
    mParserFormats << "xunitxml";
}

void TestSuiteModelTest::data(void)
{
    QTest::addColumn<Utils::OutputFormat>("outputFormat");
    QTest::addColumn<Utils::OutputFormat>("errorFormat");

    QTest::newRow("Normal") << Utils::OutputFormat::StdOutFormat << Utils::OutputFormat::StdErrFormat;
    QTest::newRow("SameLine") << Utils::OutputFormat::StdOutFormatSameLine << Utils::OutputFormat::StdErrFormatSameLine;
    QTest::newRow("OutSameLine") << Utils::OutputFormat::StdOutFormatSameLine << Utils::OutputFormat::StdErrFormat;
    QTest::newRow("ErrSameLine") << Utils::OutputFormat::StdOutFormat << Utils::OutputFormat::StdErrFormatSameLine;
}

void TestSuiteModelTest::zeroRemoveBad(void)
{
    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< TestRunData* > testRuns;

    // Try to remove non existing test
    SUB_TEST_FUNCTION(removeTestAt(&model, 0));
    //testRuns.removeAt(0);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::zeroClear(void)
{
    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< TestRunData* > testRuns;

    // Clear test suite
    SUB_TEST_FUNCTION(clearTests(&model));
    testRuns.clear();
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::appendOne(void)
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< TestRunData* > testRuns;

    // Append one test
    TestRunData testData(mTests.at(qrand() % mTests.size()), mParserFormats.at(qrand() % mParserFormats.size()), (QTestLibModelTester::Verbosity) ((qrand() % ((int) QTestLibModelTester::VerbosityCountMinusOne + 1)) - 1));
    SUB_TEST_FUNCTION(appendTest(&model, testData.testName, testData.parserFormat, testData.testVerbosity, outputFormat, errorFormat));
    testRuns.append(&testData);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::appendOneRemoveOne()
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< TestRunData* > testRuns;

    // Append one test
    TestRunData testData(mTests.at(qrand() % mTests.size()), mParserFormats.at(qrand() % mParserFormats.size()), (QTestLibModelTester::Verbosity) ((qrand() % ((int) QTestLibModelTester::VerbosityCountMinusOne + 1)) - 1));
    SUB_TEST_FUNCTION(appendTest(&model, testData.testName, testData.parserFormat, testData.testVerbosity, outputFormat, errorFormat));
    testRuns.append(&testData);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Remove the appended test
    SUB_TEST_FUNCTION(removeTestAt(&model, 0));
    testRuns.removeAt(0);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::removeBadAppendOne(void)
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< TestRunData* > testRuns;

    // Try to remove non existing test
    SUB_TEST_FUNCTION(removeTestAt(&model, 0));
    //testRuns.removeAt(0);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Append one test
    TestRunData testData(mTests.at(qrand() % mTests.size()), mParserFormats.at(qrand() % mParserFormats.size()), (QTestLibModelTester::Verbosity) ((qrand() % ((int) QTestLibModelTester::VerbosityCountMinusOne + 1)) - 1));
    SUB_TEST_FUNCTION(appendTest(&model, testData.testName, testData.parserFormat, testData.testVerbosity, outputFormat, errorFormat));
    testRuns.append(&testData);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::appendOneRemoveBad()
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< TestRunData* > testRuns;

    // Append one test
    TestRunData testData(mTests.at(qrand() % mTests.size()), mParserFormats.at(qrand() % mParserFormats.size()), (QTestLibModelTester::Verbosity) ((qrand() % ((int) QTestLibModelTester::VerbosityCountMinusOne + 1)) - 1));
    SUB_TEST_FUNCTION(appendTest(&model, testData.testName, testData.parserFormat, testData.testVerbosity, outputFormat, errorFormat));
    testRuns.append(&testData);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Try to remove non existing test
    SUB_TEST_FUNCTION(removeTestAt(&model, 1));
    //testRuns.removeAt(1);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::appendOneClear()
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< TestRunData* > testRuns;

    // Append one test
    TestRunData testData(mTests.at(qrand() % mTests.size()), mParserFormats.at(qrand() % mParserFormats.size()), (QTestLibModelTester::Verbosity) ((qrand() % ((int) QTestLibModelTester::VerbosityCountMinusOne + 1)) - 1));
    SUB_TEST_FUNCTION(appendTest(&model, testData.testName, testData.parserFormat, testData.testVerbosity, outputFormat, errorFormat));
    testRuns.append(&testData);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Clear test suite
    SUB_TEST_FUNCTION(clearTests(&model));
    testRuns.clear();
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::appendTwo(void)
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< TestRunData* > testRuns;

    // Append one test
    TestRunData testData1(mTests.at(qrand() % mTests.size()), mParserFormats.at(qrand() % mParserFormats.size()), (QTestLibModelTester::Verbosity) ((qrand() % ((int) QTestLibModelTester::VerbosityCountMinusOne + 1)) - 1));
    SUB_TEST_FUNCTION(appendTest(&model, testData1.testName, testData1.parserFormat, testData1.testVerbosity, outputFormat, errorFormat));
    testRuns.append(&testData1);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Append one test
    TestRunData testData2(mTests.at(qrand() % mTests.size()), mParserFormats.at(qrand() % mParserFormats.size()), (QTestLibModelTester::Verbosity) ((qrand() % ((int) QTestLibModelTester::VerbosityCountMinusOne + 1)) - 1));
    SUB_TEST_FUNCTION(appendTest(&model, testData2.testName, testData2.parserFormat, testData2.testVerbosity, outputFormat, errorFormat));
    testRuns.append(&testData2);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::appendTwoRemoveFirst(void)
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< TestRunData* > testRuns;

    // Append one test
    TestRunData testData1(mTests.at(qrand() % mTests.size()), mParserFormats.at(qrand() % mParserFormats.size()), (QTestLibModelTester::Verbosity) ((qrand() % ((int) QTestLibModelTester::VerbosityCountMinusOne + 1)) - 1));
    SUB_TEST_FUNCTION(appendTest(&model, testData1.testName, testData1.parserFormat, testData1.testVerbosity, outputFormat, errorFormat));
    testRuns.append(&testData1);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Append one test
    TestRunData testData2(mTests.at(qrand() % mTests.size()), mParserFormats.at(qrand() % mParserFormats.size()), (QTestLibModelTester::Verbosity) ((qrand() % ((int) QTestLibModelTester::VerbosityCountMinusOne + 1)) - 1));
    SUB_TEST_FUNCTION(appendTest(&model, testData2.testName, testData2.parserFormat, testData2.testVerbosity, outputFormat, errorFormat));
    testRuns.append(&testData2);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Remove first test
    SUB_TEST_FUNCTION(removeTestAt(&model, 0));
    testRuns.removeAt(0);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::appendTwoRemoveSecond(void)
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< TestRunData* > testRuns;

    // Append one test
    TestRunData testData1(mTests.at(qrand() % mTests.size()), mParserFormats.at(qrand() % mParserFormats.size()), (QTestLibModelTester::Verbosity) ((qrand() % ((int) QTestLibModelTester::VerbosityCountMinusOne + 1)) - 1));
    SUB_TEST_FUNCTION(appendTest(&model, testData1.testName, testData1.parserFormat, testData1.testVerbosity, outputFormat, errorFormat));
    testRuns.append(&testData1);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Append one test
    TestRunData testData2(mTests.at(qrand() % mTests.size()), mParserFormats.at(qrand() % mParserFormats.size()), (QTestLibModelTester::Verbosity) ((qrand() % ((int) QTestLibModelTester::VerbosityCountMinusOne + 1)) - 1));
    SUB_TEST_FUNCTION(appendTest(&model, testData2.testName, testData2.parserFormat, testData2.testVerbosity, outputFormat, errorFormat));
    testRuns.append(&testData2);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Remove second test
    SUB_TEST_FUNCTION(removeTestAt(&model, 1));
    testRuns.removeAt(1);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::appendTwoRemoveBad(void)
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< TestRunData*> testRuns;

    // Append one test
    TestRunData testData1(mTests.at(qrand() % mTests.size()), mParserFormats.at(qrand() % mParserFormats.size()), (QTestLibModelTester::Verbosity) ((qrand() % ((int) QTestLibModelTester::VerbosityCountMinusOne + 1)) - 1));
    SUB_TEST_FUNCTION(appendTest(&model, testData1.testName, testData1.parserFormat, testData1.testVerbosity, outputFormat, errorFormat));
    testRuns.append(&testData1);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Append one test
    TestRunData testData2(mTests.at(qrand() % mTests.size()), mParserFormats.at(qrand() % mParserFormats.size()), (QTestLibModelTester::Verbosity) ((qrand() % ((int) QTestLibModelTester::VerbosityCountMinusOne + 1)) - 1));
    SUB_TEST_FUNCTION(appendTest(&model, testData2.testName, testData2.parserFormat, testData2.testVerbosity, outputFormat, errorFormat));
    testRuns.append(&testData2);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Try to remove non existing test
    SUB_TEST_FUNCTION(removeTestAt(&model, 2));
    //testRuns.removeAt(2);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::appendOneRemoveAppendOne(void)
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< TestRunData* > testRuns;

    // Append one test
    TestRunData testData1(mTests.at(qrand() % mTests.size()), mParserFormats.at(qrand() % mParserFormats.size()), (QTestLibModelTester::Verbosity) ((qrand() % ((int) QTestLibModelTester::VerbosityCountMinusOne + 1)) - 1));
    SUB_TEST_FUNCTION(appendTest(&model, testData1.testName, testData1.parserFormat, testData1.testVerbosity, outputFormat, errorFormat));
    testRuns.append(&testData1);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Remove first test
    SUB_TEST_FUNCTION(removeTestAt(&model, 0));
    testRuns.removeAt(0);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Append one test
    TestRunData testData2(mTests.at(qrand() % mTests.size()), mParserFormats.at(qrand() % mParserFormats.size()), (QTestLibModelTester::Verbosity) ((qrand() % ((int) QTestLibModelTester::VerbosityCountMinusOne + 1)) - 1));
    SUB_TEST_FUNCTION(appendTest(&model, testData2.testName, testData2.parserFormat, testData2.testVerbosity, outputFormat, errorFormat));
    testRuns.append(&testData2);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

}

void TestSuiteModelTest::appendOneRemoveBadAppendOne(void)
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< TestRunData* > testRuns;

    // Append one test
    TestRunData testData1(mTests.at(qrand() % mTests.size()), mParserFormats.at(qrand() % mParserFormats.size()), (QTestLibModelTester::Verbosity) ((qrand() % ((int) QTestLibModelTester::VerbosityCountMinusOne + 1)) - 1));
    SUB_TEST_FUNCTION(appendTest(&model, testData1.testName, testData1.parserFormat, testData1.testVerbosity, outputFormat, errorFormat));
    testRuns.append(&testData1);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Try to remove non existing test
    SUB_TEST_FUNCTION(removeTestAt(&model, 1));
    //testRuns.removeAt(1);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Append one test
    TestRunData testData2(mTests.at(qrand() % mTests.size()), mParserFormats.at(qrand() % mParserFormats.size()), (QTestLibModelTester::Verbosity) ((qrand() % ((int) QTestLibModelTester::VerbosityCountMinusOne + 1)) - 1));
    SUB_TEST_FUNCTION(appendTest(&model, testData2.testName, testData2.parserFormat, testData2.testVerbosity, outputFormat, errorFormat));
    testRuns.append(&testData2);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::appendTwoClear(void)
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< TestRunData* > testRuns;

    // Append one test
    TestRunData testData1(mTests.at(qrand() % mTests.size()), mParserFormats.at(qrand() % mParserFormats.size()), (QTestLibModelTester::Verbosity) ((qrand() % ((int) QTestLibModelTester::VerbosityCountMinusOne + 1)) - 1));
    SUB_TEST_FUNCTION(appendTest(&model, testData1.testName, testData1.parserFormat, testData1.testVerbosity, outputFormat, errorFormat));
    testRuns.append(&testData1);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Append one test
    TestRunData testData2(mTests.at(qrand() % mTests.size()), mParserFormats.at(qrand() % mParserFormats.size()), (QTestLibModelTester::Verbosity) ((qrand() % ((int) QTestLibModelTester::VerbosityCountMinusOne + 1)) - 1));
    SUB_TEST_FUNCTION(appendTest(&model, testData2.testName, testData2.parserFormat, testData2.testVerbosity, outputFormat, errorFormat));
    testRuns.append(&testData2);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Clear test suite
    SUB_TEST_FUNCTION(clearTests(&model));
    testRuns.clear();
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::appendOneClearAppendOne(void)
{
    QFETCH(Utils::OutputFormat, outputFormat);
    QFETCH(Utils::OutputFormat, errorFormat);

    QTestLibPlugin::Internal::TestSuiteModel model(this);
    QList< TestRunData* > testRuns;

    // Append one test
    TestRunData testData1(mTests.at(qrand() % mTests.size()), mParserFormats.at(qrand() % mParserFormats.size()), (QTestLibModelTester::Verbosity) ((qrand() % ((int) QTestLibModelTester::VerbosityCountMinusOne + 1)) - 1));
    SUB_TEST_FUNCTION(appendTest(&model, testData1.testName, testData1.parserFormat, testData1.testVerbosity, outputFormat, errorFormat));
    testRuns.append(&testData1);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Clear test suite
    SUB_TEST_FUNCTION(clearTests(&model));
    testRuns.clear();
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));

    // Append one test
    TestRunData testData2(mTests.at(qrand() % mTests.size()), mParserFormats.at(qrand() % mParserFormats.size()), (QTestLibModelTester::Verbosity) ((qrand() % ((int) QTestLibModelTester::VerbosityCountMinusOne + 1)) - 1));
    SUB_TEST_FUNCTION(appendTest(&model, testData2.testName, testData2.parserFormat, testData2.testVerbosity, outputFormat, errorFormat));
    testRuns.append(&testData2);
    SUB_TEST_FUNCTION(parseSuiteRoot(&model, testRuns));
}

void TestSuiteModelTest::parseSuiteRoot(const QAbstractItemModel* model, const QList< TestRunData* > &tests)
{
    BEGIN_SUB_TEST_FUNCTION

    QVERIFY(model->rowCount(QModelIndex()) == tests.size());
    QVERIFY(model->columnCount(QModelIndex()) == 3);

    int i = 0;
    QList< TestRunData* >::const_iterator it = tests.constBegin();
    QTestLibModelTester tester(model);

    for (; it!= tests.constEnd(); it++, i++) {
        QVERIFY2(model->index(i, 0, QModelIndex()).isValid(), qPrintable(QString("Children %1 of root element is not valid").arg(i)));

        tester.setFormat((*it)->parserFormat);
        if (QString::compare((*it)->parserFormat, "txt", Qt::CaseSensitive) == 0)
            tester.setVerbosity((*it)->testVerbosity);
        else if (QString::compare((*it)->parserFormat, "xml", Qt::CaseSensitive) == 0)
            tester.setVerbosity(qMax(QTestLibModelTester::Normal, (*it)->testVerbosity));
        else if (QString::compare((*it)->parserFormat, "lightxml", Qt::CaseSensitive) == 0)
            tester.setVerbosity(qMax(QTestLibModelTester::Normal, (*it)->testVerbosity));
        else if (QString::compare((*it)->parserFormat, "xunitxml", Qt::CaseSensitive) == 0)
            tester.setVerbosity(qMax(QTestLibModelTester::Normal, (*it)->testVerbosity));
        else
            QVERIFY2(false, qPrintable(QString("Unknown parser format %1").arg((*it)->parserFormat)));

        QVERIFY2(tester.checkIndex(model->index(i, 0, QModelIndex()), (*it)->testName), qPrintable(tester.error()));
    }

    END_SUB_TEST_FUNCTION
}

void TestSuiteModelTest::appendTest(QTestLibPlugin::Internal::TestSuiteModel *model, const QString& test, const QString& format, QTestLibModelTester::Verbosity verbosity, Utils::OutputFormat outputFormat, Utils::OutputFormat errorFormat)
{
    BEGIN_SUB_TEST_FUNCTION

    QString cmdArgs;
    int argsVersion = qrand() % 3;
    if ((format != "txt") && (argsVersion == 0))
        argsVersion = 2;
    if (argsVersion == 1)
        cmdArgs.append(QString("-%1 ").arg(format));
    else if (argsVersion == 2)
        cmdArgs.append(QString("-o -,%1 ").arg(format));

    switch(verbosity) {
    case QTestLibModelTester::Silent:
        cmdArgs.append("-silent");
        break;
    case QTestLibModelTester::Normal:
        break;
    case QTestLibModelTester::Verbose1:
        cmdArgs.append("-v1");
        break;
    case QTestLibModelTester::Verbose2:
        cmdArgs.append("-v2");
        break;
    case QTestLibModelTester::VerboseSignal:
        cmdArgs.append("-vs");
        break;
    default:
        qWarning() << "Sentinel value VerbosityCountMinusOne must not be used as verbosity.";
        break;
    }
    cmdArgs.trimmed();

    qDebug() << verbosity << format << cmdArgs;

    // Creation of RunConfiguration
    QmakeProjectManager::QmakeProject project(TESTS_DIR "/" + test + "/" + test + ".pro", this);
    ProjectExplorer::Kit kit;
    ProjectExplorer::Target target(&project, &kit);
    ProjectExplorer::LocalApplicationRunConfiguration runConfig(&target);
    runConfig.setCommandLineArguments(cmdArgs);
    runConfig.setExecutable(TESTS_DIR "/" + test + "/debug/" + test);
    runConfig.setWorkingDirectory(TESTS_DIR "/" + test);

    ProjectExplorer::RunControl *runControl = new ProjectExplorer::LocalApplicationRunControl(&runConfig, this);
    runControl->setFormats(outputFormat, errorFormat);
    model->appendTestRun(runControl);
    QSignalSpy runControlFinishedSpy(runControl, SIGNAL(finished()));
    runControl->start();
    QVERIFY2(runControlFinishedSpy.wait(30000), "Run control did not finish within 30s");

    END_SUB_TEST_FUNCTION
}

void TestSuiteModelTest::removeTestAt(QTestLibPlugin::Internal::TestSuiteModel *model, int i)
{
    BEGIN_SUB_TEST_FUNCTION

    QSignalSpy rowsAboutToBeRemovedSpy(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)));
    QSignalSpy rowsRemovedSpy(model, SIGNAL(rowsRemoved(QModelIndex, int, int)));

    int emissions = (int) (i >= 0) && (i < model->rowCount());
    model->removeTestRun(i);

    QVERIFY2(rowsAboutToBeRemovedSpy.count() == emissions, "RowsAboutToBeRemoved was not emitted");
    QVERIFY2(rowsRemovedSpy.count() == emissions, "RowsRemoved signal was not emitted");

    if (emissions == 1) {
        SUB_TEST_FUNCTION(checkSignalArguments("rowsAboutToBeRemoved()", rowsAboutToBeRemovedSpy.takeFirst(), i, i));
        SUB_TEST_FUNCTION(checkSignalArguments("rowsRemoved()", rowsRemovedSpy.takeFirst(), i, i));
    }

    END_SUB_TEST_FUNCTION
}

void TestSuiteModelTest::clearTests(QTestLibPlugin::Internal::TestSuiteModel *model)
{
    BEGIN_SUB_TEST_FUNCTION

    QSignalSpy modelAboutToBeResetSpy(model, SIGNAL(modelAboutToBeReset()));
    QSignalSpy modelResetSpy(model, SIGNAL(modelReset()));

    model->clear();

    QVERIFY2(modelAboutToBeResetSpy.count() == 1, "RowsAboutToBeRemoved was not emitted");
    QVERIFY2(modelResetSpy.count() == 1, "RowsRemoved signal was not emitted");

    END_SUB_TEST_FUNCTION
}

void TestSuiteModelTest::checkSignalArguments(const QString& signal, const QList<QVariant>& args, int first, int last)
{
    BEGIN_SUB_TEST_FUNCTION

    QVERIFY2(args.at(0).type() == QVariant::ModelIndex,
             qPrintable(QString("The first argument of signal %1 should be a QModelIndex").arg(signal)));
    QVERIFY2(!args.at(0).toModelIndex().isValid(),
             qPrintable(QString("The first argument of signal %1 should be an invalid QModelIndex").arg(signal)));
    QVERIFY2(args.at(1).type() == QVariant::Int,
             qPrintable(QString("The second argument of signal %1 should be an integer").arg(signal)));
    QVERIFY2(args.at(1).toInt() == first,
             qPrintable(QString("The second argument of signal %1 should be %2").arg(signal).arg(first)));
    QVERIFY2(args.at(2).type() == QVariant::Int,
             qPrintable(QString("The third argument of signal %1 should be an integer").arg(signal)));
    QVERIFY2(args.at(2).toInt() == last,
             qPrintable(QString("The third argument of signal %1 should be %2").arg(signal).arg(last)));

    END_SUB_TEST_FUNCTION
}

QTEST_MAIN(TestSuiteModelTest)

#include "testsuitemodeltest.moc"
