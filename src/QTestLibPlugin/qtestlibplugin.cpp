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

#include "qtestlibplugin.h"
#include "qtestlibpluginconstants.h"

#include "plaintextqtestlibparserfactory.h"
#include "xmlqtestlibparserfactory.h"
#include "lightxmlqtestlibparserfactory.h"
#include "xunitxmlqtestlibparserfactory.h"

#include "testoutputpane.h"
#include "testsuitemodel.h"

#include "qmaketestrunconfigurationfactory.h"

#ifdef BUILD_TESTS
#   include "Test/testactionstest.h"
#endif

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projecttree.h>
#include <projectexplorer/session.h>
#include <projectexplorer/project.h>
#include <projectexplorer/target.h>
#include <projectexplorer/runconfiguration.h>

#include <qmakeprojectmanager/qmakeproject.h>

#include <utils/proxyaction.h>

#include <QCoreApplication>
#include <QTranslator>
#include <QDir>
#include <QAction>
#include <QMenu>

#include <QtPlugin>

using namespace QTestLibPlugin::Internal;

QTestLibPluginPlugin::QTestLibPluginPlugin()
{
    mTreeCurrentProject = NULL;
    mModel = new TestSuiteModel(this);
}

QTestLibPluginPlugin::~QTestLibPluginPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

bool QTestLibPluginPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // Connect to other plugins' signals
    // In the initialize function, a plugin can be sure that the plugins it
    // depends on have initialized their members.

    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    // Locate QML file for the current language and use it
    QString qmFile = Core::ICore::userInterfaceLanguage();
    if (qmFile.isEmpty())
        qmFile = QLatin1String("en");
    qmFile = QString(QLatin1String("qtestlibplugin_%1.qm")).arg(qmFile);

    QTranslator *translator = new QTranslator(this);
    if (translator->load(qmFile, Core::ICore::resourcePath() + QDir::separator() + QLatin1String("translations")) ||
        translator->load(qmFile, Core::ICore::userResourcePath() + QDir::separator() + QLatin1String("translations")))
        qApp->installTranslator(translator);
    else
        qWarning() << qPrintable(QString(QLatin1String("Translator file \"%1\" not found")).arg(qmFile));

    // Parser factories
    PlainTextQTestLibParserFactory *plainTextFactory = new PlainTextQTestLibParserFactory(this);
    addAutoReleasedObject(plainTextFactory);
    XMLQTestLibParserFactory *xmlFactory = new XMLQTestLibParserFactory(this);
    addAutoReleasedObject(xmlFactory);
    LightXMLQTestLibParserFactory *lightXmlFactory = new LightXMLQTestLibParserFactory(this);
    addAutoReleasedObject(lightXmlFactory);
    XUnitXMLQTestLibParserFactory *xUnitXmlFactory = new XUnitXMLQTestLibParserFactory(this);
    addAutoReleasedObject(xUnitXmlFactory);

    // Run configuration factories
    ProjectExplorer::IRunConfigurationFactory* runConfigFactory = new QMakeTestRunConfigurationFactory;
    addAutoReleasedObject(runConfigFactory);

    // New sub-menu in build menu and action in project tree context menu
    mRunTestsMenu = Core::ActionManager::createMenu(Constants::TestRunMenuId);
    qDebug() << "Run tests menu:" << mRunTestsMenu->menu();
    mRunTestsMenu->menu()->setTitle(tr("Run tests"));
    Core::ActionContainer* buildMenu = Core::ActionManager::actionContainer(ProjectExplorer::Constants::M_BUILDPROJECT);
    Q_ASSERT(buildMenu != NULL);
    buildMenu->addMenu(mRunTestsMenu, ProjectExplorer::Constants::G_BUILD_RUN);

    mRunTestsAction = new QAction(tr("Run tests"), this);
    qDebug() << "Run tests action:" << mRunTestsAction;
    Core::Context projectTreeContext(ProjectExplorer::Constants::C_PROJECT_TREE);
    Core::ActionContainer* projectContextMenu = Core::ActionManager::actionContainer(ProjectExplorer::Constants::M_PROJECTCONTEXT);
    Q_ASSERT(projectContextMenu != NULL);
    Core::Command* cmd = Core::ActionManager::registerAction(mRunTestsAction, Constants::TestRunActionId, projectTreeContext);
    cmd->setAttribute(Core::Command::CA_UpdateText);
    cmd->setAttribute(Core::Command::CA_NonConfigurable);
    projectContextMenu->addAction(cmd, ProjectExplorer::Constants::G_PROJECT_RUN);

    // Output pane
    mOutputPane = new TestOutputPane(mModel);
    addAutoReleasedObject(mOutputPane);

    // Load plugin settings
    QSettings *settings = Core::ICore::settings(QSettings::UserScope);
    settings->beginGroup(QTestLibPlugin::Constants::PluginName);
    mOutputPane->loadSettings(settings);
    settings->endGroup();

    // Connections
    connect(ProjectExplorer::ProjectExplorerPlugin::instance(), SIGNAL(runControlStarted(ProjectExplorer::RunControl*)),
            mModel, SLOT(appendTestRun(ProjectExplorer::RunControl*)));
    connect(ProjectExplorer::SessionManager::instance(), SIGNAL(projectAdded(ProjectExplorer::Project*)),
            this, SLOT(handleProjectOpen(ProjectExplorer::Project*)));
    connect(ProjectExplorer::SessionManager::instance(), SIGNAL(aboutToRemoveProject(ProjectExplorer::Project*)),
            this, SLOT(handleProjectClose(ProjectExplorer::Project*)));
    connect(ProjectExplorer::ProjectTree::instance(), SIGNAL(currentProjectChanged(ProjectExplorer::Project*)),
            this, SLOT(handleCurrentProjectTreeChange(ProjectExplorer::Project*)));
    connect(mRunTestsAction, SIGNAL(triggered()),
            this, SLOT(runTest()));

    return true;
}

