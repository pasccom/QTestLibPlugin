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

#ifndef TESTRUNCONFIGURATION_H
#define TESTRUNCONFIGURATION_H

#include "qtestlibargsparser.h"
#include "qtestlibpluginconstants.h"

#include <utils/fileutils.h>

#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/applicationlauncher.h>

#include <QWidget>

class QLabel;
class QPushButton;
class QSpinBox;


namespace ProjectExplorer {
    class Kit;
    class ToolChain;
}

namespace QTestLibPlugin {
namespace Widgets {
    class FileTypeValidatingLineEdit;
}

namespace Internal {

/*!
 * \brief The TestRunConfigurationData class holds the data of TestRunConfiguration
 *
 * This class holds the data associated with a TestRunConfiguration
 * provides some methods to accessing them consistently.
 * It is also accesssed by TestRunConfigurationWidget when the data is modified.
 *
 * \sa TestRunConfiguration and TestRunConfigurationWidget
 */
class TestRunConfigurationData : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * Initializes the members with default values.
     * \param parent The parent object (which should be the associated TestRunConfiguration)
     */
    TestRunConfigurationData(QObject* parent = NULL);

    /*!
     * \brief Path to \c make
     *
     * Returns the path to \c make executable.
     * \return The path to \c make executable
     */
    inline Utils::FilePath makeExe(void) const {return mMakeExe.isEmpty() ? mAutoMakeExe : mMakeExe;}
    /*!
     * \brief Whether default \c make path is used
     *
     * Returns \c true when the auto-detected path to \c make executable is used.
     * \return Whether auto-detected \c make path is used.
     * \sa useDefaultMakeExe()
     */
    inline bool usesDefaultMakeExe(void) const {return mMakeExe.isEmpty();}
    /*!
     * \brief Use default \c make path
     *
     * Ensures auto-detected path to \c make is used.
     *
     * \sa usesDefaultMakeExe()
     */
    inline void useDefaultMakeExe(void) {mMakeExe = Utils::FilePath();}
    /*!
     * \brief Set auto-detected \c make path
     *
     * Sets the auto-detected path to \c make executable.
     * \param makeExe The auto-detected path to \c make executable
     * \sa setMakeExe(const Utils::FilePath&), setMakeExe(const QString&)
     */
    inline void setAutoMakeExe(const Utils::FilePath& makeExe) {mAutoMakeExe = makeExe;}
    /*!
     * \brief \copybrief setMakeExe(const Utils::FilePath&)
     *
     * \overload
     * \param path The used-defined path to \c make executable (converted with <tt>Utils::FilePath::fromUserInput()</tt>)
     */
    inline void setMakeExe(const QString& path) {setMakeExe(Utils::FilePath::fromUserInput(path));}
    /*!
     * \brief Set \c make path
     *
     * Set used-defined path to \c make executable.
     * \param makeExe The used-defined path to \c make executable
     * \sa setAutoMakeExe()
     */
    inline void setMakeExe(const Utils::FilePath& makeExe) {mMakeExe = (mAutoMakeExe == makeExe ? Utils::FilePath() : makeExe);}

    /*!
     * \brief Path to \c Makefile
     *
     * Returns the path to the \c Makefile.
     * \return The path to the \c Makefile
     */
    inline Utils::FilePath makefile(void) const {return mMakefile.isEmpty() ? mAutoMakefile : mMakefile;}
    /*!
     * \brief Whether default \c Makefile path is used
     *
     * Returns \c true when the auto-detected path to \c Makefile is used.
     * \return Whether auto-detected \c Makefile path is used.
     * \sa useDefaultMakefile()
     */
    inline bool usesDefaultMakefile(void) const {return mMakefile.isEmpty();}
    /*!
     * \brief Use default \c Makefile path
     *
     * Ensures auto-detected path to \c Makefile is used.
     *
     * \sa usesDefaultMakefile()
     */
    inline void useDefaultMakefile(void) {mMakefile = Utils::FilePath();}
    /*!
     * \brief Set auto-detected \c Makefile path
     *
     * Sets the auto-detected path to \c Makefile.
     * \param path The auto-detected path to \c Makefile
     * \sa setMakefile(const Utils::FilePath&), setMakefile(const QString&)
     */
    inline void setAutoMakefile(const Utils::FilePath& path) {mAutoMakefile = path;}
    /*!
     * \brief \copybrief setMakefile(const Utils::FilePath&)
     *
     * \overload
     * \param path The user-defined path to \c Makefile (converted to <tt>Utils::FilePath::fromUserInput()</tt>)
     */
    inline void setMakefile(const QString& path) {setMakefile(Utils::FilePath::fromUserInput(path));}
    /*!
     * \brief Set \c Makefile path
     *
     * Set used-defined path to \c Makefile.
     * \param makefile The user-defined path to \c Makefile
     * \sa setAutoMakefile()
     */
    inline void setMakefile(const Utils::FilePath& makefile) {mMakefile = (mAutoMakefile == makefile  ? Utils::FilePath() : makefile);}

