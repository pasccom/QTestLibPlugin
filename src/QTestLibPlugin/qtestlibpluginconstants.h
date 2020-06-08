/* Copyright 2015-2020 Pascal COMBES <pascom@orange.fr>
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
#define TEST_RUN_CONFIGURATION_EXTRA_ASPECT_ID "QTestlibPlugin.TestRunConfigurationExtraAspect"

namespace QTestLibPlugin {
namespace Constants {
/*!
 * \defgroup QTestLibPluginConstants QTestLibPlugin constants
 * @{
 */
const QString PluginName = QLatin1String("QTestLibPlugin");                                                             /*!< The nane of the plugin (used as root group name in the settings) */

/*!
  * \defgroup ViewerSettings Viewer settings
  * @{
  */
const QString FilterProxyGroup = QLatin1String("FilterProxy");                                                          /*!< Group name for Internal::TestProxyModel settings */
const QString ViewGroup = QLatin1String("View");                                                                        /*!< Group name for Internal::TestOutputPane tree view settings */
const QString DescWidthKey = QLatin1String("DescWidth");                                                                /*!< Key for message field width in Internal::TestOutputPane */
const QString FileWidthKey = QLatin1String("FileWidth");                                                                /*!< Key for file field width in Internal::TestOutputPane */
const QString LineWidthKey = QLatin1String("LineWidth");                                                                /*!< Key for line field width in Internal::TestOutputPane */
/*!@}*/

/*!
 * \defgroup TestRunConfigurationSettings Test run configuration settings
 * @{
 */
const QString WorkingDirectoryKey = QLatin1String(TEST_RUN_CONFIGURATION_ID ".WorkingDirectory");                       /*!< Key for working directory path in Internal::TestRunConfiguration */
const QString MakeExeKey = QLatin1String(TEST_RUN_CONFIGURATION_ID ".MakeExe");                                         /*!< Key for path to \c make in Internal::TestRunConfiguration */
const QString MakefileKey = QLatin1String(TEST_RUN_CONFIGURATION_ID ".Makefile");                                       /*!< Key for \c Makefile path in Internal::TestRunConfiguration */
const QString TestRunnerKey = QLatin1String(TEST_RUN_CONFIGURATION_ID ".TestRunner");                                   /*!< Key for test runner path in Internal::TestRunConfiguration */
const QString MakeJobNumberKey = QLatin1String(TEST_RUN_CONFIGURATION_ID ".JobNumber");                                 /*!< Key for job number of \c make in Internal::TestRunConfiguration */
/*!@}*/

/*!
 * \defgroup TestRunConfigurationExtraAspectSettings Test run configuration extra aspect settings
 * @{
 */
const QString FormatKey = QLatin1String(TEST_RUN_CONFIGURATION_EXTRA_ASPECT_ID ".Format");                              /*!< Key for test output format of Internal::TestRunConfigurationExtraAspect */
const QString VerbosityKey = QLatin1String(TEST_RUN_CONFIGURATION_EXTRA_ASPECT_ID ".Verbosity");                        /*!< Key for test verbosity of Internal::TestRunConfigurationExtraAspect */
const QString OutputFileKey = QLatin1String(TEST_RUN_CONFIGURATION_EXTRA_ASPECT_ID ".LogFile");                         /*!< Key for log file path in Internal::TestRunConfigurationExtraAspect */
const QString MaxWarningKey = QLatin1String(TEST_RUN_CONFIGURATION_EXTRA_ASPECT_ID ".MaxWarnings");                     /*!< Key for maximum number of warnings field of Internal::TestRunConfigurationExtraAspect */
const QString EventDelayKey = QLatin1String(TEST_RUN_CONFIGURATION_EXTRA_ASPECT_ID ".EventDelay");                      /*!< Key for event delay field of Internal::TestRunConfigurationExtraAspect */
const QString KeyDelayKey = QLatin1String(TEST_RUN_CONFIGURATION_EXTRA_ASPECT_ID ".KeyDelay");                          /*!< Key for key delay field of Internal::TestRunConfigurationExtraAspect */
const QString MouseDelayKey = QLatin1String(TEST_RUN_CONFIGURATION_EXTRA_ASPECT_ID ".MouseDelay");                      /*!< Key for mouse delay field of Internal::TestRunConfigurationExtraAspect */
const QString CrashHandlerEnabledKey = QLatin1String(TEST_RUN_CONFIGURATION_EXTRA_ASPECT_ID ".CrashHandlerEnabled");    /*!< Key for crash handler enabled field of Internal::TestRunConfigurationExtraAspect */
/*!@}*/

