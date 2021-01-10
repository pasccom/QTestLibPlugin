/* Copyright 2016-2020 Pascal COMBES <pascom@orange.fr>
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

#ifndef TESTRUNCONFIGURATION_H
#define TESTRUNCONFIGURATION_H

#include "qtestlibargsparser.h"
#include "qtestlibpluginconstants.h"

#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/applicationlauncher.h>

namespace ProjectExplorer {
    class Kit;
    class ToolChain;
}

namespace QTestLibPlugin {
namespace Internal {

/*!
 * \brief The TestRunConfiguration class describles a test run
 *
 * When an instance of this class is added to a target, Qt Creator will propose to the user
 * to run the tests of the associated project. This can be done with the classical <tt>"Build and run kit selector"</tt>
 * from Qt Creator or with the new <tt>"Run tests"</tt> sub menu and action.
 *
 * This run configuration stores its internal data in TestRunConfigurationData
 * and can be easily modified using a TestRunConfigurationWidget. It uses a run
 * configuration extra aspect TestRunConfigurationExtraAspect to set the test command line arguments.
 *
 * \sa TestRunConfigurationData, TestRunConfigurationWidget, TestRunConfigurationExtraAspect
 */
class TestRunConfiguration : public ProjectExplorer::RunConfiguration
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * Creates a new instance with parent target.
     * \param parent The parent target
     * \param id The id for this run configuration
     * \sa initialize()
     */
    TestRunConfiguration(ProjectExplorer::Target *parent, Utils::Id id = Constants::TestRunConfigurationId);

    /*!
     * \brief Path to the working directory
     *
     * Returns the path to the working directory (see TestRunConfigurationData::workingDirectory).
     * \return The path to the working directory.
     */
    virtual Utils::FilePath workingDirectory(void) const;
    /*!
     * \brief Command-line arguments
     *
     * Returns command-line arguments to be passed to \c make.
     * \return The command-line arguments to be passed to \c make
     */
    virtual QString commandLineArguments(void) const;

    virtual ProjectExplorer::Runnable runnable(void) const override;

    /*!
     * \brief Set the \c Makefile path
     *
     * Set the auto-detected path to the \c Makefile, using TestRunConfigurationData::setAutoMakefile()
     * \param makefile The auto-detected path to the \c Makefile
     * \sa makefile()
     */
    void setMakefile(const Utils::FilePath& makefile);
    /*!
     * \brief Path to \c Makefile
     *
     * Returns the current path to \c Makefile.
     * \return
     */
    Utils::FilePath makefile(void) const;
private slots:
    /*!
     * \brief Handles a change of kit
     *
     * This slot is called when the kit used to compile the target is changed.
     * It updates the internal toolchain ID in TestRunConfigurationData
     */
    void handleTargetKitChange(void);
    /*!
     * \brief Updates the run configuration
     *
     * This function updates the path to the project \c Makefile
     * in the run configuration.
     */
    bool update(void);
private:
    /*!
     * \internal
     * \brief set the target toolchain type.
     *
     * Sets the type of the toolchain used for target compilation.
     * \param newToolChain Internal toolchain ID
     * \sa targetToolChain(), targetToolChainChanged()
     */
    void setTargetToolChain(unsigned char newToolChain);
    /*!
     * \internal
     * \brief Target toolchain type
     *
     * Retuns the type of the toolchain used to compile the target.
     * \return Internal toolchain ID
     * \sa setTargetToolChain(), targetToolChainChanged()
     */
    inline unsigned char targetToolChain(void) const {return mTargetToolChain;}

    unsigned char mTargetToolChain;     /*!< The internal ID of the current toolchain. */

    friend class QMakeTestRunConfigurationFactory;
};

} // Internal
} // QTestLibPlugin

#endif // TESTRUNCONFIGURATION_H
