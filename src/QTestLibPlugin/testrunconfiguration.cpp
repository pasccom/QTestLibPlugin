#include "testrunconfiguration.h"

#include "testrunconfigurationextraaspect.h"
#include "qtestlibpluginconstants.h"

#include "Utils/filetypevalidatinglineedit.h"

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
        QtcUtils::Environment env = target->activeBuildConfiguration()->environment();
        ProjectExplorer::ToolChain *toolChain = ProjectExplorer::ToolChainKitInformation::toolChain(target->kit());
        mAutoMakeExe = QtcUtils::FileName::fromString(toolChain->makeCommand(env));
    }
}

void TestRunConfigurationData::setMakeExe(const QString& path)
{
    QtcUtils::FileName makeExe = QtcUtils::FileName::fromUserInput(path);

    mMakeExe = (mAutoMakeExe == makeExe ? QtcUtils::FileName() : makeExe);
}

QStringList TestRunConfigurationData::commandLineArguments(void) const
{
    QStringList cmdArgs;

    if (jobNumber > 1)
        cmdArgs << QString(QLatin1String("-j%1")).arg(jobNumber);
    cmdArgs << QLatin1String("check");
    if (!testRunner.isEmpty())
        cmdArgs << QString(QLatin1String("TESTRUNNER=\"%1\"")).arg(testRunner);

    return cmdArgs;
}

QVariantMap TestRunConfigurationData::toMap(QVariantMap& map) const
{
    if (workingDirectory != QLatin1String("."))
        map.insert(Constants::WorkingDirectoryKey, workingDirectory);
    if (!mMakeExe.isNull())
        map.insert(Constants::MakeExeKey, mMakeExe.toString());
    if (!testRunner.isEmpty())
        map.insert(Constants::TestRunnerKey, testRunner);
    if (jobNumber != 1)
        map.insert(Constants::MakeJobNumberKey, jobNumber);
    return map;
}

