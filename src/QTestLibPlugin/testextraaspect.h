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

#ifndef TESTRUNCONFIGURATIONEXTRAASPECT_H
#define TESTRUNCONFIGURATIONEXTRAASPECT_H

#include "qtestlibargsparser.h"

#include <projectexplorer/runconfiguration.h>

class QLabel;
class QComboBox;
class QCheckBox;
class QPushButton;
class QSpinBox;

namespace Utils {
    class DetailsWidget;
}

namespace QTestLibPlugin {
namespace Widgets {
    class FileTypeValidatingLineEdit;
}

namespace Internal {

class TestExtraAspect;

/*!
 * \brief The TestRunConfigWidget class defines a form widget to configure a TestRunConfigurationExtraAspect
 *
 * The instances of this class are form widgets allowing
 * to easily configure the command line arguments to a \c qtestlib test
 * uisng a TestRunConfigurationExtraAspect.
 * Namely, it provides a form to configure easily
 *  \li The test output format with a QComboBox with the available formats
 *  \li The test verbosity with a QComboBox with the available verbosities
 *  \li The path to a log file with a Widgets::FileTypeValidatingLineEdit supporting macro variables
 *  \li The maximum number of messages through a QSpinBox
 *  \li The event delay through a QSpinBox
 *  \li The key delay through a QSpinBox
 *  \li The mouse delay through a QSpinBox
 *  \li The crash handler through a QCheckBox
 *
 * \sa TestRunConfigurationExtraAspect
 */
class TestRunConfigWidget : public QWidget
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * Constructs a new instance of this class to configure the given exta aspect.
     * The parent of the widget is set internally by Qt Creator.
     * \param aspect The extra aspect to configure with this form widget
     */
    TestRunConfigWidget(TestExtraAspect* aspect);
signals:
    /*!
     * \brief Test arguments changed
     *
     * This signal is emitted when test arguments are changed.
     * \param newArgs The new test arguments.
     */
    void testArgumentsChanged(const QString& newArgs);
private slots:
    /*
     * \brief Handles change of test arguments
     *
     * This slot should be called when test arguments are changed.
     * It updates the state of the widgets.
     */
    //void handleTestArgumentsChange(void);
    /*!
     * \brief Updates the summary
     *
     * This slot should be called whenever the summary needs to be updated.
     */
    void updateSummary(void);

    /*!
     * \brief Updates the format
     *
     * This slot is called when the test output format is modified in \ref mFormatCombo.
     * \param index The current index in \ref mFormatCombo
     */
    void updateFormat(int index);
    /*!
     * \brief Updates the verbosity
     *
     * This slot is called when the verbosity is modified in \ref mVerbosityCombo.
     * \param index The current index in \ref mVerbosityCombo
     */
    void updateVerbosity(int index);

    /*!
     * \brief Updates the validity of the log file path
     *
     * This slot is called when the validity of the log file path changes in \ref mOutFileEdit.
     * If the current log file path is valid, it is saved. Otherwise the slot does nothing.
     * \param valid Whether the current log file path is valid in \ref mOutFileEdit
     */
    void updateOutFile(bool valid);
    /*!
     * \brief Updates the log file path
     *
     * This slot is called when the user finishes editing the log file path in \ref mOutFileEdit.
     * It saves the path, if it is valid or restores the previous one if it is invalid.
     */
    void updateOutFile(void);
    /*!
     * \brief Open file browser for log file path.
     *
     * This slot is called when the user clicks \ref mOutFileButton,
     * to browse the files for a log file.
     */
    void browseOutFile(void);