    /*!
     * \brief Conversion to map
     *
     * Converts the instance into a QVariantMap by initializing the fields of the given map.
     * \param map The map to be initialized with the contents of the instance
     * \return The initialzed map.
     * \sa fromMap()
     */
    QVariantMap toMap(QVariantMap& map) const;
    /*!
     * \brief Conversion from map
     *
     * Initializes the instance with the given QVariantMap.
     * \param map The map containing the data of the instance
     * \return \c true when the initialization of the instance was sucessful, \c false otherwise
     * \sa toMap()
     */
    bool fromMap(const QVariantMap& map);

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

    /*!
     * \brief Converts to path used by \c make
     *
     * Converts the given path to a path which can be used by \c make.
     * This is required to support MSYS based compilation on Windows platform.
     * \param path The system path to convert
     * \return The same path to be used by \c make
     */
    QString toMakeFilePath(const QString& path) const;

    int jobNumber;                      /*!< The number of jobs \c make should use when running <tt>make check</tt>. */
    QString testRunner;                 /*!< The path to the test runner. */
    Utils::FilePath workingDirectory;   /*!< The path to working directory. */
signals:
    /*!
     * \internal
     * \brief Target toolchain changed
     *
     * This signal is emitted whenever the toolchain used for target compilation is changed.
     * \param newToolChain Internal ID for the new toolchain.
     * \sa setTargetToolChain(), targetToolChain()
     */
    void targetToolChainChanged(unsigned char newToolChain);
private:
    unsigned char mTargetToolChain;     /*!< The internal ID of the current toolchain. */
    Utils::FilePath mAutoMakeExe;       /*!< The auto-detected path to \c make executable. */
    Utils::FilePath mMakeExe;           /*!< The user-defined path to \c make executable. */
    Utils::FilePath mAutoMakefile;      /*!< The auto-detected path to \c Makefile. */
    Utils::FilePath mMakefile;          /*!< The user-defined path to \c Makefile. */
};

/*!
 * \brief The TestRunConfigurationWidget class provides a form widget to edit a TestRunConfiguration
 *
 * This class describes a form widget which allows to easily configure a TestRunConfiguration.
 * More precisely it allow the user to configure
 *  \li The working directory path with a Widgets::FileTypeValidatingLineEdit supporting macro variables
 *  \li The \c Makefile path with a Widgets::FileTypeValidatingLineEdit supporting macro variables
 *  \li The path to \c make executable with a Widgets::FileTypeValidatingLineEdit supporting macro variables
 *  \li The test runner path with a Widgets::FileTypeValidatingLineEdit supporting macro variables
 *  \li The number of jobs used by \c make with a QSpinBox. It is limited to the number of processor cores.
 *
 * To store the data, it uses a TestRunConfigurationData instance.
 *
 * \sa TestRunConfiguration, TestRunConfigurationData
 */
class TestRunConfigurationWidget : public QWidget
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * Constructs a new instance of this widget
     * which uses the given TestRunConfigurationData
     * to stores its internal data.
     * \param data The TestRunConfigurationData holding the internal data
     * \param macroExpander Qt Creator macro expander to expand macro variables
     * \param parent The parent widget
     */
    TestRunConfigurationWidget(TestRunConfigurationData* data, Utils::MacroExpander *macroExpander, QWidget* parent = NULL);
protected:
    /*!
     * \brief On show
     *
     * As a reimplementation of <tt>QWidget::showEvent()</tt>,
     * this function is called when the widget is shown.
     * It ensures all the fields are correctly initialized.
     * \param se The show event.
     */
    void showEvent(QShowEvent *se) override;
private slots:
    /*!
     * \brief Updates the validity of the working directory path
     *
     * This slot is called when the validity of the working directory path changes in \ref mWorkingDirectoryEdit.
     * If the current working directory path is valid, it is saved. Otherwise the slot does nothing.
     * \param valid Whether the current working directory path is valid in \ref mWorkingDirectoryEdit
     */
    void updateWorkingDirectory(bool valid);
    /*!
     * \brief Updates the working directory path
     *
     * This slot is called when the user finishes editing the working directory path in \ref mWorkingDirectoryEdit.
     * It saves the path, if it is valid or restores the previous one if it is invalid.
     */
    void updateWorkingDirectory(void);
    /*!
     * \brief Open file browser for working directory path.
     *
     * This slot is called when the user clicks \ref mWorkingDirectoryButton,
     * to browse the folders for a working directory.
     */
    void browseWorkingDirectory(void);

