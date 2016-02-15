#ifndef TESTRUNCONFIGURATION_H
#define TESTRUNCONFIGURATION_H

#include <projectexplorer/localapplicationrunconfiguration.h>
#include <projectexplorer/applicationlauncher.h>

namespace QTestLibPlugin {
namespace Internal {

class TestRunConfiguration : public ProjectExplorer::LocalApplicationRunConfiguration
{
    Q_OBJECT
public:
    ~TestRunConfiguration();
    inline QWidget* createConfigurationWidget(void) {return new QWidget();}
    virtual inline QString executable() const {return mMakeExe;}
    virtual inline ProjectExplorer::ApplicationLauncher::Mode runMode(void) const {return ProjectExplorer::ApplicationLauncher::Console;}
    inline QString workingDirectory(void) const {return mWorkingDirectory;}
    inline QString commandLineArguments(void) const {return mCmdArgs.join(QLatin1Char(' '));}
private:
    TestRunConfiguration(ProjectExplorer::Target *parent, Core::Id id);

    QString mMakeExe;
    QString mWorkingDirectory;
    QStringList mCmdArgs;

    friend class QMakeTestRunConfigurationFactory;
};

} // Internal
} // QTestLibPlugin

#endif // TESTRUNCONFIGURATION_H
