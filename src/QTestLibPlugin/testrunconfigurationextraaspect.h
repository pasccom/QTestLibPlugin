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
    class FileNameValidatingLineEdit;
}

namespace QTestLibPlugin {
namespace Internal {

class TestRunConfigurationExtraAspect;

class TestRunConfigWidget : public ProjectExplorer::RunConfigWidget
{
    Q_OBJECT
public:
    TestRunConfigWidget(TestRunConfigurationExtraAspect* aspect);
    inline QString displayName() const {return tr("Test arguments");}
private:
    TestRunConfigurationExtraAspect* mAspect;

    QLabel* mFormatLabel;
    QComboBox* mFormatCombo;
    QLabel* mVerbosityLabel;
    QComboBox* mVerbosityCombo;
    QLabel* mOutFileLabel;
    Utils::FileNameValidatingLineEdit* mOutFileEdit;
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

    inline void toMap(QVariantMap& map) const {Q_UNUSED(map);}
    inline void fromMap(const QVariantMap& map) {Q_UNUSED(map);}
private:
    QTestLibArgsParser *mTestArgsParser;

    friend class TestRunConfigWidget;
};

} // Internal
} // QTestLibPlugin

#endif // TESTRUNCONFIGURATIONEXTRAASPECT_H
