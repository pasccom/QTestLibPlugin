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
    void testOpenProjectWithTests_data(void);
    void testOpenProjectWithTests(void);
    void testOpenProjectWithoutTests_data(void);
    void testOpenProjectWithoutTests(void);
    void cleanup(void);
private:
    ProjectExplorer::Project* mProject;

    void openProject(const QString& projectFilePath);
};

} // Test
} // QTestLibPlugin

#endif // QMAKEMAKECHECKTEST_H
