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
class QMakeTestRunConfigurationFactory : public ProjectExplorer::RunConfigurationFactory
{
public:
    /*!
     * \brief Constructor
     *
     * Constructs an new run configuration factory instance
     * for QMake projects with desktop target.
     */
    QMakeTestRunConfigurationFactory(void);

    /*!
     * \brief The build targets this factory can create
     *
     * This function returns a list of the build targets this factory can create.
     *
     * \note A more extensive documentation may be available in Qt Creator Developper documentation
     *
     * \param target The target of the future run configuration.
     * \return A list of the build targets this factory can create.
     * \sa canCreate()
     */
    QList<ProjectExplorer::RunConfigurationCreationInfo> availableCreators(ProjectExplorer::Target *target) const override;

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
};

} // Internal
} // QTestLibPlugin

#endif // QMAKETESTRUNCONFIGURATIONFACTORY_H