void QTestLibPluginPlugin::extensionsInitialized(void)
{
    // Retrieve objects from the plugin manager's object pool
    // In the extensionsInitialized function, a plugin can be sure that all
    // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag QTestLibPluginPlugin::aboutToShutdown(void)
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)

    // Save plugin settings
    QSettings *settings = Core::ICore::settings(QSettings::UserScope);
    settings->beginGroup(QTestLibPlugin::Constants::PluginName);
    mOutputPane->saveSettings(settings);
    settings->endGroup();

    return SynchronousShutdown;
}


void QTestLibPluginPlugin::handleProjectOpen(ProjectExplorer::Project* project)
{
    // TODO remove it!
    qDebug() << "Opened project:" << project->displayName();

    /*foreach (ProjectExplorer::Target *t, project->targets()) {
        qDebug() << "    Target:" << t->displayName() << t->metaObject()->className() << t->id().toString();
        foreach (ProjectExplorer::RunConfiguration* r, t->runConfigurations()) {
            qDebug() << "        Run config:" << r->displayName() << r->metaObject()->className() << r->id().toString();
        }
    }*/

    // Used to update the run test action before the project is ready (see detailed explanations below)
    qDebug() << "Disabled RunTestsAction";
    mRunTestsAction->setEnabled(false);
    mTreeCurrentProject = project;

    connect(project, SIGNAL(activeTargetChanged(ProjectExplorer::Target*)),
            this, SLOT(handleActiveTargetChange(ProjectExplorer::Target*)));
    handleActiveTargetChange(project->activeTarget(), false);
}

