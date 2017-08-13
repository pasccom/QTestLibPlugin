/* Copyright 2015-2016 Pascal COMBES <pascom@orange.fr>
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

#include <qtestlibplugin_global.h>

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

/*!
 * \mainpage QTestLib plugin: Qt Creator plugin to simplify testing.
 *
 * \section features Features
 * Here is a list of the current features of the plugin (see also \ref future for further devopments):
 *  \li Allows to parse plain text (<tt>txt</tt>) QTestLib output
 *  \li Allows to parse XML (<tt>xml</tt>) QTestLib output
 *  \li Allows to parse Light XML (<tt>lightxml</tt>) QTestLib output
 *  \li Allows to parse XUinit XML (<tt>xunitxml</tt>) QTestLib output
 *  \li Check beforehand if the parser may parse the test
 *  \li Model for a QTestLib test
 *  \li Filters to filter the test model (by message type: \c Pass, \c Fail, ...)
 *  \li Merge models for multiple tests
 *  \li Display the models in a QTreeView
 *  \li Allows to run <tt>make check</tt> for projects supporting it.
 *  \li Easy test environment and arguments configuration
 *
 * \section future Planned developments
 * Here are some ideas I plan to implement later:
 *  \li Replace QTreeView by a specially designed view as QTreeView is not suitable
 * for lengthy test messages.
 *  \li Parse the message contents (\c QVERIFY, \c QCOMPARE, ...)
 *  \li Complete information of the model by parsing <tt>*.cpp</tt> test files.
 *  \li Allow to easily select test cases and data row.
 *  \li Have parser read in QTestLib output files (and not only <tt>stdout</tt>).
 *  \li Add parsers and models for other test libraries (gtest, cppunit)
 *
 * If you have any other feature you will be interested in, please let me know.
 * I will be pleased to develop it if I think it is a must have.
 *
 * If you want to implement extension, also tell me please. Admittedly you
 * can do what you desire with the code (under the constraints stated in the
 * section \ref license below), but this will avoid double work.
 *
 * \section compat Compatibity table
 *
 *  | QTestLibPlugin | Qt Creator | Linux | Windows | Mac OS |
 *  |:---------------|:-----------|:-----:|:-------:|:------:|
 *  | 0.0.1          | 3.6.0      |  `Y`  |   `Y`   |  `?`   |
 *  | 0.0.2          | 3.6.0      |  `Y`  |   `Y`   |  `?`   |
 *  | 0.0.3          | 3.6.0      |  `Y`  |   `Y`   |  `?`   |
 *  | 0.0.4          | 3.6.0      |  `Y`  |   `Y`   |  `?`   |
 *  | 0.0.5          | 4.0.0      |  `Y`  |   `Y`   |  `?`   |
 *  | 0.0.6          | 4.2.0      |  `Y`  |   `Y`   |  `?`   |
 *  | 0.0.7          | 4.3.0      |  `Y`  |   `Y`   |  `?`   |
 *
 * A \c Y means that QTestLibPlugin was tested against this Qt Creator version
 * under the given operating system. A \c N means that the version of the plugin
 * cannot work properly for the given operating system. A \c ? means that I
 * currently do not know whether it works.
 * 
 * \section license Licensing information
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
 * along with QTestLibPlugin. If not, see http://www.gnu.org/licenses/
 */

/*!
 * \brief The TestLibPlugin class is the main class of QTestLibPlugin.
 *
 * It implements the plugin functionnality
 * (see Qt Creator developper documentation for details).
 *
 * And also connects and declare the slots required to manage
 * the <tt>"Run tests"</tt> actions and sub menu.
 *
 * It proposes some tests for the functionnalities of the factories
 * and the <tt>"Run tests"</tt> actions.
 */