    /*!
     * \brief Updates the maximum number of warnings
     *
     * This slot is called when the user changes the value in \ref mWarningSpin.
     * \param value The current value in \ref mWarningSpin
     */
    void updateMaxWarnings(int value);
    /*!
     * \brief Updates the state of maximum number of warnings
     *
     * This slot is called when the user enables or disables \ref mWarningSpin through \ref mWarningCheck.
     * \param enabled Whether \ref mWarningSpin should be enabled
     */
    void updateMaxWarnings(bool enabled);
    /*!
     * \brief Updates the event delay
     *
     * This slot is called when the user changes the value in \ref mEventDelaySpin.
     * \param value The current value in \ref mEventDelaySpin
     */
    void updateEventDelay(int value);
    /*!
     * \brief Updates the state of event delay
     *
     * This slot is called when the user enables or disables \ref mEventDelaySpin through \ref mEventDelayCheck.
     * \param enabled Whether \ref mEventDelaySpin should be enabled
     */
    void updateEventDelay(bool enabled);
    /*!
     * \brief Updates the key delay
     *
     * This slot is called when the user changes the value in \ref mKeyDelaySpin.
     * \param value The current value in \ref mKeyDelaySpin.
     */
    void updateKeyDelay(int value);
    /*!
     * \brief Updates the state of key delay
     *
     * This slot is called when the user enables or disables \ref mKeyDelaySpin through \ref mKeyDelayCheck.
     * \param enabled Whether \ref mKeyDelaySpin should be enabled
     */
    void updateKeyDelay(bool enabled);
    /*!
     * \brief Updates the mouse delay
     *
     * This slot is called when the user changes the value in \ref mMouseDelaySpin.
     * \param value The current value in \ref mMouseDelaySpin.
     */
    void updateMouseDelay(int value);
    /*!
     * \brief Updates the state of event delay
     *
     * This slot is called when the user enables or disables \ref mMouseDelaySpin through \ref mMouseDelayCheck.
     * \param enabled Whether \ref mMouseDelaySpin should be enabled
     */
    void updateMouseDelay(bool enabled);
private:
    TestExtraAspect* mAspect;           /*!< The TestRunConfigurationExtraAspect this widget configures */

    Utils::DetailsWidget* mDetailWidget;                /*!< The main Detail widgets */
    QLabel* mFormatLabel;                               /*!< The QLabel associated with \ref mFormatCombo */
    QComboBox* mFormatCombo;                            /*!< A QCombobox for easy setting of test output format */
    QLabel* mVerbosityLabel;                            /*!< The QLabel associated with \ref mVerbosityCombo */
    QComboBox* mVerbosityCombo;                         /*!< A QCombobox for easy setting of test verbosity */
    QLabel* mOutFileLabel;                              /*!< The QLabel for assciated with \ref mOutFileEdit */
    Widgets::FileTypeValidatingLineEdit* mOutFileEdit;  /*!< A Widgets::FileTypeValidatingLineEdit for the user to enter a path to the log file */
    QPushButton* mOutFileButton;                        /*!< The QPushButton to start browsing for the log file path */
    QCheckBox* mWarningCheck;                           /*!< The QCheckBox to enable \ref mWarningSpin */
    QSpinBox* mWarningSpin;                             /*!< A QSpinBox for easy configuration of the maximum number of warnings */
    QCheckBox* mEventDelayCheck;                        /*!< The QCheckBox to enable \ref mEventDelaySpin */
    QSpinBox* mEventDelaySpin;                          /*!< A QSpinBox for easy configuration of the event delay */
    QCheckBox* mKeyDelayCheck;                          /*!< The QCheckBox to enable \ref mKeyDelaySpin */
    QSpinBox* mKeyDelaySpin;                            /*!< A QSpinBox for easy configuration of the key delay */
    QCheckBox* mMouseDelayCheck;                        /*!< The QCheckBox to enable \ref mMouseDelaySpin */
    QSpinBox* mMouseDelaySpin;                          /*!< A QSpinBox for easy configuration of the mouse delay */

};

/*!
 * \brief The TestRunConfigurationExtraAspect class describes the command line arguments of a \c qtestlib test.
 *
 * The instances of this class describe the command line arguments of a \c qtestlib test.
 * It uses a QTestLibArgsParser to store the internal data.
 *
 * \sa TestRunConfigWidget, QTestLibArgsParser
 */
class TestExtraAspect : public ProjectExplorer::ProjectConfigurationAspect
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * Constructs a new instance of this class and allocates the internal QTestLibArgsParser
     * to store data.
     * \param argParser An \c testlib argument parser which will be copied in this instance.
     * \sa create()
     */
    TestExtraAspect(QTestLibArgsParser* argParser = nullptr);
    /*!
     * \brief Destructor
     *
     * Destructs an instance of this class by desallocating the internal QTestLibArgsParser.
     */
    ~TestExtraAspect() override;