void QTestLibPluginPlugin::handleProjectClose(ProjectExplorer::Project* project)
{
    // TODO remove it!
    qDebug() << "Closed project:" << project->displayName();

    /*foreach (ProjectExplorer::Target *t, project->targets()) {
        qDebug() << "    Target:" << t->displayName() << t->metaObject()->className() << t->id().toString();
        foreach (ProjectExplorer::RunConfiguration* r, t->runConfigurations()) {
            qDebug() << "        Run config:" << r->displayName() << r->metaObject()->className() << r->id().toString();
        }
    }*/

    Core::Command* cmd = Core::ActionManager::command(Core::Id(Constants::TestRunActionId).withSuffix(project->projectFilePath().toString()));
    if (cmd != NULL) {
        QAction* action = cmd->action();
        if (qobject_cast<Utils::ProxyAction*>(action) != NULL)
            action = qobject_cast<Utils::ProxyAction*>(action)->action();

        mRunTestsMenu->menu()->removeAction(cmd->action());
        Core::ActionManager::unregisterAction(action, Core::Id(Constants::TestRunActionId).withSuffix(project->projectFilePath().toString()));
        delete action;
    }

    if (mTreeCurrentProject == project) {
        mTreeCurrentProject = NULL;
        mRunTestsAction->setEnabled(false);
        mRunTestsAction->setText(tr("Run tests"));
    }

    disconnect(project, SIGNAL(activeTargetChanged(ProjectExplorer::Target*)),
               this, SLOT(handleActiveTargetChange(ProjectExplorer::Target*)));
}

void QTestLibPluginPlugin::handleActiveTargetChange(ProjectExplorer::Target* target, bool clean)
{
    if (target == NULL)
        return;

    qDebug() << "Changed to target:" << target->displayName();

    if (target->project() == mTreeCurrentProject) {
        if (target == NULL)
            mRunTestsAction->setText(tr("Run tests for \"%1\"").arg(target->project()->displayName()));
        else
            mRunTestsAction->setText(tr("Run tests for \"%1\" (%2)").arg(target->project()->displayName()).arg(target->displayName()));
    }

    ProjectExplorer::Project* project = qobject_cast<ProjectExplorer::Project*>(sender());
    if (clean && (project != NULL)) {
        foreach (ProjectExplorer::Target* t, project->targets()) {
            disconnect(t, SIGNAL(addedRunConfiguration(ProjectExplorer::RunConfiguration*)),
                       this, SLOT(handleNewRunConfiguration(ProjectExplorer::RunConfiguration*)));
            disconnect(t, SIGNAL(removedRunConfiguration(ProjectExplorer::RunConfiguration*)),
                       this, SLOT(handleNewRunConfiguration(ProjectExplorer::RunConfiguration*)));
            foreach (ProjectExplorer::RunConfiguration* runConfig, t->runConfigurations())
                handleDeleteRunConfiguration(runConfig);
        }
    }

    connect(target, SIGNAL(addedRunConfiguration(ProjectExplorer::RunConfiguration*)),
            this, SLOT(handleNewRunConfiguration(ProjectExplorer::RunConfiguration*)));
    connect(target, SIGNAL(removedRunConfiguration(ProjectExplorer::RunConfiguration*)),
            this, SLOT(handleDeleteRunConfiguration(ProjectExplorer::RunConfiguration*)));
    foreach (ProjectExplorer::RunConfiguration* runConfig, target->runConfigurations())
        handleNewRunConfiguration(runConfig);
}

void QTestLibPluginPlugin::handleNewRunConfiguration(ProjectExplorer::RunConfiguration* runConfig)
{
    if (runConfig->id() != Core::Id(Constants::TestRunConfigurationId))
        return;

    qDebug() << "Added a test run configuration:" << runConfig->target()->displayName();
    ProjectExplorer::Target* target = runConfig->target();
    Q_ASSERT(target != NULL);
    ProjectExplorer::Project* project = target->project();
    Q_ASSERT(project != NULL);

    Core::Command* cmd = Core::ActionManager::command(Core::Id(Constants::TestRunActionId).withSuffix(project->projectFilePath().toString()));
    if (cmd == NULL) {
        QAction *action = new QAction(tr("Run tests for \"%1\"").arg(project->displayName()), this);
        connect(action, &QAction::triggered,
                this, [runConfig] () {
            ProjectExplorer::ProjectExplorerPlugin::runRunConfiguration(runConfig, ProjectExplorer::Constants::NORMAL_RUN_MODE, true);
        });

        cmd = Core::ActionManager::registerAction(action, Core::Id(Constants::TestRunActionId).withSuffix(project->projectFilePath().toString()));
        cmd->setAttribute(Core::Command::CA_UpdateText);
        cmd->setAttribute(Core::Command::CA_Hide);
        cmd->setAttribute(Core::Command::CA_NonConfigurable);
        mRunTestsMenu->addAction(cmd);
    }

    cmd->action()->setText(tr("Run tests for \"%1\" (%2)").arg(project->displayName()).arg(target->displayName()));
    cmd->action()->setEnabled(true);

    // Used to update the run test action before the project is ready (see detailed explanations below)
    if (project == mTreeCurrentProject) {
        qDebug() << "Enabled RunTestsAction";
        mRunTestsAction->setEnabled(true);
    }
}

