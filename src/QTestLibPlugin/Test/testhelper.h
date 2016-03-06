#ifndef TESTHELPER_H
#define TESTHELPER_H

#include <QString>

namespace ProjectExplorer {
    class Project;
}

namespace QTestLibPlugin {
namespace Test {

bool openQMakeProject(const QString& projectFilePath, ProjectExplorer::Project** project);

} // Test
} // QTestLibPlugin

#endif // TESTHELPER_H
