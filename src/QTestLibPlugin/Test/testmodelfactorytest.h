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

#ifndef TESTMODELFACTORYTEST_H
#define TESTMODELFACTORYTEST_H

#include <QObject>
#include <QDateTime>
#include <QRandomGenerator>

#include "../qtestlibargsparser.h"

class QAbstractItemModel;

namespace Utils {
    class Id;
}

namespace ProjectExplorer {
    class Project;
    class RunConfiguration;
}

namespace QTestLibPlugin {
namespace Test {

class TestModelFactoryTest : public QObject
{
    Q_OBJECT
public:
    inline TestModelFactoryTest(void) {mRandom = QRandomGenerator::global();}
private Q_SLOTS:
    void initTestCase(void);
    void init(void);
    inline void testOneClass_data(void) {data();}
    void testOneClass(void);
    inline void testAllMessages_data(void) {data();}
    void testAllMessages(void);
    inline void testMultipleClasses_data(void) {data();}
    void testMultipleClasses(void);
    inline void testSignalsTest_data(void) {data();}
    void testSignalsTest(void);
    inline void testLimits_data(void) {data();}
    void testLimits(void);
    inline void testOneSubTest_data(void) {data();}
    void testOneSubTest(void);
    inline void testTwoSubTests_data(void) {data();}
    void testTwoSubTests(void);
    void cleanup(void);
private:
    void data(void);
    QStringList commandLineArguments(Internal::QTestLibArgsParser::TestOutputFormat format, Internal::QTestLibArgsParser::TestVerbosity verbosity);
    QString formatToString(Internal::QTestLibArgsParser::TestOutputFormat format);
    void runTest(const QString& qtVersion, const QString& testName, Internal::QTestLibArgsParser::TestOutputFormat format, Internal::QTestLibArgsParser::TestVerbosity verbosity, Utils::Id runMode);
    void runMakeCheck(const QString& qtVersion, const QString& testName, Internal::QTestLibArgsParser::TestOutputFormat format, Internal::QTestLibArgsParser::TestVerbosity verbosity, Utils::Id runMode);
    void runRunConfiguration(ProjectExplorer::RunConfiguration* runConfig, const QString& resultsFilePath, Internal::QTestLibArgsParser::TestOutputFormat format, Internal::QTestLibArgsParser::TestVerbosity verbosity, Utils::Id runMode);

    ProjectExplorer::Project* mProject;
    QAbstractItemModel *mFoundModel;
    QAbstractItemModel *mPopulatedModel;
    int mFoundCount;
    int mPopulatedCount;
    bool mOrderOk;
    QRandomGenerator* mRandom;
};

} // Test
} // QTestLibPlugin

#endif // TESTMODELFACTORYTEST_H
