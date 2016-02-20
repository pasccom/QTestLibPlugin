#ifndef QMAKETESTRUNCONFIGURATIONFACTORY_H
#define QMAKETESTRUNCONFIGURATIONFACTORY_H

#include <projectexplorer/runconfiguration.h>

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

class QMakeTestRunConfigurationFactory : public ProjectExplorer::IRunConfigurationFactory
{
    Q_OBJECT
public:
    QMakeTestRunConfigurationFactory(QObject *parent = NULL);
    QList<Core::Id> availableCreationIds(ProjectExplorer::Target *target, CreationMode mode = UserCreate) const;
    QString displayNameForId(Core::Id id) const;

    bool canHandle(ProjectExplorer::Target* target) const;
    //bool canHandle(ProjectExplorer::Project* project) const;
    bool isUseful(ProjectExplorer::Project* project) const;

    bool canCreate(ProjectExplorer::Target* parent, Core::Id id) const;
    bool canRestore(ProjectExplorer::Target* parent, const QVariantMap &map) const;
    bool canClone(ProjectExplorer::Target* parent, ProjectExplorer::RunConfiguration *product) const;
    ProjectExplorer::RunConfiguration* clone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *product);

    //TestRunConfiguration* create(ProjectExplorer::Target* target);
    //void remove(ProjectExplorer::Target* target);

    //void createForAllTargets(ProjectExplorer::Project* project);
    //void removeForAllTargets(ProjectExplorer::Project* project);
//private slots:
    //void updateTargetKit(void);
private:
    ProjectExplorer::RunConfiguration* doCreate(ProjectExplorer::Target* target, Core::Id id);
    ProjectExplorer::RunConfiguration* doRestore(ProjectExplorer::Target* target, const QVariantMap& map);
};

} // Internal
} // QTestLibPlugin

#endif // QMAKETESTRUNCONFIGURATIONFACTORY_H
