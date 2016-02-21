#ifndef TESTRUNCONFIGURATIONEXTRAASPECT_H
#define TESTRUNCONFIGURATIONEXTRAASPECT_H

#include "qtestlibargsparser.h"

#include <projectexplorer/runconfiguration.h>

class QLabel;
class QComboBox;
class QCheckBox;
class QPushButton;
class QSpinBox;

namespace QTestLibPlugin {
namespace Utils {
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
    void updateFormat(int index);
    void updateVerbosity(int index);

    void updateOutFile(bool valid);
    void updateOutFile(void);
    void browseOutFile(void);

    void updateMaxWarnings(int value);
    void updateEventDelay(int value);
    void updateKeyDelay(int value);
    void updateMouseDelay(int value);
private:
    TestRunConfigurationExtraAspect* mAspect;

    QLabel* mFormatLabel;
    QComboBox* mFormatCombo;
    QLabel* mVerbosityLabel;
    QComboBox* mVerbosityCombo;
    QLabel* mOutFileLabel;
    Utils::FileTypeValidatingLineEdit* mOutFileEdit;
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