    /*!
     * \brief Updates the validity of the \c Makefile path
     *
     * This slot is called when the validity of the \c Makefile path changes in \ref mMakefileEdit.
     * If the current \c Makefile path is valid, it is saved. Otherwise the slot does nothing.
     * \param valid Whether the current \c Makefile path is valid in \ref mMakefileEdit
     */
    void updateMakefile(bool valid);
    /*!
     * \brief Updates the \c Makefile path
     *
     * This slot is called when the user finishes editing the \c Makefile path in \ref mMakefileEdit.
     * It saves the path, if it is valid or restores the previous one if it is invalid.
     */
    void updateMakefile(void);
    /*!
     * \brief Auto-detect \c Makefile path.
     *
     * This slot is called when the user clicks \ref mMakefileDetectButton,
     * for Qt Creator the use the auto-detected path to \c Makefile.
     */
    void autoDetectMakefile(void);
    /*!
     * \brief Open file browser for \c Makefile path.
     *
     * This slot is called when the user clicks \ref mMakefileBrowseButton,
     * to browse the files for a \c Makefile.
     */
    void browseMakefile(void);

    /*!
     * \brief Updates the validity of \c make path
     *
     * This slot is called when the validity of the path to \c make executable changes in \ref mMakeExeEdit.
     * If the current \c make path is valid, it is saved. Otherwise the slot does nothing.
     * \param valid Whether the current path to \c make executable is valid in \ref mMakeExeEdit
     */
    void updateMakeExe(bool valid);
    /*!
     * \brief Updates \c make path
     *
     * This slot is called when the user finishes editing the path to \c make executable in \ref mMakeExeEdit.
     * It saves the path, if it is valid or restores the previous one if it is invalid.
     */
    void updateMakeExe(void);
    /*!
     * \brief Auto-detect \c make path.
     *
     * This slot is called when the user clicks \ref mMakeExeDetectButton,
     * for Qt Creator the use the auto-detected path to \c make executable.
     */
    void autoDetectMakeExe(void);
    /*!
     * \brief Open file browser for \c make path.
     *
     * This slot is called when the user clicks \ref mMakeExeBrowseButton,
     * to browse the files for \c make.
     */
    void browseMakeExe(void);

    /*!
     * \brief Updates the validity of the test runner path
     *
     * This slot is called when the validity of the test runner path changes in \ref mTestRunnerEdit.
     * If the current test runner path is valid, it is saved. Otherwise the slot does nothing.
     * \param valid Whether the current test runner path is valid in \ref mTestRunnerEdit
     */
    void updateTestRunner(bool valid);
    /*!
     * \brief Updates the test runner path
     *
     * This slot is called when the user finishes editing the test runner path in \ref mTestRunnerEdit.
     * It saves the path, if it is valid or restores the previous one if it is invalid.
     */
    void updateTestRunner(void);
    /*!
     * \brief Open file browser for test runner path.
     *
     * This slot is called when the user clicks \ref mTestRunnerButton,
     * to browse the files for a test runner.
     */
    void browseTestRunner(void);

    /*!
     * \brief Updates the job number
     *
     * This slot is called when the user changes the value in \ref mJobsSpin.
     * \param jobNumber The current value in \ref mJobsSpin
     */
    void updateJubNumber(int jobNumber);

