#ifndef QMAKETESTRUNCONFIGURATIONFACTORY_H
#define QMAKETESTRUNCONFIGURATIONFACTORY_H

#include <QObject>

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

class QMakeTestRunConfigurationFactory : public QObject
{
    Q_OBJECT
public:
    QMakeTestRunConfigurationFactory(QObject *parent = NULL);
    bool canHandle(ProjectExplorer::Project* project) const;
    bool canHandle(ProjectExplorer::Target* target) const;
    bool isUseful(ProjectExplorer::Project* project) const;
    TestRunConfiguration* create(ProjectExplorer::Target* target);
    void remove(ProjectExplorer::Target* target);
    void createForAllTargets(ProjectExplorer::Project* project);
    void removeForAllTargets(ProjectExplorer::Project* project);
};

} // Internal
} // QTestLibPlugin

#endif // QMAKETESTRUNCONFIGURATIONFACTORY_H