class TestLibPlugin : public ExtensionSystem::IPlugin
{
#ifndef DOXYGEN
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "QTestLibPlugin.json")
#endif
public:
    /*!
     * \brief Constructor
     *
     * Allocates the model and initializes private members.
     */
    TestLibPlugin();
    /*!
     * \brief Destructor
     *
     * Does not do anything presently.
     */
    ~TestLibPlugin();

    /*!
     * \brief Plugin initialisation method
     *
     * This function is in charge of registering objects in Qt Creator object pool.
     * It creates:
     *  \li The output pane
     *  \li The parser factories
     *  \li The run configuration factories
     *  \li The menu items.
     *  \li Loads the settings.
     *  \li Install the plugin translator.
     *
     * \note This function of this method is extensively described in Qt Creator developper documentation.
     * \param arguments Arguments passed to Qt Creator through the command line.
     * \param errorString If there is an error and if the pointer is not empty,
     * then it is set to a description of the problem.
     * \return \c true if the plugin initialisation suceeded, \c false otherwise.
     * \sa extensionsInitialized()
     */
    bool initialize(const QStringList &arguments, QString *errorString) override;
    /*!
     * \brief Listener for dependant plugin initialisation.
     *
     * This function is currently unused (as this plugin cannot have dependencies).
     * \note This function of this method is extensively described in Qt Creator developper documentation.
     * \sa initialize()
     */
    void extensionsInitialized(void) override;
    /*!
     * \brief Listener for shutdown
     *
     * Only saves the settings.
     * \note This function of this method is extensively described in Qt Creator developper documentation.
     * \return \c SynchronousShutdown in all cases.
     */
    ShutdownFlag aboutToShutdown(void) override;
#ifdef BUILD_TESTS
    /*!
     * \brief Creates tests instances
     *
     * This method creates instances of test classes.
     * \note This function of this method is extensively described in Qt Creator developper documentation.
     * \return A list of instances of test classes.
     */
    QList<QObject *> createTestObjects(void) const override;
#endif
private slots:
    /*!
     * \brief Handler for project openning
     *
     * This slot is intended to be called when a project is openned.
     * It is used for the management of run test sub menus.
     * \param project The newly-opened project.
     * \sa handleProjectClose()
     */
    void handleProjectOpen(ProjectExplorer::Project* project);
    /*!
     * \brief Handler for project closing.
     *
     * This slot is intended to be called when a project is closed.
     * It is used for the management of run test sub menus.
     * \param project The closed project.
     * \sa handleProjectOpen()
     */
    void handleProjectClose(ProjectExplorer::Project* project);
    /*!
     * \brief Handler for current project change.
     *
     * This slot is intended to be called when a the current project changes.
     * It is used for the management of run test sub menus.
     * \param project The new current project.
     */
    void handleCurrentProjectTreeChange(ProjectExplorer::Project* project);

    /*!
     * \brief Handler for active target change.
     *
     * This slot is intended to be called when
     * the active target of an open project is changed.
     * It is used for the management of run test sub menus.
     * \param target The new active target.
     * \param clean Whether old connections should be cleaned.
     */
    void handleActiveTargetChange(ProjectExplorer::Target* target, bool clean = true);

    /*!
     * \brief Handler for run configuration creation.
     *
     * This slot in intended to be called when a new run configuration
     * is added to the active target of an open project.
     * It is used for the management of run test sub menus.
     * \param runConfig The new run configuration.
     * \sa handleDeleteRunConfiguration()
     */
    void handleNewRunConfiguration(ProjectExplorer::RunConfiguration* runConfig);
    /*!
     * \brief Handler for run configuration deletion.
     *
     * This slot in intended to be called when a run configuration
     * is removed from the active target of an open project.
     * It is used for the management of run test sub menus.
     * \param runConfig The removed run configuration.
     * \sa handleNewRunConfiguration()
     */
    void handleDeleteRunConfiguration(ProjectExplorer::RunConfiguration* runConfig);

    /*!
     * \brief Run the tests of the selected project.
     *
     * This slots starts the run configuration to test the selected project.
     * It is expected to be called only when such a run configuration exists.
     * It is currently trigerred the project pane context menu action.
     */
    void runTest(void);
private:
    TestSuiteModel* mModel;                         /*!< The internal model gathering all tests */
    TestOutputPane *mOutputPane;                    /*!< The ouput pane */ // TODO useful?
    Core::ActionContainer* mRunTestsMenu;           /*!< The <tt>"Run tests"</tt> sub menu of <tt>"Build"</tt> menu */
    QAction* mRunTestsAction;                       /*!< The <tt>"Run tests"</tt> action for project pane context menu */
    ProjectExplorer::Project* mTreeCurrentProject;  /*!< The selected project in project tree */
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // QTESTLIBPLUGIN_H

