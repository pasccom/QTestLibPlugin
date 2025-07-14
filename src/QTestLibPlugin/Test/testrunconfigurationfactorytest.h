/* Copyright 2016-2020 Pascal COMBES <pascom@orange.fr>
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

#ifndef TESTRUNCONFIGURATIONFACTORYTEST_H
#define TESTRUNCONFIGURATIONFACTORYTEST_H

#include <QObject>
#include <qttestsubfunction.h>

namespace ProjectExplorer {
    class Project;
}

namespace QTestLibPlugin {
namespace Test {

class TestRunConfigurationFactoryTest : public QObject
{
    Q_OBJECT
    HAS_SUB_TEST_FUNCTIONS
public:
    TestRunConfigurationFactoryTest(void);
private Q_SLOTS:
    void init(void);
    void initTestCase(void);
    inline void testOpenProjectWithTestsCreate_data(void) {testOpenProjectWithTests_data();}
    inline void testOpenProjectWithTestsCreate(void) {testOpenProjectWithTests();}
    inline void testOpenProjectWithoutTestsCreate_data(void) {testOpenProjectWithoutTests_data();}
    inline void testOpenProjectWithoutTestsCreate(void) {testOpenProjectWithoutTests();}
    inline void testOpenProjectWithTestsRestore_data(void) {testOpenProjectWithTests_data();}
    inline void testOpenProjectWithTestsRestore(void) {testOpenProjectWithTests();}
    inline void testOpenProjectWithoutTestsRestore_data(void) {testOpenProjectWithoutTests_data();}
    inline void testOpenProjectWithoutTestsRestore(void) {testOpenProjectWithoutTests();}
    void cleanup(void);
private:
    ProjectExplorer::Project* mProject;

    void testOpenProjectWithTests_data(void);
    void testOpenProjectWithTests(void);
    void testOpenProjectWithoutTests_data(void);
    void testOpenProjectWithoutTests(void);
};

} // Test
} // QTestLibPlugin

#endif // TESTRUNCONFIGURATIONFACTORYTEST_H
