#ifndef TESTRUNCONFIGURATION_H
#define TESTRUNCONFIGURATION_H

#include "qtestlibargsparser.h"

#include <utils/fileutils.h>

#include <projectexplorer/localapplicationrunconfiguration.h>
#include <projectexplorer/applicationlauncher.h>

#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;

namespace ProjectExplorer {
    class Kit;
}

namespace QTestLibPlugin {
namespace Internal {

class QTestLibArgsParser;

class TestRunConfigurationData
{
public:
    TestRunConfigurationData(ProjectExplorer::Target* target);

    inline QString makeExe(void) const {return mMakeExe.isNull() ? mAutoMakeExe.toString() : mMakeExe.toString();}
    inline bool usesDefaultMakeExe(void) const {return mMakeExe.isNull();}
    inline void useDefaultMakeExe(void) {mMakeExe = Utils::FileName();}
    void setMakeExe(const QString& path);

    QString commandLineArguments(void) const;

    int jobNumber;
    QString testRunner;
    QString workingDirectory;
    QTestLibArgsParser testArgsParser;
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
    QLineEdit* mWorkingDirectoryEdit;
    QPushButton* mWorkingDirectoryButton;
    QLabel* mMakeExeLabel;
    QLineEdit* mMakeExeEdit;
    QPushButton* mMakeExeDetectButton;
    QPushButton* mMakeExeBrowseButton;
    QLabel* mTestRunnerLabel;
    QLineEdit* mTestRunnerEdit;
    QPushButton* mTestRunnerButton;
    /*QLabel* mOutputFileLabel;
    QLineEdit* mOutputFileEdit;
    QPushButton* mOutputFileButton;*/

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
    inline QString commandLineArguments(void) const {return mData->commandLineArguments();}
private:
    TestRunConfiguration(ProjectExplorer::Target *parent, Core::Id id);

    TestRunConfigurationData* mData;

    friend class QMakeTestRunConfigurationFactory;
};

} // Internal
} // QTestLibPlugin

#endif // TESTRUNCONFIGURATION_H
