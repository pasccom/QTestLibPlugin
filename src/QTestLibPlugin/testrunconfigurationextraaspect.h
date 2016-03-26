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

class TestRunConfigurationExtraAspect;

class TestRunConfigWidget : public ProjectExplorer::RunConfigWidget
{
    Q_OBJECT
public:
    TestRunConfigWidget(TestRunConfigurationExtraAspect* aspect);
    inline QString displayName() const {return tr("Test arguments");}
private slots:
    void updateSummary(void);
    void updateFormat(int index);
    void updateVerbosity(int index);

    void updateOutFile(bool valid);
    void updateOutFile(void);
    void browseOutFile(void);

    void updateMaxWarnings(int value);
    void updateMaxWarnings(bool enabled);
    void updateEventDelay(int value);
    void updateEventDelay(bool enabled);
    void updateKeyDelay(int value);
    void updateKeyDelay(bool enabled);
    void updateMouseDelay(int value);
    void updateMouseDelay(bool enabled);
private:
    TestRunConfigurationExtraAspect* mAspect;

    Utils::DetailsWidget* mDetailWidget;
    QLabel* mFormatLabel;
    QComboBox* mFormatCombo;
    QLabel* mVerbosityLabel;
    QComboBox* mVerbosityCombo;
    QLabel* mOutFileLabel;
    Widgets::FileTypeValidatingLineEdit* mOutFileEdit;
    QPushButton* mOutFileButton;
    QCheckBox* mWarningCheck;
    QSpinBox* mWarningSpin;
    QCheckBox* mEventDelayCheck;
    QSpinBox* mEventDelaySpin;
    QCheckBox* mKeyDelayCheck;
    QSpinBox* mKeyDelaySpin;
    QCheckBox* mMouseDelayCheck;
    QSpinBox* mMouseDelaySpin;

};

class TestRunConfigurationExtraAspect : public ProjectExplorer::IRunConfigurationAspect
{
    Q_OBJECT
public:
    TestRunConfigurationExtraAspect(ProjectExplorer::RunConfiguration *parent);
    ~TestRunConfigurationExtraAspect();

    inline TestRunConfigurationExtraAspect* create(ProjectExplorer::RunConfiguration* parent) const {return new TestRunConfigurationExtraAspect(parent);}
    inline ProjectExplorer::RunConfigWidget* createConfigurationWidget(void) {return new TestRunConfigWidget(this);}

    inline QStringList commandLineArguments(void) const {return mTestArgsParser->toStringList();}

    inline void toMap(QVariantMap& map) const {mTestArgsParser->toMap(map);}
    inline void fromMap(const QVariantMap& map) {mTestArgsParser->fromMap(map);}
private:
    QTestLibArgsParser *mTestArgsParser;

    friend class TestRunConfigWidget;
};

} // Internal
} // QTestLibPlugin

#endif // TESTRUNCONFIGURATIONEXTRAASPECT_H