    /*!
     * \brief Set the output format
     *
     * Sets the format of the output.
     *
     * \param format The output format
     * \sa outputFormat(), TestLibArgsParser::setOutputFormat()
     */
    inline void setOutputFormat(QTestLibArgsParser::TestOutputFormat format) {mTestArgsParser->setOutputFormat(format);}
    /*!
     * \brief The output format
     *
     * Returns the format of the output.
     *
     * \return The output format
     * \sa setOutputFormat(), QTestLibArgsParser::outputFormat()
     */
    inline QTestLibArgsParser::TestOutputFormat outputFormat(void) const {return mTestArgsParser->outputFormat();}

    /*!
     * \brief Set the verbosity
     *
     * Sets the verbosity of the test output.
     *
     * \param verbosity The verbosity of the test output.
     * \sa verbosity(), QTestLibArgsParser::setVerbosity()
     */
    inline void setVerbosity(QTestLibArgsParser::TestVerbosity verbosity) {mTestArgsParser->setVerbosity(verbosity);}
    /*!
     * \brief The verbosity
     *
     * Returns the verbosity of the test output.
     *
     * \return The verbosity of the test output.
     * \sa setVerbosity(), QTestLibArgsParser::verbosity()
     */
    inline QTestLibArgsParser::TestVerbosity verbosity(void) const {return mTestArgsParser->verbosity();}

    /*!
     * \brief Test command-line arguments
     *
     * Returns the command-line arguments which should be passed to the test
     * to ensure the expected behaviour.
     * \return The list of command-line arguments which should be passed to the test
     */
    inline QStringList commandLineArguments(void) const {return mTestArgsParser->toStringList();}

    /*!
     * \brief Whether this class is useful
     *
     * Tells whether this class should be added in the list of extra aspects
     * of the given run configuration by inspecting its attributes.
     * \param runConfiguration The run configuration to test.
     * \return \c true it this class is useful for the given run configuration, \c false otherwise.
     */
    static bool isUseful(ProjectExplorer::RunConfiguration* runConfiguration);
protected:
    /*!
     * \brief Conversion to map
     *
     * Converts the instance into a QVariantMap by initializing the fields of the given map.
     * \param map The map to be initialized with the contents of the instance
     * \sa fromMap()
     */
    inline void toMap(QVariantMap& map) const override {mTestArgsParser->toMap(map);}
    /*!
     * \brief Conversion from map
     *
     * Initializes the instance with the given QVariantMap.
     * \param map The map containing the data of the instance
     * \sa toMap()
     */
    inline void fromMap(const QVariantMap& map) override {mTestArgsParser->fromMap(map);}
/*signals:
    void testArgumentsChanged(void);
private slots:
    void handleTestArgumentsChange(const QString& newArgs);
    void handleArgumentsChange(const QString& newArgs);*/
private:
    QTestLibArgsParser *mTestArgsParser;    /*!< The internal QTestLibArgsParser used to store data */
    QString mOtherArgs;                     /*!< Arguments which are not managed by this extra aspect */
    friend class TestRunConfigWidget;
};

} // Internal
} // QTestLibPlugin

#endif // TESTRUNCONFIGURATIONEXTRAASPECT_H
