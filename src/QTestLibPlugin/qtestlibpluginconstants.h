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

#ifndef QTESTLIBPLUGINCONSTANTS_H
#define QTESTLIBPLUGINCONSTANTS_H

#include <QString>

#define TEST_RUN_CONFIGURATION_ID "QTestLibPlugin.TestRunConfiguration"

namespace QTestLibPlugin {
namespace Constants {

const QString PluginName = QLatin1String("QTestLibPlugin");

// Viewer settings:
const QString FilterProxyGroup = QLatin1String("FilterProxy");
const QString ViewGroup = QLatin1String("View");
const QString DescWidthKey = QLatin1String("DescWidth");
const QString FileWidthKey = QLatin1String("FileWidth");
const QString LineWidthKey = QLatin1String("LineWidth");

// Test run configuration settings:
const QString WorkingDirectoryKey = QLatin1String(TEST_RUN_CONFIGURATION_ID ".WorkingDirectory");
const QString MakeExeKey = QLatin1String(TEST_RUN_CONFIGURATION_ID ".MakeExe");
const QString TestRunnerKey = QLatin1String(TEST_RUN_CONFIGURATION_ID ".TestRunner");
const QString MakeJobNumberKey = QLatin1String(TEST_RUN_CONFIGURATION_ID ".JobNumber");

// IDs:
const char TestRunConfigurationId [] = TEST_RUN_CONFIGURATION_ID;
const char TestRunConfigurationExtraAspectId [] = "QTestlibPlugin.TestRunConfigurationExtraAspect";
const char TestRunMenuId[] = "QTestLibPlugin.Menu.TestRun";
const char TestRunActionId[] = "QTestLibPlugin.Action.TestRun";

} // namespace QTestLibPlugin
} // namespace Constants

#endif // QTESTLIBPLUGINCONSTANTS_H

