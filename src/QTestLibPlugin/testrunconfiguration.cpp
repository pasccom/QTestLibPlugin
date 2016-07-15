/* Copyright 2016 Pascal COMBES <pascom@orange.fr>
 * 
 * This file is part of QTestLibPlugin.
 * 
 * QTestLibPlugin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * QTestLibPlugin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with QTestLibPlugin. If not, see <http://www.gnu.org/licenses/>
 */

#include "testrunconfiguration.h"

#include "testrunconfigurationextraaspect.h"
#include "qtestlibpluginconstants.h"

#include "Widgets/filetypevalidatinglineedit.h"

#include <coreplugin/variablechooser.h>

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projectexplorersettings.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/devicesupport/devicemanager.h>
#include <projectexplorer/localenvironmentaspect.h>
#include <projectexplorer/toolchain.h>
#include <projectexplorer/gcctoolchain.h>
#include <projectexplorer/customtoolchain.h>
#include <projectexplorer/target.h>
#include <projectexplorer/runnables.h>

#include <QtWidgets>

#define UNSUPPORTED_TOOL_CHAIN  0
#define GCC_BASED_TOOL_CHAIN    1
#define NMMAKE_MSVC_TOOL_CHAIN  2
#define JOM_MSVC_TOOL_CHAIN     3
#define CUSTOM_TOOL_CHAIN       4

