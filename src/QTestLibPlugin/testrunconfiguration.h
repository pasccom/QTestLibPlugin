#ifndef TESTRUNCONFIGURATION_H
#define TESTRUNCONFIGURATION_H

#include <utils/fileutils.h>

#include <projectexplorer/localapplicationrunconfiguration.h>
#include <projectexplorer/applicationlauncher.h>

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

    QString mWorkingDirectory;
    QStringList mCmdArgs;
private:
    Utils::FileName mAutoMakeExe;
    Utils::FileName mMakeExe;
};

class TestRunConfiguration : public ProjectExplorer::LocalApplicationRunConfiguration
{
    Q_OBJECT
public:
    ~TestRunConfiguration();
    inline QWidget* createConfigurationWidget(void) {return new QWidget();}
    virtual inline QString executable() const {return mData->makeExe();}
    virtual inline ProjectExplorer::ApplicationLauncher::Mode runMode(void) const {return ProjectExplorer::ApplicationLauncher::Gui;}
    inline QString workingDirectory(void) const {return mData->mWorkingDirectory;}
    inline QString commandLineArguments(void) const {return mData->mCmdArgs.join(QLatin1Char(' '));}
private:
    TestRunConfiguration(ProjectExplorer::Target *parent, Core::Id id);

    TestRunConfigurationData* mData;

    friend class QMakeTestRunConfigurationFactory;
};

} // Internal
} // QTestLibPlugin

#endif // TESTRUNCONFIGURATION_H
