/* Copyright 2016 Pascal COMBES <pascom@orange.fr>
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

#ifndef TESTACTIONSTEST_H
#define TESTACTIONSTEST_H

#include <QObject>
#include "../../../tests/common/qttestsubfunction.h"

class QAction;

namespace ProjectExplorer {
    class Project;
}

namespace QTestLibPlugin {
namespace Test {

class TestActionsTest : public QObject
{
    Q_OBJECT
    HAS_SUB_TEST_FUNCTIONS
public:
    TestActionsTest(void);

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
    inline void testChangeTarget_data(void) {testOpenProjectWithTests_data();}
    void testChangeTarget(void);
    void testTwoProjectsWithTests_data(void);
    void testTwoProjectsWithTests(void);
    void testTwoProjectsWithAndWithoutTests_data(void);
    void testTwoProjectsWithAndWithoutTests(void);
    void testTwoProjectsWithoutAndWithTests_data(void);
    void testTwoProjectsWithoutAndWithTests(void);
    void testTwoProjectsWithoutTests_data(void);
    void testTwoProjectsWithoutTests(void);
    void cleanup(void);
private:
    ProjectExplorer::Project* mProject;
    ProjectExplorer::Project* mProject1;
    ProjectExplorer::Project* mProject2;

    void testOpenProjectWithTests_data(void);
    void testOpenProjectWithTests(void);
    void testOpenProjectWithoutTests_data(void);
    void testOpenProjectWithoutTests(void);

    void cleanupProject(int number = 0);
    void checkSubMenuAction(const QString& projectPath);
    void checkSubMenuAction(ProjectExplorer::Project* project, bool present, bool enabled);
    void checkContextMenuAction(ProjectExplorer::Project* project, bool enabled);
    void checkSubMenu(int actionCount);
    void setCurrentProjectTree(ProjectExplorer::Project* project);
    void runMakeCheck(ProjectExplorer::Project* project, QAction* runControlAction);
};

} // Test
} // QTestLibPlugin

#endif // TESTACTIONSTEST_H