/*!
 * \defgroup QTestLibPluginIds QTestLibPlugin Ids
 * @{
 */
const char TestRunConfigurationId [] = TEST_RUN_CONFIGURATION_ID;                                                       /*!< Id for Internal::TestRunConfiguration */
const char TestRunConfigurationExtraAspectId [] = TEST_RUN_CONFIGURATION_EXTRA_ASPECT_ID;                               /*!< Id for Internal::TestRunConfigurationExtraAspect */
const char WorkingDirectoryId [] = TEST_RUN_CONFIGURATION_ID ".WorkingDirectory";                                       /*!< Id for working directory path aspect in Internal::TestRunConfiguration */
const char MakeExeId [] = TEST_RUN_CONFIGURATION_ID ".MakeExe";                                                         /*!< Id for path aspect for \c make in Internal::TestRunConfiguration */
const char MakefileId [] = TEST_RUN_CONFIGURATION_ID ".Makefile";                                                       /*!< Id for \c Makefile path aspect in Internal::TestRunConfiguration */
const char TestRunnerId [] = TEST_RUN_CONFIGURATION_ID ".TestRunner";                                                   /*!< Id for test runner path aspect in Internal::TestRunConfiguration */
const char MakeJobNumberId [] = TEST_RUN_CONFIGURATION_ID ".JobNumber";                                                 /*!< Id for \c make job number aspect in Internal::TestRunConfiguration */
const char BaseQMakeQTestLibParserFactoryId [] = "QTestLibPlugin.ParserFactory.QMake";                                  /*!< Id for Internal::BaseQMakeQTestLibParserFactory */
const char BaseForceParserFactoryId [] = "QTestLibPlugin.ParserFactory.User";                                           /*!< Id for Internal::BaseForceParserFactory */
const char LightXmlQTestLibParserFactoryId [] = "QTestLibPlugin.ParserFactory.LightXml";                                /*!< Id for Internal::LightXMLQTestLibParserFactory */
const char PlainTextQTestLibParserFactoryId [] = "QTestLibPlugin.ParserFactory.PlainText";                              /*!< Id for Internal::PlainTextQTestLibParserFactory */
const char XmlQTestLibParserFactoryId [] = "QTestLibPlugin.ParserFactory.Xml";                                          /*!< Id for Internal::XMLQTestLibParserFactory */
const char XUnitXmlQTestLibParserFactoryId [] = "QTestLibPlugin.ParserFactory.XUnitXml";                                /*!< Id for Internal::XUnitXMLQTestLibParserFactory */
const char TestRunGroupId[] = "QTestLibPugin.Group.Run";                                                                /*!< Id for <tt>Run test</tt> sub menu group */
const char TestRunMenuId[] = "QTestLibPlugin.Menu.TestRun";                                                             /*!< Id for <tt>Run tests</tt> sub menu */
const char TestRunActionId[] = "QTestLibPlugin.Action.TestRun";                                                         /*!< Id for <tt>Run tests</tt> action in project pane context menu */
/*!@}*/

/*!@}*/
} // namespace QTestLibPlugin
} // namespace Constants

#endif // QTESTLIBPLUGINCONSTANTS_H

