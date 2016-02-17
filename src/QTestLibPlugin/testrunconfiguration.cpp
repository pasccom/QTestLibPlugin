#include "testrunconfiguration.h"

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

QString TestRunConfigurationData::commandLineArguments(void) const
{
    QStringList cmdArgs;
    QStringList testCmdArgs = testArgsParser.toStringList();

    if (jobNumber > 1)
        cmdArgs << QString(QLatin1String("-j%1")).arg(jobNumber);
    cmdArgs << QLatin1String("check");
    if (!testRunner.isEmpty())
        cmdArgs << QString(QLatin1String("TESTRUNNER=%1")).arg(testRunner);
    if (!testCmdArgs.isEmpty())
        cmdArgs << QString(QLatin1String("TESTARGS=\"%1\"")).arg(testCmdArgs.join(QLatin1Char(' ')));
    qDebug() << cmdArgs;

    return cmdArgs.join(QLatin1Char(' '));
}

TestRunConfiguration::TestRunConfiguration(ProjectExplorer::Target *parent, Core::Id id):
    ProjectExplorer::LocalApplicationRunConfiguration(parent, id)
{
    addExtraAspect(new ProjectExplorer::LocalEnvironmentAspect(this));

    mData = new TestRunConfigurationData(parent);
    setDefaultDisplayName(QLatin1String("make check"));
}

TestRunConfiguration::~TestRunConfiguration()
{
    delete mData;
}

TestRunConfigurationWidget::TestRunConfigurationWidget(TestRunConfigurationData* data, QWidget* parent)
    : QWidget(parent), mData(data)
{
    mWorkingDirectoryEdit = new QLineEdit(this);
    mWorkingDirectoryLabel = new QLabel(tr("Working directory:"), this);
    mWorkingDirectoryLabel->setBuddy(mWorkingDirectoryEdit);
    mWorkingDirectoryButton = new QPushButton(tr("Browse..."), this);
    mMakeExeEdit = new QLineEdit(this);
    mMakeExeLabel = new QLabel(tr("Path to \"make\":"), this);
    mMakeExeLabel->setBuddy(mMakeExeEdit);
    mMakeExeDetectButton = new QPushButton(tr("Auto-detect"), this);
    mMakeExeBrowseButton = new QPushButton(tr("Browse..."), this);
    mTestRunnerEdit = new QLineEdit(this);
    mTestRunnerLabel = new QLabel(tr("Test runner:"), this);
    mTestRunnerLabel->setBuddy(mTestRunnerLabel);
    mTestRunnerButton = new QPushButton(tr("Browse..."), this);
    /*mOutputFileEdit = new QLineEdit(this);
    mOutputFileLabel = new QLabel(tr("Test output file:"), this);
    mOutputFileLabel->setBuddy(mOutputFileEdit);
    mOutputFileButton = new QPushButton(tr("Browse..."), this);*/

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
    /*mainLayout->addWidget(mOutputFileLabel, 3, 0, Qt::AlignLeft);
    mainLayout->addWidget(mOutputFileEdit, 3, 1, 1, 2);
    mainLayout->addWidget(mOutputFileButton, 3, 3, Qt::AlignCenter);*/
    mainLayout->setColumnStretch(1, 1);

    setLayout(mainLayout);
}

} // Internal
} // QTestLibPlugin

