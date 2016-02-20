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

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/session.h>
#include <projectexplorer/project.h>
#include <projectexplorer/target.h>
#include <projectexplorer/runconfiguration.h>

#include <qmakeprojectmanager/qmakeproject.h>

#include <QCoreApplication>
#include <QTranslator>
#include <QDir>
#include <QAction>
#include <QMenu>

#include <QtPlugin>

using namespace QTestLibPlugin::Internal;

QTestLibPluginPlugin::QTestLibPluginPlugin()
{
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

    PlainTextQTestLibParserFactory *plainTextFactory = new PlainTextQTestLibParserFactory(this);
    addAutoReleasedObject(plainTextFactory);
    XMLQTestLibParserFactory *xmlFactory = new XMLQTestLibParserFactory(this);
    addAutoReleasedObject(xmlFactory);
    LightXMLQTestLibParserFactory *lightXmlFactory = new LightXMLQTestLibParserFactory(this);
    addAutoReleasedObject(lightXmlFactory);
    XUnitXMLQTestLibParserFactory *xUnitXmlFactory = new XUnitXMLQTestLibParserFactory(this);
    addAutoReleasedObject(xUnitXmlFactory);

    ProjectExplorer::IRunConfigurationFactory* runConfigFactory = new QMakeTestRunConfigurationFactory;
    addAutoReleasedObject(runConfigFactory);

    mRunTestsMenu = Core::ActionManager::createMenu(Constants::TestRunMenuId);
    mRunTestsMenu->menu()->setTitle(tr("Run tests"));
    Core::ActionContainer* buildMenu = Core::ActionManager::actionContainer(ProjectExplorer::Constants::M_BUILDPROJECT);
    Q_ASSERT(buildMenu != NULL);
    buildMenu->addMenu(mRunTestsMenu, ProjectExplorer::Constants::G_BUILD_RUN);

    mOutputPane = new TestOutputPane(mModel);
    addAutoReleasedObject(mOutputPane);

    QSettings *settings = Core::ICore::settings(QSettings::UserScope);
    settings->beginGroup(QTestLibPlugin::Constants::PluginName);
    mOutputPane->loadSettings(settings);
    settings->endGroup();

    connect(ProjectExplorer::ProjectExplorerPlugin::instance(), SIGNAL(runControlStarted(ProjectExplorer::RunControl*)),
            mModel, SLOT(appendTestRun(ProjectExplorer::RunControl*)));
    // NOTE this is done automatically thanks to QMakeTestRunConfigFactory
    connect(ProjectExplorer::SessionManager::instance(), SIGNAL(projectAdded(ProjectExplorer::Project*)),
            this, SLOT(handleProjectOpen(ProjectExplorer::Project*)));
    connect(ProjectExplorer::SessionManager::instance(), SIGNAL(aboutToRemoveProject(ProjectExplorer::Project*)),
            this, SLOT(handleProjectClose(ProjectExplorer::Project*)));

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

    connect(project, SIGNAL(activeTargetChanged(ProjectExplorer::Target*)),
            this, SLOT(handleActiveTargetChange(ProjectExplorer::Target*)));
    handleActiveTargetChange(project->activeTarget());
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

    disconnect(project, SIGNAL(activeTargetChanged(ProjectExplorer::Target*)),
               this, SLOT(handleActiveTargetChange(ProjectExplorer::Target*)));
}

void QTestLibPluginPlugin::handleActiveTargetChange(ProjectExplorer::Target* target)
{
    if (target == NULL)
        return;

    ProjectExplorer::Project* project = qobject_cast<ProjectExplorer::Project*>(sender());
    if (project != NULL) {
        foreach (ProjectExplorer::Target* t, project->targets()) {
            disconnect(t, SIGNAL(addedRunConfiguration(ProjectExplorer::RunConfiguration*)),
                       this, SLOT(handleNewRunConfiguration(ProjectExplorer::RunConfiguration*)));
            disconnect(t, SIGNAL(removedRunConfiguration(ProjectExplorer::RunConfiguration*)),
                       this, SLOT(handleNewRunConfiguration(ProjectExplorer::RunConfiguration*)));
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

    qDebug() << "Added a test run configuration!";
    ProjectExplorer::Target* target = runConfig->target();
    Q_ASSERT(target != NULL);
    ProjectExplorer::Project* project = target->project();
    Q_ASSERT(project != NULL);

    Core::Command* cmd = Core::ActionManager::command(Core::Id(Constants::TestRunActionId).withSuffix(project->id().toString()));
    if (cmd != NULL) {
        cmd->action()->setEnabled(true);
    } else {
        QAction *action = new QAction(tr("Run tests for \"%1\" (%2)").arg(project->displayName()).arg(target->displayName()), this);
        cmd = Core::ActionManager::registerAction(action, Core::Id(Constants::TestRunActionId).withSuffix(project->id().toString()));
        cmd->setAttribute(Core::Command::CA_Hide);
        cmd->setAttribute(Core::Command::CA_NonConfigurable);
        mRunTestsMenu->addAction(cmd);
    }
}

void QTestLibPluginPlugin::handleDeleteRunConfiguration(ProjectExplorer::RunConfiguration* runConfig)
{
    if (runConfig->id() != Core::Id(Constants::TestRunConfigurationId))
        return;

    qDebug() << "Removed a test run configuration!";
    ProjectExplorer::Target* target = runConfig->target();
    Q_ASSERT(target != NULL);
    ProjectExplorer::Project* project = target->project();
    Q_ASSERT(project != NULL);

    Core::Command* cmd = Core::ActionManager::command(Core::Id(Constants::TestRunActionId).withSuffix(project->id().toString()));
    Q_ASSERT(cmd != NULL);
    cmd->action()->setEnabled(false);
}

/*void QTestLibPluginPlugin::updateProjectTargets(void)
{
    ProjectExplorer::Project* project = qobject_cast<ProjectExplorer::Project*>(sender());

    if (mRunConfigFactory->isUseful(project))
        mRunConfigFactory->createForAllTargets(project);
    else
        mRunConfigFactory->removeForAllTargets(project);
}*/
