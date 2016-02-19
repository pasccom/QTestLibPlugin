#ifndef QMAKETESTRUNCONFIGURATIONFACTORY_H
#define QMAKETESTRUNCONFIGURATIONFACTORY_H

#include <QtGlobal>

namespace ProjectExplorer {
    class Project;
    class Target;
}

namespace QmakeProjectManager {
    class QmakeProject;
}

namespace QTestLibPlugin {
namespace Internal {

class TestRunConfiguration;

class QMakeTestRunConfigurationFactory
{
public:
    QMakeTestRunConfigurationFactory(ProjectExplorer::Project *project = NULL);
    inline QMakeTestRunConfigurationFactory(QmakeProjectManager::QmakeProject* qMakeProject) :
        mQMakeProject(qMakeProject) {}
    bool canHandle(void) const;
    bool canHandle(ProjectExplorer::Target* target) const;
    bool isUseful(void) const;
    TestRunConfiguration* create(ProjectExplorer::Target* target);
    void createForAllTargets(void);
private:
    QmakeProjectManager::QmakeProject* mQMakeProject;
};

} // Internal
} // QTestLibPlugin

#endif // QMAKETESTRUNCONFIGURATIONFACTORY_H