    /*!
     * \brief Handles a change of target toolchain
     *
     * This slot is called when the toolchain used to comile targets changes.
     * It activates or deactivates fields depending on the toolchain type.
     * \param targetToolChain The internal ID of the new toolchain
     */
    void handleTargetToolChainChange(unsigned char targetToolChain);
private:
    TestRunConfigurationData* mData;                            /*!< TestRunConfigurationData to hold internal data. */

    QLabel* mWorkingDirectoryLabel;                             /*!< The QLabel associated with \ref mWorkingDirectoryEdit. */
    Widgets::FileTypeValidatingLineEdit* mWorkingDirectoryEdit; /*!< A Widgets::FileTypeValidatingLineEdit for the user to enter a path to the working directory. */
    QPushButton* mWorkingDirectoryButton;                       /*!< The QPushButton to start browsing for the working directory */
    QLabel* mMakefileLabel;                                     /*!< The QLabel associated with \ref mMakefileEdit. */
    Widgets::FileTypeValidatingLineEdit* mMakefileEdit;         /*!< A Widgets::FileTypeValidatingLineEdit for the user to enter a path to the \c Makefile. */
    QPushButton* mMakefileDetectButton;                         /*!< A QPushbutton to use auto-detected path to \c Makefile */
    QPushButton* mMakefileBrowseButton;                         /*!< The QPushButton to start browsing for the \c Makefile path */
    QLabel* mMakeExeLabel;                                      /*!< The QLabel associated with \ref mMakeExeEdit. */
    Widgets::FileTypeValidatingLineEdit* mMakeExeEdit;          /*!< A Widgets::FileTypeValidatingLineEdit for the user to enter a path to \c make executable. */
    QPushButton* mMakeExeDetectButton;                          /*!< The QPushButton to use auto-detected path to \c make executable */
    QPushButton* mMakeExeBrowseButton;                          /*!< The QPushButton to start browsing for \c make executable */
    QLabel* mTestRunnerLabel;                                   /*!< The QLabel associated with \ref mTestRunnerEdit. */
    Widgets::FileTypeValidatingLineEdit* mTestRunnerEdit;       /*!< A Widgets::FileTypeValidatingLineEdit for the user to enter a path to the test runner. */
    QPushButton* mTestRunnerButton;                             /*!< The QPushButton to start browsing for the test runner */
    QLabel* mJobsLabel;                                         /*!< The QLabel associated with \ref mJobsSpin. */
    QSpinBox* mJobsSpin;                                        /*!< A QSpinBox for easy configuration of the number of jobs used by \c make */

};

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
    TestRunConfiguration(ProjectExplorer::Target *parent, Core::Id id = Constants::TestRunConfigurationId);

    /*!
     * \brief Creates a configuration widget
     *
     * Creates an instance of the configuration widget TestRunConfigurationWidget
     * associated to the current instance.
     * \return The newly allocated instance of the configuration widget.
     */
    inline QWidget* createConfigurationWidget(void) override {return new TestRunConfigurationWidget(mData, macroExpander());}

    /*!
     * \brief Path to the working directory
     *
     * Returns the path to the working directory (see TestRunConfigurationData::workingDirectory).
     * \return The path to the working directory.
     */
    virtual QString workingDirectory(void) const;
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
     * Returns the current path to \c Makefile (see TestRunConfigurationData::makefile())
     * \return
     */
    inline Utils::FilePath makefile(void) const {return mData->makefile();}

    /*!
     * \brief Conversion to map
     *
     * Converts the instance into a QVariantMap by initializing the fields of the given map.
     * \return The map initialized with the contents of the instance
     * \sa fromMap()
     */
    QVariantMap toMap(void) const override;
    /*!
     * \brief Conversion from map
     *
     * Initializes the instance with the given QVariantMap.
     * \param map The map containing the data of the instance
     * \return \c true when the initialization of the instance was sucessful, \c false otherwise
     * \sa toMap()
     */
    bool fromMap(const QVariantMap& map) override;
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
    TestRunConfigurationData* mData;    /*!< TestRunConfigurationData to hold internal data. */

    friend class QMakeTestRunConfigurationFactory;
};

} // Internal
} // QTestLibPlugin

#endif // TESTRUNCONFIGURATION_H
