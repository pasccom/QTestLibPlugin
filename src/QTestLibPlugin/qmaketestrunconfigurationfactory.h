#ifndef QMAKETESTRUNCONFIGURATIONFACTORY_H
#define QMAKETESTRUNCONFIGURATIONFACTORY_H

#include <projectexplorer/runconfiguration.h>

namespace ProjectExplorer {
    class Project;
    class Target;
}

namespace QmakeProjectManager {
    class QmakeProject;
    class QmakeProFileNode;
}

namespace QTestLibPlugin {
namespace Internal {

class TestRunConfiguration;

class QMakeTestRunConfigurationFactory : public ProjectExplorer::IRunConfigurationFactory
{
    Q_OBJECT
public:
    QMakeTestRunConfigurationFactory(QObject *parent = NULL);
    QList<Core::Id> availableCreationIds(ProjectExplorer::Target *target, CreationMode mode = UserCreate) const;
    QString displayNameForId(Core::Id id) const;

    static bool canHandle(ProjectExplorer::Target* target);
    static bool isReady(ProjectExplorer::Project* project);
    static bool isUseful(ProjectExplorer::Project* project);

    bool canCreate(ProjectExplorer::Target* parent, Core::Id id) const;
    bool canRestore(ProjectExplorer::Target* parent, const QVariantMap &map) const;
    bool canClone(ProjectExplorer::Target* parent, ProjectExplorer::RunConfiguration *product) const;
    ProjectExplorer::RunConfiguration* clone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *product);
private:
    ProjectExplorer::RunConfiguration* doCreate(ProjectExplorer::Target* target, Core::Id id);
    ProjectExplorer::RunConfiguration* doRestore(ProjectExplorer::Target* target, const QVariantMap& map);

    //void updateRunConfiguration(TestRunConfiguration* runConfig, QmakeProjectManager::QmakeProFileNode* qMakeRoot);

};

} // Internal
} // QTestLibPlugin

#endif // QMAKETESTRUNCONFIGURATIONFACTORY_H
