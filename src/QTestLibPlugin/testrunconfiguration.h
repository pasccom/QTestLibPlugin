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


namespace ProjectExplorer {
    class Kit;
    class ToolChain;
}

namespace QTestLibPlugin {
namespace Widgets {
    class FileTypeValidatingLineEdit;
}

namespace Internal {

class TestRunConfigurationData : public QObject
{
    Q_OBJECT
public:
    TestRunConfigurationData(QObject* parent = NULL);

    inline Utils::FileName makeExe(void) const {return mMakeExe.isNull() ? mAutoMakeExe : mMakeExe;}
    inline bool usesDefaultMakeExe(void) const {return mMakeExe.isNull();}
    inline void useDefaultMakeExe(void) {mMakeExe = Utils::FileName();}
    inline void setAutoMakeExe(const Utils::FileName& makeExe) {mAutoMakeExe = makeExe;}
    inline void setMakeExe(const QString& path) {setMakeExe(Utils::FileName::fromUserInput(path));}
    inline void setMakeExe(const Utils::FileName& makeExe) {mMakeExe = (mAutoMakeExe == makeExe ? Utils::FileName() : makeExe);}

    inline Utils::FileName makefile(void) const {return mMakefile.isNull() ? mAutoMakefile : mMakefile;}
    inline bool usesDefaultMakefile(void) const {return mMakefile.isNull();}
    inline void useDefaultMakefile(void) {mMakefile = Utils::FileName();}
    inline void setAutoMakefile(const Utils::FileName& path) {mAutoMakefile = path;}
    inline void setMakefile(const QString& path) {setMakefile(Utils::FileName::fromUserInput(path));}
    inline void setMakefile(const Utils::FileName& makefile) {mMakefile = (mAutoMakefile == makefile  ? Utils::FileName() : makefile);}

    QVariantMap toMap(QVariantMap& map) const;
    bool fromMap(const QVariantMap& map);

    QString toMakeFilePath(const QString& path) const;

    int jobNumber;
    QString testRunner;
    Utils::FileName workingDirectory;
    void setTargetToolChain(unsigned char newToolChain);
    inline unsigned char targetToolChain(void) const {return mTargetToolChain;}
signals:
    void targetToolChainChanged(unsigned char newToolChain);
private:
    unsigned char mTargetToolChain;
    Utils::FileName mAutoMakeExe;
    Utils::FileName mMakeExe;
    Utils::FileName mAutoMakefile;
    Utils::FileName mMakefile;
};

class TestRunConfigurationWidget : public QWidget
{
    Q_OBJECT
public:
    TestRunConfigurationWidget(TestRunConfigurationData* data, Utils::MacroExpander *macroExpander, QWidget* parent = NULL);
protected:
    void showEvent(QShowEvent *se);
private slots:
    void updateWorkingDirectory(bool valid);
    void updateWorkingDirectory(void);
    void browseWorkingDirectory(void);

    void updateMakefile(bool valid);
    void updateMakefile(void);
    void autoDetectMakefile(void);
    void browseMakefile(void);

    void updateMakeExe(bool valid);
    void updateMakeExe(void);
    void autoDetectMakeExe(void);
    void browseMakeExe(void);

    void updateTestRunner(bool valid);
    void updateTestRunner(void);
    void browseTestRunner(void);

    void updateJubNumber(int jobNumber);

    void handleTargetToolChainChange(unsigned char targetToolChain);
private:
    TestRunConfigurationData* mData;

    QLabel* mWorkingDirectoryLabel;
    Widgets::FileTypeValidatingLineEdit* mWorkingDirectoryEdit;
    QPushButton* mWorkingDirectoryButton;
    QLabel* mMakefileLabel;
    Widgets::FileTypeValidatingLineEdit* mMakefileEdit;
    QPushButton* mMakefileDetectButton;
    QPushButton* mMakefileBrowseButton;
    QLabel* mMakeExeLabel;
    Widgets::FileTypeValidatingLineEdit* mMakeExeEdit;
    QPushButton* mMakeExeDetectButton;
    QPushButton* mMakeExeBrowseButton;
    QLabel* mTestRunnerLabel;
    Widgets::FileTypeValidatingLineEdit* mTestRunnerEdit;
    QPushButton* mTestRunnerButton;
    QLabel* mJobsLabel;
    QSpinBox* mJobsSpin;

};

class TestRunConfiguration : public ProjectExplorer::LocalApplicationRunConfiguration
{
    Q_OBJECT
public:
    inline QWidget* createConfigurationWidget(void) {return new TestRunConfigurationWidget(mData, macroExpander());}

    virtual inline ProjectExplorer::ApplicationLauncher::Mode runMode(void) const {return ProjectExplorer::ApplicationLauncher::Gui;}
    virtual QString executable() const;
    virtual QString workingDirectory(void) const;
    virtual QString commandLineArguments(void) const;

    void setMakefile(const Utils::FileName& makefile);
    inline Utils::FileName makefile(void) const {return mData->makefile();}

    QVariantMap toMap(void) const;
    bool fromMap(const QVariantMap& map);
private slots:
    void handleTargetKitChange(void);
private:
    TestRunConfiguration(ProjectExplorer::Target *parent, Core::Id id);

    TestRunConfigurationData* mData;

    friend class QMakeTestRunConfigurationFactory;
};

} // Internal
} // QTestLibPlugin

#endif // TESTRUNCONFIGURATION_H