bool TestRunConfigurationData::fromMap(const QVariantMap& map)
{
    workingDirectory = map.value(Constants::WorkingDirectoryKey, QLatin1String(".")).toString();
    mMakeExe = QtcUtils::FileName::fromString(map.value(Constants::MakeExeKey, QString()).toString());
    testRunner = map.value(Constants::TestRunnerKey, QString()).toString();
    jobNumber = map.value(Constants::MakeJobNumberKey, 1).toInt();

    return true;
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

QVariantMap TestRunConfiguration::toMap(void) const
{
    QVariantMap map(ProjectExplorer::RunConfiguration::toMap());
    map = mData->toMap(map);

    qDebug() << __func__ << map;
    return map;
}

bool TestRunConfiguration::fromMap(const QVariantMap& map)
{
    qDebug() << __func__ << map;

    return mData->fromMap(map) && ProjectExplorer::RunConfiguration::fromMap(map);
}


QString TestRunConfiguration::commandLineArguments(void) const
{
    QStringList cmdArgs = mData->commandLineArguments();
    Q_ASSERT(extraAspect<TestRunConfigurationExtraAspect>() != NULL);
    QStringList testCmdArgs = extraAspect<TestRunConfigurationExtraAspect>()->commandLineArguments();
    cmdArgs << QString(QLatin1String("TESTARGS=\"%1\"")).arg(testCmdArgs.join(QLatin1Char(' ')));

    qDebug() << "Command line arguments:" << cmdArgs;

    return cmdArgs.join(QLatin1Char(' '));
}

TestRunConfigurationWidget::TestRunConfigurationWidget(TestRunConfigurationData* data, QWidget* parent)
    : QWidget(parent), mData(data)
{
    mWorkingDirectoryEdit = new Utils::FileTypeValidatingLineEdit(this);
    mWorkingDirectoryEdit->setAcceptDirectories(true);
    mWorkingDirectoryEdit->setText(mData->workingDirectory);
    mWorkingDirectoryLabel = new QLabel(tr("Working directory:"), this);
    mWorkingDirectoryLabel->setBuddy(mWorkingDirectoryEdit);
    mWorkingDirectoryButton = new QPushButton(tr("Browse..."), this);
    mMakeExeEdit = new Utils::FileTypeValidatingLineEdit(this);
    mMakeExeEdit->setRequireExecutable(true);
    mMakeExeEdit->setText(mData->makeExe());
    if (QtcUtils::HostOsInfo::isWindowsHost())
        mMakeExeEdit->setRequiredExtensions(QStringList() << QLatin1String("exe"));
    else
        mMakeExeEdit->setRequiredExtensions(QStringList());
    mMakeExeLabel = new QLabel(tr("Path to \"make\":"), this);
    mMakeExeLabel->setBuddy(mMakeExeEdit);
    mMakeExeDetectButton = new QPushButton(tr("Auto-detect"), this);
    mMakeExeBrowseButton = new QPushButton(tr("Browse..."), this);
    mTestRunnerEdit = new Utils::FileTypeValidatingLineEdit(this);
    mTestRunnerEdit->setAcceptEmpty(true);
    mTestRunnerEdit->setRequireExecutable(true);
    mTestRunnerEdit->setText(mData->testRunner);
    mTestRunnerLabel = new QLabel(tr("Test runner:"), this);
    mTestRunnerLabel->setBuddy(mTestRunnerLabel);
    mTestRunnerButton = new QPushButton(tr("Browse..."), this);
    mJobsSpin = new QSpinBox(this);
    mJobsSpin->setRange(1, QThread::idealThreadCount());
    mJobsSpin->setValue(mData->jobNumber);
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

    connect(mWorkingDirectoryEdit, SIGNAL(validChanged(bool)),
            this, SLOT(updateWorkingDirectory(bool)));
    connect(mWorkingDirectoryEdit, SIGNAL(editingFinished()),
            this, SLOT(updateWorkingDirectory()));
    connect(mWorkingDirectoryButton, SIGNAL(released()),
            this, SLOT(browseWorkingDirectory()));

    connect(mMakeExeEdit, SIGNAL(validChanged(bool)),
            this, SLOT(updateMakeExe(bool)));
    connect(mMakeExeEdit, SIGNAL(editingFinished()),
            this, SLOT(updateMakeExe()));
    connect(mMakeExeDetectButton, SIGNAL(released()),
            this, SLOT(autoDetectMakeExe()));
    connect(mMakeExeBrowseButton, SIGNAL(released()),
            this, SLOT(browseMakeExe()));

    connect(mTestRunnerEdit, SIGNAL(validChanged(bool)),
            this, SLOT(updateTestRunner(bool)));
    connect(mTestRunnerEdit, SIGNAL(editingFinished()),
            this, SLOT(updateTestRunner()));
    connect(mTestRunnerButton, SIGNAL(released()),
            this, SLOT(browseTestRunner()));

    connect(mJobsSpin, SIGNAL(valueChanged(int)),
            this, SLOT(updateJubNumber(int)));
}

void TestRunConfigurationWidget::updateWorkingDirectory(bool valid)
{
    if (valid) {
        qDebug() << "Saving WD:" << mWorkingDirectoryEdit->text();
        mData->workingDirectory = mWorkingDirectoryEdit->text();
    }
}

void TestRunConfigurationWidget::updateWorkingDirectory(void)
{
    if (mWorkingDirectoryEdit->isValid())
        mData->workingDirectory = mWorkingDirectoryEdit->text();
    else
        mWorkingDirectoryEdit->setText(mData->workingDirectory);
}

void TestRunConfigurationWidget::browseWorkingDirectory(void)
{
    QString wd = QFileDialog::getExistingDirectory(this, tr("Choose working directory"), mData->workingDirectory);

    if (!wd.isNull())
        mData->workingDirectory = wd;
    mWorkingDirectoryEdit->setText(mData->workingDirectory);
}

void TestRunConfigurationWidget::updateMakeExe(bool valid)
{
    if (valid)
        mData->setMakeExe(mMakeExeEdit->text());
}

void TestRunConfigurationWidget::updateMakeExe(void)
{
    if (mMakeExeEdit->isValid())
        mData->setMakeExe(mMakeExeEdit->text());
    else
        mMakeExeEdit->setText(mData->makeExe());
}

void TestRunConfigurationWidget::autoDetectMakeExe(void)
{
    mData->useDefaultMakeExe();
    mMakeExeEdit->setText(mData->makeExe());
}
void TestRunConfigurationWidget::browseMakeExe(void)
{
    QString filter;
    if (QtcUtils::HostOsInfo::isWindowsHost())
        filter = tr("Exécutable files *.exe");

    QString me = QFileDialog::getOpenFileName(this, tr("Choose \"make\""), mData->makeExe(), filter, &filter);

    if (!me.isNull())
        mData->setMakeExe(me);
    mMakeExeEdit->setText(mData->makeExe());
}

void TestRunConfigurationWidget::updateTestRunner(bool valid)
{
    if (valid)
        mData->testRunner = mTestRunnerEdit->text();
}

void TestRunConfigurationWidget::updateTestRunner(void)
{
    if (mTestRunnerEdit->isValid())
        mData->testRunner = mTestRunnerEdit->text();
    else
        mTestRunnerEdit->setText(mData->testRunner);
}

void TestRunConfigurationWidget::browseTestRunner(void)
{
    QString runner = QFileDialog::getOpenFileName(this, tr("Choose test runner"), mData->testRunner);
    if (!runner.isNull())
        mData->testRunner = runner;
    mTestRunnerEdit->setText(mData->testRunner);
}


void TestRunConfigurationWidget::updateJubNumber(int jobNumber)
{
    qDebug() << "Saving job number:" << jobNumber;
    mData->jobNumber = jobNumber;
}

} // Internal
} // QTestLibPlugin

