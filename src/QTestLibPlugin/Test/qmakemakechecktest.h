#ifndef QMAKEMAKECHECKTEST_H
#define QMAKEMAKECHECKTEST_H

#include <QObject>
#include "../../tests/common/qttestsubfunction.h"

namespace ProjectExplorer {
    class Project;
}

namespace QTestLibPlugin {
namespace Test {

class QMakeMakeCheckTest : public QObject
{
    Q_OBJECT
    HAS_SUB_TEST_FUNCTIONS
public:
    QMakeMakeCheckTest(void);

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

    void openProject(const QString& projectFilePath, int number = 0);
    void closeProject(int number = 0);
    void checkSubMenuAction(const QString& projectPath);
    void checkSubMenuAction(ProjectExplorer::Project* project, bool present, bool enabled);
    void checkContextMenuAction(ProjectExplorer::Project* project, bool enabled);
    void checkSubMenu(int actionCount);
    void setCurrentProjectTree(ProjectExplorer::Project* project);
};

} // Test
} // QTestLibPlugin

#endif // QMAKEMAKECHECKTEST_H
