/* Copyright 2016 Pascal COMBES <pascom@orange.fr>
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

/*!
 * \brief The QMakeTestRunConfigurationFactory class creates TestRunConfiguration for supported \c qMake projects.
 *
 * This class is in charge of managing TestRunConfiguration for supported \c qMake projects.
 * For this is uses the result of the parsing of the project file
 * and also the features of the kit of the active target.
 *
 * It handles only desktop targets and is useful for \c subdirs \c qMake projects
 * with a sub project with the \c CONFIG \c testcase option.
 *
 * \sa TestRunConfiguration
 */
class QMakeTestRunConfigurationFactory : public ProjectExplorer::IRunConfigurationFactory
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * The constructor currently does nothing.
     * \param parent The parent object.
     */
    QMakeTestRunConfigurationFactory(QObject *parent = NULL);
    /*!
     * \brief The run configuration IDs this factory can create
     *
     * This function returns a list of the the IDs this factory can create.
     * Currentlty, it returns only the TestRunConfiguration ID for desktop targets.
     *
     * \note A more extensive documentation may be available in Qt Creator Developper documentation
     *
     * \param target The target of the future run configuration.
     * \param mode The creation mode.
     * \return A lis of the run configuration IDs this factory can create.
     * \sa canCreate()
     */
    QList<Core::Id> availableCreationIds(ProjectExplorer::Target *target, CreationMode mode = UserCreate) const override;
    /*!
     * \brief The display name corresponding to the given ID
     *
     * Expects the given ID to be the TestRunConfiguration ID
     * and returns <tt>"make check"</tt>.
     *
     * \note A more extensive documentation may be available in Qt Creator Developper documentation
     *
     * \param id A run configuration ID (expects a TestRunConfiguration ID)
     * \return <tt>"make check"</tt> if the ID is a TestRunConfiguration ID.
     */
    QString displayNameForId(Core::Id id) const override;

    /*!
     * \brief Whether the factory handles the given target
     *
     * Returns \c true if the factory handles the given target,
     * i.e. if the associated project is a \c qMake project and
     * if the associated kit is a desktop kit.
     * \param target A target
     * \return \c true if the factory can handle the target, \c false otherwise.
     * \sa isUseful(), canCreate(), canRestore(), canClone()
     */
    static bool canHandle(ProjectExplorer::Target* target);
    /*!
     * \brief Whether the project is ready for examination
     *
     * Returns \c true when the given project is ready for examination by isUseful().
     * \param project A project
     * \return \c true if the given project is ready for examination, \c false otherwise.
     */
    static bool isReady(ProjectExplorer::Project* project);
    /*!
     * \brief Whether TestRunConfiguration is useful for the given project
     *
     * Returns \c true when a TestRunConfiguration is useful for the given project,
     * i.e when it canHandle() the associated target,
     * the project is a \c subdirs \c qMake project
     * and at least one sub project has \c CONFIG \c testcase.
     * \param project A project
     * \return \c true when a TestRunConfiguration is usefull for the given project.
     * \sa canHandle(), canCreate(), canRestore(), canClone()
     */
    static bool isUseful(ProjectExplorer::Project* project);

    /*!
     * \brief Whether a run configuration can be created
     *
     * Returns \c true when a run configuration with the given ID can be created.
     * This implementation supports only TestRunConfiguration,
     * the target must be handled,
     * and TestRunConfiguration must be useful for the associated project.
     *
     * \note A more extensive documentation may be available in Qt Creator Developper documentation
     *
     * \param parent A target, which will be the parent of the run configuration
     * \param id The ID of the desired run configuration
     * \return \c true if the factory can create a run configuration.
     * \sa canHandle(), isUseful(), canRestore(), canClone()
     */
    bool canCreate(ProjectExplorer::Target* parent, Core::Id id) const override;
    /*!
     * \brief Whether a run configuration can be restored
     *
     * Returns \c true when a run configuration can be restored from the given data.
     * This implementation supports only TestRunConfiguration,
     * the target must be handled,
     * and TestRunConfiguration must be useful for the associated project.
     *
     * \note A more extensive documentation may be available in Qt Creator Developper documentation
     *
     * \param parent A target, which will be the parent of the run configuration
     * \param map The run configuration data
     * \return \c true if the factory can restore a run configuration from the data.
     * \sa canHandle(), isUseful(), canCreate(), canClone()
     */
    bool canRestore(ProjectExplorer::Target* parent, const QVariantMap &map) const override;
    /*!
     * \brief Whether a run configuration can be cloned
     *
     * Returns \c true when the given run configuration can be cloned.
     * This implementation supports only TestRunConfiguration,
     * the target must be handled,
     * and TestRunConfiguration must be useful for the associated project.
     *
     * \note A more extensive documentation may be available in Qt Creator Developper documentation
     *
     * \param parent A target, which will be the parent of the run configuration
     * \param product A run configuration
     * \return \c true if the factory can create a run configuration.
     * \sa canHandle(), isUseful(), canCreate(), canRestore(), clone()
     */
    bool canClone(ProjectExplorer::Target* parent, ProjectExplorer::RunConfiguration *product) const override;

    /*!
     * \brief Clones a run configuration
     *
     * This method can be used after checking canClone() returns \c true.
     * It clones the run configuration on the new target by copying its internal data.
     *
     * \note A more extensive documentation may be available in Qt Creator Developper documentation
     *
     * \param parent A target (must be target the factory canHandle())
     * \param product A run configuration (must be a TestRunConfiguration)
     * \return A clone of the given run configuration associated with the given target,
     * or \c NULL when unsupported.
     * \sa canClone()
     */
    ProjectExplorer::RunConfiguration* clone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *product) override;
private:
    /*!
     * \brief Really creates a run configuration.
     *
     * This method is called automatically by ProjectExplorer
     * to create an new run configuration with the given ID.
     *
     * \note A more extensive documentation may be available in Qt Creator Developper documentation
     *
     * \param target A target (must be target the factory canHandle())
     * \param id An ID (must be a TestRunConfiguration ID)
     * \return A new run configuration associated with the given target,
     * or \c NULL when unsupported.
     * \sa canCreate()
     */
    ProjectExplorer::RunConfiguration* doCreate(ProjectExplorer::Target* target, Core::Id id) override;
    /*!
     * \brief Really restores a run configuration.
     *
     * This method is called automatically by ProjectExplorer
     * to restore a run configuration from the given data.
     *
     * \note A more extensive documentation may be available in Qt Creator Developper documentation
     *
     * \param target A target (must be target the factory canHandle())
     * \param map Data to restore the run configuration
     * \return A new run configuration initialized with the given data,
     * or \c NULL when unsupported.
     * \sa canRestore()
     */
    ProjectExplorer::RunConfiguration* doRestore(ProjectExplorer::Target* target, const QVariantMap& map) override;
};

} // Internal
} // QTestLibPlugin

#endif // QMAKETESTRUNCONFIGURATIONFACTORY_H
