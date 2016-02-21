/* Copyright 2015 Pascal COMBES <pascom@orange.fr>
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

#ifndef QTESTLIBPLUGIN_H
#define QTESTLIBPLUGIN_H

#include "qtestlibplugin_global.h"

#include <extensionsystem/iplugin.h>

class QAction;

namespace Core {
    class ActionContainer;
}

namespace ProjectExplorer {
    class Project;
    class Target;
    class RunConfiguration;
}

namespace QTestLibPlugin {
namespace Internal {

class TestSuiteModel;
class TestOutputPane;
class QMakeTestRunConfigurationFactory;

class QTestLibPluginPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "QTestLibPlugin.json")

public:
    QTestLibPluginPlugin();
    ~QTestLibPluginPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized(void);
    ShutdownFlag aboutToShutdown(void);
private slots:
    void handleProjectOpen(ProjectExplorer::Project* project);
    void handleProjectClose(ProjectExplorer::Project* project);
    void handleCurrentProjectTreeChange(ProjectExplorer::Project* project);

    void handleActiveTargetChange(ProjectExplorer::Target* target);

    void handleNewRunConfiguration(ProjectExplorer::RunConfiguration* runConfig);
    void handleDeleteRunConfiguration(ProjectExplorer::RunConfiguration* runConfig);
    /*void updateProjectTargets(void);*/
private:
    TestSuiteModel* mModel;
    TestOutputPane *mOutputPane;
    Core::ActionContainer* mRunTestsMenu;
    QAction* mRunTestsAction;
    ProjectExplorer::Project* mTreeCurrentProject;
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // QTESTLIBPLUGIN_H

