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

#ifndef LIGHTXMLQTESTLIBPARSERTEST_H
#define LIGHTXMLQTESTLIBPARSERTEST_H

#include <QObject>
#include <QDateTime>

#include <qtestlibargsparser.h>

namespace ProjectExplorer {
    class Project;
    class RunConfiguration;
}

namespace QTestLibPlugin {
namespace Test {

class LightXMLQTestLibParserFactoryTest : public QObject
{
    Q_OBJECT
public:
    inline LightXMLQTestLibParserFactoryTest(void) {qsrand(QDateTime::currentMSecsSinceEpoch());}
private Q_SLOTS:
    void initTestCase(void);
    void init(void);
    inline void testOneClass_data(void) {dataTest();}
    void testOneClass(void);
    inline void testAllMessages_data(void) {dataTest();}
    void testAllMessages(void);
    inline void testMultipleClasses_data(void) {dataTest();}
    void testMultipleClasses(void);
    inline void testSignalsTest_data(void) {dataTest();}
    void testSignalsTest(void);
    inline void testLimits_data(void) {dataTest();}
    void testLimits(void);
    inline void testOneSubTest_data(void) {dataMakeCheck();}
    void testOneSubTest(void);
    inline void testTwoSubTests_data(void) {dataMakeCheck();}
    void testTwoSubTests(void);
    void cleanup(void);
private:
    void dataTest(void);
    void dataMakeCheck(void);
    void runTest(const QString& testName, const QStringList &cmdArgs, bool result);
    void runMakeCheck(const QString& testName, Internal::QTestLibArgsParser::TestOutputFormat format, Internal::QTestLibArgsParser::TestVerbosity verbosity, bool result);

    ProjectExplorer::Project* mProject;
};

} // Test
} // QTestLibPlugin

#endif // LIGHTXMLQTESTLIBPARSERTEST_H