namespace QTestLibPlugin {
namespace Internal {

TestRunConfigurationData::TestRunConfigurationData(QObject* parent)
    : QObject(parent), jobNumber(1), testRunner(), mTargetToolChain(UNSUPPORTED_TOOL_CHAIN), mAutoMakeExe(), mMakeExe(), mAutoMakefile(), mMakefile()
{
    workingDirectory = Utils::FileName::fromString(QLatin1String("%{buildDir}"));
}

void TestRunConfigurationData::setTargetToolChain(unsigned char newToolChain)
{
    unsigned char oldToolChain = mTargetToolChain;
    mTargetToolChain = newToolChain;
    if (newToolChain != oldToolChain)
        emit targetToolChainChanged(newToolChain);
}

QString TestRunConfigurationData::toMakeFilePath(const QString& path) const
{
    if (!Utils::HostOsInfo::isWindowsHost() || (mTargetToolChain != GCC_BASED_TOOL_CHAIN))
        return path;

    // The path must be converted to MSYS path as used in the Makefile
    QString mSysPath = path;

    mSysPath.replace(QLatin1Char('\\'), QLatin1Char('/'));
    if (QDir(path).isAbsolute())
        mSysPath.replace(QRegExp(QLatin1String("^([^:]+):/")), QLatin1String("/\\1/"));

    qDebug() << path << "--msys>>" << mSysPath;
    return mSysPath;
}

QVariantMap TestRunConfigurationData::toMap(QVariantMap& map) const
{
    if (workingDirectory.toString() != QLatin1String("%{buildDir}"))
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
    workingDirectory = Utils::FileName::fromString(map.value(Constants::WorkingDirectoryKey, QLatin1String("%{buildDir}")).toString());
    mMakeExe = Utils::FileName::fromString(map.value(Constants::MakeExeKey, QString()).toString());
    mMakefile = Utils::FileName::fromString(map.value(Constants::MakefileKey, QString()).toString());
    testRunner = map.value(Constants::TestRunnerKey, QString()).toString();
    jobNumber = map.value(Constants::MakeJobNumberKey, 1).toInt();

    return true;
}

TestRunConfiguration::TestRunConfiguration(ProjectExplorer::Target *parent, Core::Id id):
    ProjectExplorer::RunConfiguration(parent, id)
{
    setDefaultDisplayName(QLatin1String("make check"));

    /* TODO ensure this run configuration cannot be run with valgrind...
     * To do this, the code of the Valgrind plugin should be altered:
     * 1.ValgrindRunControlFactory should check the type of the given RunConfiguration (e.g. in canRun())
     * and addAspects() should only add aspects provided bu runnable RunControl factories.
     * 2.Alternatively, ValgrindPlugin, should ensure the extra aspects are added to
     * sensible RunConfiguration and RunConfiguration::addExtraAspects() should be removed. */
    addExtraAspect(new ProjectExplorer::LocalEnvironmentAspect(this, ProjectExplorer::LocalEnvironmentAspect::BaseEnvironmentModifier()));
    addExtraAspect(new TestRunConfigurationExtraAspect(this));

    QTC_ASSERT(parent != NULL, return);

    mData = new TestRunConfigurationData(this);

    connect(parent, SIGNAL(kitChanged()),
            this, SLOT(handleTargetKitChange()));
    handleTargetKitChange();
}

void TestRunConfiguration::handleTargetKitChange(void)
{
    QTC_ASSERT(target() != NULL, return);
    QTC_ASSERT(target()->kit() != NULL, return);
    QTC_ASSERT(target()->activeBuildConfiguration() != NULL, return);

    ProjectExplorer::ToolChain *toolChain = ProjectExplorer::ToolChainKitInformation::toolChain(target()->kit());

    Utils::Environment env = target()->activeBuildConfiguration()->environment();
    mData->setAutoMakeExe(Utils::FileName::fromString(toolChain->makeCommand(env)));

    if (dynamic_cast<ProjectExplorer::GccToolChain*>(toolChain) != NULL) {
        mData->setTargetToolChain(GCC_BASED_TOOL_CHAIN);
    } else if (dynamic_cast<ProjectExplorer::CustomToolChain*>(toolChain) != NULL) {
        mData->setTargetToolChain(CUSTOM_TOOL_CHAIN);
    } else if ((toolChain->typeId() == ProjectExplorer::Constants::MSVC_TOOLCHAIN_TYPEID) || (toolChain->typeId() == ProjectExplorer::Constants::WINCE_TOOLCHAIN_TYPEID)) {
        if (ProjectExplorer::ProjectExplorerPlugin::projectExplorerSettings().useJom)
            mData->setTargetToolChain(JOM_MSVC_TOOL_CHAIN);
        else
            mData->setTargetToolChain(NMMAKE_MSVC_TOOL_CHAIN);
    }
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

ProjectExplorer::Runnable TestRunConfiguration::runnable(void) const
{
    ProjectExplorer::StandardRunnable runnable;
    if (macroExpander() != NULL)
        runnable.executable = macroExpander()->expand(mData->makeExe().toString());
    else
        runnable.executable = mData->makeExe().toString();
    runnable.commandLineArguments = commandLineArguments();
    runnable.workingDirectory = workingDirectory();
    runnable.environment = extraAspect<ProjectExplorer::LocalEnvironmentAspect>()->environment();
    runnable.runMode = ProjectExplorer::ApplicationLauncher::Gui;
    runnable.device = ProjectExplorer::DeviceManager::instance()->defaultDevice(ProjectExplorer::Constants::DESKTOP_DEVICE_TYPE);
    return runnable;
}

QString TestRunConfiguration::workingDirectory(void) const
{
    if ((mData->targetToolChain() == CUSTOM_TOOL_CHAIN) || (mData->targetToolChain() == UNSUPPORTED_TOOL_CHAIN)) {
        QTC_ASSERT(target()->activeBuildConfiguration() != NULL, return QLatin1String("."));
        return target()->activeBuildConfiguration()->buildDirectory().toString();
    }

    QString wd = mData->workingDirectory.toString();
    if (macroExpander() != NULL)
        wd = macroExpander()->expand(wd);

    return wd;
}

QString TestRunConfiguration::commandLineArguments(void) const
{
    QStringList cmdArgs;

    // Makefile path for make, nmake and jom
    QString makefilePath = mData->makefile().toString();
    makefilePath.replace(QLatin1Char('\"'), QLatin1String("\\\""));
    if (makefilePath.contains(QLatin1Char(' ')))
        makefilePath = QLatin1Char('\"') + makefilePath + QLatin1Char('\"');
    if (macroExpander() != NULL)
        makefilePath = macroExpander()->expand(makefilePath);
    if ((mData->targetToolChain() == NMMAKE_MSVC_TOOL_CHAIN) || (mData->targetToolChain() == JOM_MSVC_TOOL_CHAIN))
        cmdArgs << QLatin1String("/F") << makefilePath;
    else if (mData->targetToolChain() == GCC_BASED_TOOL_CHAIN)
        cmdArgs << QLatin1String("-f") << makefilePath;

    // Number of jobs for make and jom
    if (mData->jobNumber > 1) {
        if (mData->targetToolChain() == JOM_MSVC_TOOL_CHAIN)
            cmdArgs << QString(QLatin1String("/J %1")).arg(mData->jobNumber);
        else if (mData->targetToolChain() != GCC_BASED_TOOL_CHAIN)
            cmdArgs << QString(QLatin1String("-j%1")).arg(mData->jobNumber);
        // NOTE other targets don't support setting job number.
    }

    cmdArgs << QLatin1String("check");

    // Test runner:
    if (!mData->testRunner.isEmpty()) {
        QString testRunner = mData->testRunner;
        if (macroExpander() != NULL)
            testRunner = macroExpander()->expand(testRunner);
        cmdArgs << QString(QLatin1String("TESTRUNNER=\"%1\"")).arg(mData->toMakeFilePath(testRunner));
    }

    // Test arguments:
    Q_ASSERT(extraAspect<TestRunConfigurationExtraAspect>() != NULL);
    QString testCmdArgs = extraAspect<TestRunConfigurationExtraAspect>()->commandLineArguments().join(QLatin1Char(' '));
    if (macroExpander() != NULL)
        testCmdArgs = macroExpander()->expandProcessArgs(testCmdArgs);
    if (!testCmdArgs.isEmpty())
        cmdArgs << QString(QLatin1String("TESTARGS=\"%1\"")).arg(testCmdArgs);

    qDebug() << "Run config command line arguments:" << cmdArgs;
    return cmdArgs.join(QLatin1Char(' '));
}

TestRunConfigurationWidget::TestRunConfigurationWidget(TestRunConfigurationData* data, Utils::MacroExpander* macroExpander, QWidget* parent)
    : QWidget(parent), mData(data)
{
    mWorkingDirectoryEdit = new Widgets::FileTypeValidatingLineEdit(this);
    mWorkingDirectoryEdit->setMacroExpander(macroExpander);
    mWorkingDirectoryEdit->setAcceptDirectories(true);
    mWorkingDirectoryLabel = new QLabel(tr("Working directory:"), this);
    mWorkingDirectoryLabel->setBuddy(mWorkingDirectoryEdit);
    mWorkingDirectoryButton = new QPushButton(tr("Browse..."), this);
    mMakefileEdit = new Widgets::FileTypeValidatingLineEdit(this);
    mMakefileEdit->setMacroExpander(macroExpander);
    mMakefileLabel = new QLabel(tr("Makefile:"), this);
    mMakefileLabel->setBuddy(mWorkingDirectoryEdit);
    mMakefileDetectButton = new QPushButton(tr("Auto-detect"), this);
    mMakefileBrowseButton = new QPushButton(tr("Browse..."), this);
    mMakeExeEdit = new Widgets::FileTypeValidatingLineEdit(this);
    mMakeExeEdit->setMacroExpander(macroExpander);
    mMakeExeEdit->setRequireExecutable(true);
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
    mTestRunnerLabel = new QLabel(tr("Test runner:"), this);
    mTestRunnerLabel->setBuddy(mTestRunnerLabel);
    mTestRunnerButton = new QPushButton(tr("Browse..."), this);
    mJobsSpin = new QSpinBox(this);
    mJobsSpin->setRange(1, QThread::idealThreadCount());
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

    connect(mData, SIGNAL(targetToolChainChanged(unsigned char)),
            this, SLOT(handleTargetToolChainChange(unsigned char)));
}

void TestRunConfigurationWidget::showEvent(QShowEvent *se)
{
    mWorkingDirectoryEdit->setText(mData->workingDirectory.toString());
    mMakefileEdit->setText(mData->makefile().toString());
    mMakeExeEdit->setText(mData->makeExe().toString());
    mTestRunnerEdit->setText(mData->testRunner);
    mJobsSpin->setValue(mData->jobNumber);

    handleTargetToolChainChange(mData->targetToolChain());

    QWidget::showEvent(se);
}

void TestRunConfigurationWidget::handleTargetToolChainChange(unsigned char targetToolChain)
{
    mWorkingDirectoryLabel->setVisible((targetToolChain == GCC_BASED_TOOL_CHAIN) || (targetToolChain == NMMAKE_MSVC_TOOL_CHAIN) || (targetToolChain == JOM_MSVC_TOOL_CHAIN));
    mWorkingDirectoryEdit->setVisible((targetToolChain == GCC_BASED_TOOL_CHAIN) || (targetToolChain == NMMAKE_MSVC_TOOL_CHAIN) || (targetToolChain == JOM_MSVC_TOOL_CHAIN));
    mWorkingDirectoryButton->setVisible((targetToolChain == GCC_BASED_TOOL_CHAIN) || (targetToolChain == NMMAKE_MSVC_TOOL_CHAIN) || (targetToolChain == JOM_MSVC_TOOL_CHAIN));

    mMakefileLabel->setVisible((targetToolChain == GCC_BASED_TOOL_CHAIN) || (targetToolChain == NMMAKE_MSVC_TOOL_CHAIN) || (targetToolChain == JOM_MSVC_TOOL_CHAIN));
    mMakefileEdit->setVisible((targetToolChain == GCC_BASED_TOOL_CHAIN) || (targetToolChain == NMMAKE_MSVC_TOOL_CHAIN) || (targetToolChain == JOM_MSVC_TOOL_CHAIN));
    mMakefileDetectButton->setVisible((targetToolChain == GCC_BASED_TOOL_CHAIN) || (targetToolChain == NMMAKE_MSVC_TOOL_CHAIN) || (targetToolChain == JOM_MSVC_TOOL_CHAIN));
    mMakefileBrowseButton->setVisible((targetToolChain == GCC_BASED_TOOL_CHAIN) || (targetToolChain == NMMAKE_MSVC_TOOL_CHAIN) || (targetToolChain == JOM_MSVC_TOOL_CHAIN));

    mJobsLabel->setVisible((targetToolChain == GCC_BASED_TOOL_CHAIN) || (targetToolChain == JOM_MSVC_TOOL_CHAIN));
    mJobsSpin->setVisible((targetToolChain == GCC_BASED_TOOL_CHAIN) || (targetToolChain == JOM_MSVC_TOOL_CHAIN));
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
        filter = tr("Executable files *.exe");

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

