#ifndef TESTRUNCONFIGURATIONFACTORYTEST_H
#define TESTRUNCONFIGURATIONFACTORYTEST_H

#include <QObject>
#include "../../tests/common/qttestsubfunction.h"

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

    void openProject(const QString& projectFilePath);
};

} // Test
} // QTestLibPlugin

#endif // TESTRUNCONFIGURATIONFACTORYTEST_H
