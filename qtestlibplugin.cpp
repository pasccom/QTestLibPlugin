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

#include "testoutputpane.h"
#include "testsuitemodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>

#include <projectexplorer/projectexplorer.h>

#include <QCoreApplication>
#include <QTranslator>
#include <QDir>

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

    mOutputPane = new TestOutputPane(mModel);
    addAutoReleasedObject(mOutputPane);

    connect(ProjectExplorer::ProjectExplorerPlugin::instance(), SIGNAL(runControlStarted(ProjectExplorer::RunControl*)),
            mModel, SLOT(appendTestRun(ProjectExplorer::RunControl*)));

    return true;
}

void QTestLibPluginPlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // In the extensionsInitialized function, a plugin can be sure that all
    // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag QTestLibPluginPlugin::aboutToShutdown()
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
