#include "testrunconfiguration.h"

#include "testrunconfigurationextraaspect.h"
#include "qtestlibpluginconstants.h"

#include "Widgets/filetypevalidatinglineedit.h"

#include <coreplugin/variablechooser.h>

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/localenvironmentaspect.h>
#include <projectexplorer/toolchain.h>
#include <projectexplorer/target.h>

#include <QtWidgets>

namespace QTestLibPlugin {
namespace Internal {

TestRunConfigurationData::TestRunConfigurationData(ProjectExplorer::Target *target)
    :  jobNumber(1), testRunner(), mAutoMakeExe(), mMakeExe()
{
    if (target != NULL) {
        workingDirectory = target->activeBuildConfiguration()->buildDirectory();
        Utils::Environment env = target->activeBuildConfiguration()->environment();
        ProjectExplorer::ToolChain *toolChain = ProjectExplorer::ToolChainKitInformation::toolChain(target->kit());
        mAutoMakeExe = Utils::FileName::fromString(toolChain->makeCommand(env));
    }
}

QStringList TestRunConfigurationData::commandLineArguments(void) const
{
    QStringList cmdArgs;
    QString makefilePath = makefile().toString();

    makefilePath.replace(QLatin1Char('\"'), QLatin1String("\\\""));
    if (makefilePath.contains(QLatin1Char(' ')))
        makefilePath = QLatin1Char('\"') + makefilePath + QLatin1Char('\"');

    cmdArgs << QLatin1String("-f") << makefilePath;
    if (jobNumber > 1)
        cmdArgs << QString(QLatin1String("-j%1")).arg(jobNumber);
    cmdArgs << QLatin1String("check");
    if (!testRunner.isEmpty())
        cmdArgs << QString(QLatin1String("TESTRUNNER=\"%1\"")).arg(testRunner);

    return cmdArgs;
}

QVariantMap TestRunConfigurationData::toMap(QVariantMap& map) const
{
    if (!workingDirectory.isNull())
        map.insert(Constants::WorkingDirectoryKey, workingDirectory.toString());
    if (!mMakeExe.isNull())
        map.insert(Constants::MakeExeKey, mMakeExe.toString());
    if (!mMakefile.isNull())
        map.insert(Constants::MakefileKey, mMakefile.toString());
    if (!testRunner.isEmpty())
        map.insert(Constants::TestRunnerKey, testRunner);
    if (jobNumber != 1)
        map.insert(Constants::MakeJobNumberKey, jobNumber);
    return map;
}

bool TestRunConfigurationData::fromMap(const QVariantMap& map)
{
    workingDirectory = Utils::FileName::fromString(map.value(Constants::WorkingDirectoryKey, QString()).toString());
    mMakeExe = Utils::FileName::fromString(map.value(Constants::MakeExeKey, QString()).toString());
    mMakefile = Utils::FileName::fromString(map.value(Constants::MakefileKey, QString()).toString());
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
    setMakefile(Utils::FileName());
    setDefaultDisplayName(QLatin1String("make check"));
}

TestRunConfiguration::~TestRunConfiguration()
{
    delete mData;
}

void TestRunConfiguration::setMakefile(const Utils::FileName& makefile)
{
    qDebug() << __func__ << makefile;

    if (!makefile.isNull())
        mData->setAutoMakefile(makefile);
    else if ((target() != NULL) && (target()->activeBuildConfiguration() != NULL))
        mData->setAutoMakefile(target()->activeBuildConfiguration()->buildDirectory().appendPath("Makefile"));
}

QVariantMap TestRunConfiguration::toMap(void) const
{
    QVariantMap map(ProjectExplorer::RunConfiguration::toMap());
    map = mData->toMap(map);

    return map;
}

bool TestRunConfiguration::fromMap(const QVariantMap& map)
{
    return mData->fromMap(map) && ProjectExplorer::RunConfiguration::fromMap(map);
}

QString TestRunConfiguration::executable() const
{
    if (macroExpander() != NULL)
        return macroExpander()->expand(mData->makeExe().toString());
    return mData->makeExe().toString();
}

QString TestRunConfiguration::workingDirectory(void) const
{
    if (macroExpander() != NULL)
        return macroExpander()->expand(mData->workingDirectory.toString());
    return mData->workingDirectory.toString();
}

QString TestRunConfiguration::commandLineArguments(void) const
{
    QStringList cmdArgs = mData->commandLineArguments();
    Q_ASSERT(extraAspect<TestRunConfigurationExtraAspect>() != NULL);
    QStringList testCmdArgs = extraAspect<TestRunConfigurationExtraAspect>()->commandLineArguments();
    if (!testCmdArgs.isEmpty())
        cmdArgs << QString(QLatin1String("TESTARGS=\"%1\"")).arg(testCmdArgs.join(QLatin1Char(' ')));

    if (macroExpander() != NULL)
        return macroExpander()->expandProcessArgs(cmdArgs.join(QLatin1Char(' ')));
    return cmdArgs.join(QLatin1Char(' '));
}

TestRunConfigurationWidget::TestRunConfigurationWidget(TestRunConfigurationData* data, Utils::MacroExpander* macroExpander, QWidget* parent)
    : QWidget(parent), mData(data)
{
    mWorkingDirectoryEdit = new Widgets::FileTypeValidatingLineEdit(this);
    mWorkingDirectoryEdit->setMacroExpander(macroExpander);
    mWorkingDirectoryEdit->setAcceptDirectories(true);
    mWorkingDirectoryEdit->setText(mData->workingDirectory.toString());
    mWorkingDirectoryLabel = new QLabel(tr("Working directory:"), this);
    mWorkingDirectoryLabel->setBuddy(mWorkingDirectoryEdit);
    mWorkingDirectoryButton = new QPushButton(tr("Browse..."), this);
    mMakefileEdit = new Widgets::FileTypeValidatingLineEdit(this);
    mMakefileEdit->setMacroExpander(macroExpander);
    mMakefileEdit->setText(mData->makefile().toString());
    mMakefileLabel = new QLabel(tr("Makefile:"), this);
    mMakefileLabel->setBuddy(mWorkingDirectoryEdit);
    mMakefileDetectButton = new QPushButton(tr("Auto-detect"), this);
    mMakefileBrowseButton = new QPushButton(tr("Browse..."), this);
    mMakeExeEdit = new Widgets::FileTypeValidatingLineEdit(this);
    mMakeExeEdit->setMacroExpander(macroExpander);
    mMakeExeEdit->setRequireExecutable(true);
    mMakeExeEdit->setText(mData->makeExe().toString());
    if (Utils::HostOsInfo::isWindowsHost())
        mMakeExeEdit->setRequiredExtensions(QStringList() << QLatin1String("exe"));
    else
        mMakeExeEdit->setRequiredExtensions(QStringList());
    mMakeExeLabel = new QLabel(tr("Path to \"make\":"), this);
    mMakeExeLabel->setBuddy(mMakeExeEdit);
    mMakeExeDetectButton = new QPushButton(tr("Auto-detect"), this);
    mMakeExeBrowseButton = new QPushButton(tr("Browse..."), this);
    mTestRunnerEdit = new Widgets::FileTypeValidatingLineEdit(this);
    mTestRunnerEdit->setMacroExpander(macroExpander);
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
    mainLayout->addWidget(mMakefileLabel, 1, 0, Qt::AlignLeft);
    mainLayout->addWidget(mMakefileEdit, 1, 1);
    mainLayout->addWidget(mMakefileDetectButton, 1, 2, Qt::AlignCenter);
    mainLayout->addWidget(mMakefileBrowseButton, 1, 3, Qt::AlignCenter);
    mainLayout->addWidget(mMakeExeLabel, 2, 0, Qt::AlignLeft);
    mainLayout->addWidget(mMakeExeEdit, 2, 1);
    mainLayout->addWidget(mMakeExeDetectButton, 2, 2, Qt::AlignCenter);
    mainLayout->addWidget(mMakeExeBrowseButton, 2, 3, Qt::AlignCenter);
    mainLayout->addWidget(mTestRunnerLabel, 3, 0, Qt::AlignLeft);
    mainLayout->addWidget(mTestRunnerEdit, 3, 1, 1, 2);
    mainLayout->addWidget(mTestRunnerButton, 3, 3, Qt::AlignCenter);
    mainLayout->addWidget(mJobsLabel, 4, 0, 1, 3, Qt::AlignLeft);
    mainLayout->addWidget(mJobsSpin, 4, 3, Qt::AlignRight);
    mainLayout->setColumnStretch(1, 1);

    Core::VariableChooser::addSupportForChildWidgets(this, macroExpander);

    setLayout(mainLayout);

    connect(mWorkingDirectoryEdit, SIGNAL(validChanged(bool)),
            this, SLOT(updateWorkingDirectory(bool)));
    connect(mWorkingDirectoryEdit, SIGNAL(editingFinished()),
            this, SLOT(updateWorkingDirectory()));
    connect(mWorkingDirectoryButton, SIGNAL(released()),
            this, SLOT(browseWorkingDirectory()));

    connect(mMakefileEdit, SIGNAL(validChanged(bool)),
            this, SLOT(updateMakefile(bool)));
    connect(mMakefileEdit, SIGNAL(editingFinished()),
            this, SLOT(updateMakefile()));
    connect(mMakefileDetectButton, SIGNAL(released()),
            this, SLOT(autoDetectMakefile()));
    connect(mMakefileBrowseButton, SIGNAL(released()),
            this, SLOT(browseMakefile()));

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
    if (valid)
        mData->workingDirectory = Utils::FileName::fromUserInput(mWorkingDirectoryEdit->text());
}

void TestRunConfigurationWidget::updateWorkingDirectory(void)
{
    if (mWorkingDirectoryEdit->isValid())
        mData->workingDirectory = Utils::FileName::fromUserInput(mWorkingDirectoryEdit->text());
    mWorkingDirectoryEdit->setText(mData->workingDirectory.toString());
}

void TestRunConfigurationWidget::browseWorkingDirectory(void)
{
    QString wd = QFileDialog::getExistingDirectory(this, tr("Choose working directory"), mData->workingDirectory.toString());

    if (!wd.isNull())
        mData->workingDirectory = Utils::FileName::fromString(wd);
    mWorkingDirectoryEdit->setText(mData->workingDirectory.toString());
}

void TestRunConfigurationWidget::updateMakefile(bool valid)
{
    if (valid)
        mData->setMakefile(mMakefileEdit->text());
}

void TestRunConfigurationWidget::updateMakefile(void)
{
    if (mMakefileEdit->isValid())
        mData->setMakefile(mMakefileEdit->text());
    else
        mMakefileEdit->setText(mData->makefile().toString());
}

void TestRunConfigurationWidget::autoDetectMakefile(void)
{
    mData->useDefaultMakefile();
    mMakefileEdit->setText(mData->makefile().toString());
}

void TestRunConfigurationWidget::browseMakefile(void)
{
    QString mf = QFileDialog::getOpenFileName(this, tr("Choose \"Makefile\""), mData->makefile().toString());

    if (!mf.isNull())
        mData->setMakefile(mf);
    mMakefileEdit->setText(mData->makefile().toString());
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
        mMakeExeEdit->setText(mData->makeExe().toString());
}

void TestRunConfigurationWidget::autoDetectMakeExe(void)
{
    mData->useDefaultMakeExe();
    mMakeExeEdit->setText(mData->makeExe().toString());
}
void TestRunConfigurationWidget::browseMakeExe(void)
{
    QString filter;
    if (Utils::HostOsInfo::isWindowsHost())
        filter = tr("Exécutable files *.exe");

    QString me = QFileDialog::getOpenFileName(this, tr("Choose \"make\""), mData->makeExe().toString(), filter, &filter);

    if (!me.isNull())
        mData->setMakeExe(me);
    mMakeExeEdit->setText(mData->makeExe().toString());
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

