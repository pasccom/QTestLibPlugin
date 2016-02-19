#ifndef TESTRUNCONFIGURATION_H
#define TESTRUNCONFIGURATION_H

#include "qtestlibargsparser.h"

#include <utils/fileutils.h>

#include <projectexplorer/localapplicationrunconfiguration.h>
#include <projectexplorer/applicationlauncher.h>

#include <QWidget>

class QLabel;
class QPushButton;
class QSpinBox;

namespace Utils {
    class FileNameValidatingLineEdit;
}

namespace ProjectExplorer {
    class Kit;
}

namespace QTestLibPlugin {
namespace Internal {

class TestRunConfigurationData
{
public:
    TestRunConfigurationData(ProjectExplorer::Target* target);

    inline QString makeExe(void) const {return mMakeExe.isNull() ? mAutoMakeExe.toString() : mMakeExe.toString();}
    inline bool usesDefaultMakeExe(void) const {return mMakeExe.isNull();}
    inline void useDefaultMakeExe(void) {mMakeExe = Utils::FileName();}
    void setMakeExe(const QString& path);

    QStringList commandLineArguments(void) const;

    int jobNumber;
    QString testRunner;
    QString workingDirectory;
private:
    Utils::FileName mAutoMakeExe;
    Utils::FileName mMakeExe;
};

class TestRunConfigurationWidget : public QWidget
{
    Q_OBJECT
public:
    TestRunConfigurationWidget(TestRunConfigurationData* data, QWidget* parent = NULL);
private:
    TestRunConfigurationData* mData;

    QLabel* mWorkingDirectoryLabel;
    Utils::FileNameValidatingLineEdit* mWorkingDirectoryEdit;
    QPushButton* mWorkingDirectoryButton;
    QLabel* mMakeExeLabel;
    Utils::FileNameValidatingLineEdit* mMakeExeEdit;
    QPushButton* mMakeExeDetectButton;
    QPushButton* mMakeExeBrowseButton;
    QLabel* mTestRunnerLabel;
    Utils::FileNameValidatingLineEdit* mTestRunnerEdit;
    QPushButton* mTestRunnerButton;
    QLabel* mJobsLabel;
    QSpinBox* mJobsSpin;

};

class TestRunConfiguration : public ProjectExplorer::LocalApplicationRunConfiguration
{
    Q_OBJECT
public:
    ~TestRunConfiguration();
    inline QWidget* createConfigurationWidget(void) {return new TestRunConfigurationWidget(mData);}
    virtual inline QString executable() const {return mData->makeExe();}
    virtual inline ProjectExplorer::ApplicationLauncher::Mode runMode(void) const {return ProjectExplorer::ApplicationLauncher::Gui;}
    inline QString workingDirectory(void) const {return mData->workingDirectory;}
    QString commandLineArguments(void) const;
private:
    TestRunConfiguration(ProjectExplorer::Target *parent, Core::Id id);

    TestRunConfigurationData* mData;

    friend class QMakeTestRunConfigurationFactory;
};

} // Internal
} // QTestLibPlugin

#endif // TESTRUNCONFIGURATION_H