void QTestLibPluginPlugin::handleDeleteRunConfiguration(ProjectExplorer::RunConfiguration* runConfig)
{
    if (runConfig->id() != Core::Id(Constants::TestRunConfigurationId))
        return;

    qDebug() << "Removed a test run configuration:" << runConfig->target()->displayName();
    ProjectExplorer::Target* target = runConfig->target();
    Q_ASSERT(target != NULL);
    ProjectExplorer::Project* project = target->project();
    Q_ASSERT(project != NULL);

    Core::Command* cmd = Core::ActionManager::command(Core::Id(Constants::TestRunActionId).withSuffix(project->projectFilePath().toString()));
    if (cmd != NULL)
        cmd->action()->setEnabled(false);

    // Used to update the run test action before the project is ready (see detailed explanations below)
    if (project == mTreeCurrentProject) {
        qDebug() << "Disabled RunTestsAction";
        mRunTestsAction->setEnabled(false);
    }
}

/*
 * NOTE This function can be called when the project is not yet initialized.
 * That is why, the test QMakeTestRunConfigurationFactory::isReady(project) is used,
 * before relying on QMakeTestRunConfigurationFactory::isUseful(project).
 *
 * When the project is not ready when this function is called we have no clue...
 *
 * That is why we disable the action when the project is open and save the current project
 * (see handleProjectOpen(ProjectExplorer::Project*)). Then if a TestRunConfiguration is added
 * or removed during project loading, we test the current project.
 * If it matches we enable or disable the action.
 */
void QTestLibPluginPlugin::handleCurrentProjectTreeChange(ProjectExplorer::Project* project)
{
    qDebug() << __func__ << (project != NULL ? project->displayName() : QString::null);

    mTreeCurrentProject = project;
    if (project == NULL)
        return;

    if (QMakeTestRunConfigurationFactory::isReady(project)) {
        mRunTestsAction->setEnabled(QMakeTestRunConfigurationFactory::isUseful(project));
        qDebug() << (mRunTestsAction->isEnabled() ? "Enabled RunTestsAction" : "Disabled RunTestsAction");
    }

    ProjectExplorer::Target* target = project->activeTarget();
    if (target == NULL)
        mRunTestsAction->setText(tr("Run tests for \"%1\"").arg(project->displayName()));
    else
        mRunTestsAction->setText(tr("Run tests for \"%1\" (%2)").arg(project->displayName()).arg(target->displayName()));
}

void QTestLibPluginPlugin::runTest(void)
{
    QTC_ASSERT(mTreeCurrentProject != NULL, return);
    QTC_ASSERT(mTreeCurrentProject->activeTarget() != NULL, return);

    ProjectExplorer::RunConfiguration* runConfig = NULL;
    foreach (ProjectExplorer::RunConfiguration* rc, mTreeCurrentProject->activeTarget()->runConfigurations()) {
        if (rc->id() == Core::Id(Constants::TestRunConfigurationId))
            runConfig = rc;
    }

    qDebug() << runConfig;

    QTC_ASSERT(runConfig != NULL, return);

    ProjectExplorer::ProjectExplorerPlugin::runRunConfiguration(runConfig, ProjectExplorer::Constants::NORMAL_RUN_MODE, true);
}

#ifdef BUILD_TESTS
QList<QObject *> QTestLibPluginPlugin::createTestObjects(void) const
{
    QList<QObject *> testObjects;

    testObjects << new Test::TestActionsTest;

    return testObjects;
}
#endif
