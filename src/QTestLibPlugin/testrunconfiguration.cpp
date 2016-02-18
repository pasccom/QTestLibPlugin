#include "testrunconfiguration.h"

#include "testrunconfigurationextraaspect.h"

#include <utils/filenamevalidatinglineedit.h>

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/localenvironmentaspect.h>
#include <projectexplorer/toolchain.h>
#include <projectexplorer/target.h>

#include <QtWidgets>

namespace QTestLibPlugin {
namespace Internal {

TestRunConfigurationData::TestRunConfigurationData(ProjectExplorer::Target *target)
    :  jobNumber(1), testRunner(), workingDirectory(QLatin1String(".")), mAutoMakeExe(), mMakeExe()
{
    if (target != NULL) {
        Utils::Environment env = target->activeBuildConfiguration()->environment();
        ProjectExplorer::ToolChain *toolChain = ProjectExplorer::ToolChainKitInformation::toolChain(target->kit());
        mAutoMakeExe = Utils::FileName::fromString(toolChain->makeCommand(env));
    }
}

void TestRunConfigurationData::setMakeExe(const QString& path)
{
    Utils::FileName makeExe = Utils::FileName::fromUserInput(path);

    mMakeExe = (mAutoMakeExe == makeExe ? Utils::FileName() : makeExe);
}

QStringList TestRunConfigurationData::commandLineArguments(void) const
{
    QStringList cmdArgs;

    if (jobNumber > 1)
        cmdArgs << QString(QLatin1String("-j%1")).arg(jobNumber);
    cmdArgs << QLatin1String("check");
    if (!testRunner.isEmpty())
        cmdArgs << QString(QLatin1String("TESTRUNNER=%1")).arg(testRunner);
    /*if (!testCmdArgs.isEmpty())
        cmdArgs << QString(QLatin1String("TESTARGS=\"%1\"")).arg(testCmdArgs.join(QLatin1Char(' ')));*/
    qDebug() << cmdArgs;

    return cmdArgs;
}

TestRunConfiguration::TestRunConfiguration(ProjectExplorer::Target *parent, Core::Id id):
    ProjectExplorer::LocalApplicationRunConfiguration(parent, id)
{
    /* TODO ensure this run configuration cannot be run with valgrind...
     * To do this, the code of the Valgrind plugin should be altered:
     * 1.ValgrindRunControlFactory should check the type of the given RunConfiguration (e.g. in canRun())
     * and addAspects() should only add aspects provided bu runnable RunControl factories.
     * 2.Alternatively, ValgrindPlugin, should ensure the extra aspects are added to
     * sensible RunConfiguration and RunConfiguration::addExtraAspects() should be removed. */
    addExtraAspect(new ProjectExplorer::LocalEnvironmentAspect(this));
    addExtraAspect(new TestRunConfigurationExtraAspect(this));

    mData = new TestRunConfigurationData(parent);
    setDefaultDisplayName(QLatin1String("make check"));
}

TestRunConfiguration::~TestRunConfiguration()
{
    delete mData;
}

QString TestRunConfiguration::commandLineArguments(void) const
{
    QStringList cmdArgs = mData->commandLineArguments();
    Q_ASSERT(extraAspect<TestRunConfigurationExtraAspect>() != NULL);
    QStringList testCmdArgs = extraAspect<TestRunConfigurationExtraAspect>()->commandLineArguments();
    cmdArgs << QString(QLatin1String("TESTARGS=\"%1\"")).arg(testCmdArgs.join(QLatin1Char(' ')));

    return cmdArgs.join(QLatin1Char(' '));
}

TestRunConfigurationWidget::TestRunConfigurationWidget(TestRunConfigurationData* data, QWidget* parent)
    : QWidget(parent), mData(data)
{
    mWorkingDirectoryEdit = new Utils::FileNameValidatingLineEdit(this);
    mWorkingDirectoryEdit->setAllowDirectories(true);
    mWorkingDirectoryLabel = new QLabel(tr("Working directory:"), this);
    mWorkingDirectoryLabel->setBuddy(mWorkingDirectoryEdit);
    mWorkingDirectoryButton = new QPushButton(tr("Browse..."), this);
    mMakeExeEdit = new Utils::FileNameValidatingLineEdit(this);
    mMakeExeEdit->setAllowDirectories(false);
    if (Utils::HostOsInfo::isWindowsHost())
        mMakeExeEdit->setRequiredExtensions(QStringList() << QLatin1String("exe"));
    else
        mMakeExeEdit->setRequiredExtensions(QStringList());
    mMakeExeLabel = new QLabel(tr("Path to \"make\":"), this);
    mMakeExeLabel->setBuddy(mMakeExeEdit);
    mMakeExeDetectButton = new QPushButton(tr("Auto-detect"), this);
    mMakeExeBrowseButton = new QPushButton(tr("Browse..."), this);
    mTestRunnerEdit = new Utils::FileNameValidatingLineEdit(this);
    mTestRunnerEdit->setAllowDirectories(false);
    mTestRunnerLabel = new QLabel(tr("Test runner:"), this);
    mTestRunnerLabel->setBuddy(mTestRunnerLabel);
    mTestRunnerButton = new QPushButton(tr("Browse..."), this);
    mJobsSpin = new QSpinBox(this);
    mJobsLabel = new QLabel(tr("Number of jobs (for \"make\"):"), this);
    mJobsLabel->setBuddy(mJobsSpin);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(mWorkingDirectoryLabel, 0, 0, Qt::AlignLeft);
    mainLayout->addWidget(mWorkingDirectoryEdit, 0, 1, 1, 2);
    mainLayout->addWidget(mWorkingDirectoryButton, 0, 3, Qt::AlignCenter);
    mainLayout->addWidget(mMakeExeLabel, 1, 0, Qt::AlignLeft);
    mainLayout->addWidget(mMakeExeEdit, 1, 1);
    mainLayout->addWidget(mMakeExeDetectButton, 1, 2, Qt::AlignCenter);
    mainLayout->addWidget(mMakeExeBrowseButton, 1, 3, Qt::AlignCenter);
    mainLayout->addWidget(mTestRunnerLabel, 2, 0, Qt::AlignLeft);
    mainLayout->addWidget(mTestRunnerEdit, 2, 1, 1, 2);
    mainLayout->addWidget(mTestRunnerButton, 2, 3, Qt::AlignCenter);
    mainLayout->addWidget(mJobsLabel, 3, 0, 1, 3, Qt::AlignLeft);
    mainLayout->addWidget(mJobsSpin, 3, 3, Qt::AlignRight);
    mainLayout->setColumnStretch(1, 1);

    setLayout(mainLayout);
}

} // Internal
} // QTestLibPlugin

