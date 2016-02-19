#ifndef TESTRUNCONFIGURATION_H
#define TESTRUNCONFIGURATION_H

#include "qtestlibargsparser.h"

#include <utils/fileutils.h>
namespace QtcUtils = Utils;

#include <projectexplorer/localapplicationrunconfiguration.h>
#include <projectexplorer/applicationlauncher.h>

#include <QWidget>

class QLabel;
class QPushButton;
class QSpinBox;


namespace ProjectExplorer {
    class Kit;
}

namespace QTestLibPlugin {
namespace Utils {
    class FileTypeValidatingLineEdit;
}

namespace Internal {

class TestRunConfigurationData
{
public:
    TestRunConfigurationData(ProjectExplorer::Target* target);

    inline QString makeExe(void) const {return mMakeExe.isNull() ? mAutoMakeExe.toString() : mMakeExe.toString();}
    inline bool usesDefaultMakeExe(void) const {return mMakeExe.isNull();}
    inline void useDefaultMakeExe(void) {mMakeExe = QtcUtils::FileName();}
    void setMakeExe(const QString& path);

    QStringList commandLineArguments(void) const;

    int jobNumber;
    QString testRunner;
    QString workingDirectory;
private:
    QtcUtils::FileName mAutoMakeExe;
    QtcUtils::FileName mMakeExe;
};

class TestRunConfigurationWidget : public QWidget
{
    Q_OBJECT
public:
    TestRunConfigurationWidget(TestRunConfigurationData* data, QWidget* parent = NULL);
private slots:
    void updateWorkingDirectory(bool valid);
    void updateWorkingDirectory(void);
    void browseWorkingDirectory(void);

    void updateMakeExe(bool valid);
    void updateMakeExe(void);
    void autoDetectMakeExe(void);
    void browseMakeExe(void);

    void updateTestRunner(bool valid);
    void updateTestRunner(void);
    void browseTestRunner(void);
private:
    TestRunConfigurationData* mData;

    QLabel* mWorkingDirectoryLabel;
    Utils::FileTypeValidatingLineEdit* mWorkingDirectoryEdit;
    QPushButton* mWorkingDirectoryButton;
    QLabel* mMakeExeLabel;
    Utils::FileTypeValidatingLineEdit* mMakeExeEdit;
    QPushButton* mMakeExeDetectButton;
    QPushButton* mMakeExeBrowseButton;
    QLabel* mTestRunnerLabel;
    Utils::FileTypeValidatingLineEdit* mTestRunnerEdit;
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
