/* Copyright 2016-2020 Pascal COMBES <pascom@orange.fr>
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

#include "testextraaspect.h"
#include "pathaspect.h"
#include "qtestlibpluginconstants.h"

#include "Widgets/filetypevalidatinglineedit.h"

#include <coreplugin/variablechooser.h>

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projectexplorersettings.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projectconfigurationaspects.h>
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/devicesupport/devicemanager.h>
#include <projectexplorer/localenvironmentaspect.h>
#include <projectexplorer/toolchain.h>
#include <projectexplorer/gcctoolchain.h>
#include <projectexplorer/customtoolchain.h>
#include <projectexplorer/target.h>
#include <projectexplorer/runcontrol.h>

#include <qmakeprojectmanager/qmakeproject.h>

#include <utils/macroexpander.h>

#include <QThread>

#define UNSUPPORTED_TOOL_CHAIN  0
#define GCC_BASED_TOOL_CHAIN    1
#define NMMAKE_MSVC_TOOL_CHAIN  2
#define JOM_MSVC_TOOL_CHAIN     3
#define CUSTOM_TOOL_CHAIN       4

namespace QTestLibPlugin {
namespace Internal {

TestRunConfiguration::TestRunConfiguration(ProjectExplorer::Target *parent, Utils::Id id) :
    ProjectExplorer::RunConfiguration(parent, id)
{
    setDefaultDisplayName(QLatin1String("make check"));

    auto workingDirectoryAspect = addAspect<PathAspect>();
    workingDirectoryAspect->setId(Utils::Id(Constants::WorkingDirectoryId));
    workingDirectoryAspect->setSettingsKey(Constants::WorkingDirectoryKey);
    workingDirectoryAspect->setDisplayName(tr("Working directory"));
    workingDirectoryAspect->setMacroExpanderProvider([this] {return const_cast<Utils::MacroExpander*>(macroExpander());});
    workingDirectoryAspect->setAcceptDirectories(true);
    workingDirectoryAspect->setDefaultValue(Utils::FilePath::fromString("%{buildDir}"));

    auto makefileAspect = addAspect<PathAspect>();
    makefileAspect->setId(Utils::Id(Constants::MakefileId));
    makefileAspect->setSettingsKey(Constants::MakefileKey);
    makefileAspect->setDisplayName(tr("Makefile"));
    makefileAspect->setAcceptFiles(true);
    makefileAspect->setAcceptEmpty(true);
    makefileAspect->setMacroExpanderProvider([this] {return const_cast<Utils::MacroExpander*>(macroExpander());});

    auto makeExeAspect = addAspect<PathAspect>();
    makeExeAspect->setId(Utils::Id(Constants::MakeExeId));
    makeExeAspect->setSettingsKey(Constants::MakeExeKey);
    makeExeAspect->setDisplayName(tr("Path to alternative \"make\""));
    makeExeAspect->setAcceptFiles(true);
    makeExeAspect->setAcceptEmpty(true);
    makeExeAspect->setRequireExecutable(true);
    if (Utils::HostOsInfo::isWindowsHost())
        makeExeAspect->setRequiredExtensions(QStringList() << QLatin1String("exe"));
    else
        makeExeAspect->setRequiredExtensions(QStringList());
    makeExeAspect->setMacroExpanderProvider([this] {return const_cast<Utils::MacroExpander*>(macroExpander());});
    makeExeAspect->setCheckable(true);

    auto testRunnerAspect = addAspect<PathAspect>();
    testRunnerAspect->setId(Utils::Id(Constants::TestRunnerId));
    testRunnerAspect->setSettingsKey(Constants::TestRunnerKey);
    testRunnerAspect->setDisplayName(tr("Test runner"));
    testRunnerAspect->setAcceptFiles(true);
    testRunnerAspect->setAcceptEmpty(true);
    testRunnerAspect->setRequireExecutable(true);
    testRunnerAspect->setMacroExpanderProvider([this] {return const_cast<Utils::MacroExpander*>(macroExpander());});

    auto makeJobNumberAspect = addAspect<ProjectExplorer::BaseIntegerAspect>();
    makeJobNumberAspect->setId(Utils::Id(Constants::MakeJobNumberId));
    makeJobNumberAspect->setSettingsKey(Constants::MakeJobNumberKey);
    makeJobNumberAspect->setLabel(tr("Number of jobs (for \"make\")"));
    makeJobNumberAspect->setRange(1, QThread::idealThreadCount());

    addAspect<TestExtraAspect>();
    /* TODO ensure this run configuration cannot be run with valgrind...
     * To do this, the code of the Valgrind plugin should be altered:
     * 1.ValgrindRunControlFactory should check the type of the given RunConfiguration (e.g. in canRun())
     * and addAspects() should only add aspects provided bu runnable RunControl factories.
     * 2.Alternatively, ValgrindPlugin, should ensure the extra aspects are added to
     * sensible RunConfiguration and RunConfiguration::addExtraAspects() should be removed. */
    addAspect<ProjectExplorer::LocalEnvironmentAspect>(parent);

    QTC_ASSERT(parent != NULL, return);

    connect(parent, SIGNAL(kitChanged()),
            this, SLOT(handleTargetKitChange()));
    handleTargetKitChange();

    QMetaObject::Connection updateConnection = connect(parent, SIGNAL(parsingFinished(bool)), this, SLOT(update()));
    connect(parent, &ProjectExplorer::Target::removedRunConfiguration,
            this, [this, updateConnection] (ProjectExplorer::RunConfiguration* rc) {
        qDebug() << "QTC run configuration removed: " << rc;
        if (rc == this)
            disconnect(updateConnection);
    });
    if (!target()->buildSystem()->isParsing())
        update();
}

bool TestRunConfiguration::update(void)
{
    QmakeProjectManager::QmakePriFileNode* qMakeRootNode = dynamic_cast<QmakeProjectManager::QmakePriFileNode*>(target()->project()->rootProjectNode());
    QTC_ASSERT(qMakeRootNode != nullptr, return false);

    QmakeProjectManager::QmakeProFile* qMakeRoot = qMakeRootNode->proFileNode()->proFile();
    QStringList makefile = qMakeRoot->variableValue(QmakeProjectManager::Variable::Makefile);
    if (makefile.size() == 0) {
        setMakefile(Utils::FilePath());
    } else {
        QTC_ASSERT(makefile.size() == 1, );
        setMakefile(qMakeRoot->targetInformation().buildDir.pathAppended(makefile.first()));
    }

    return true;
}

void TestRunConfiguration::handleTargetKitChange(void)
{
    QTC_ASSERT(target() != NULL, return);
    QTC_ASSERT(target()->kit() != NULL, return);
    QTC_ASSERT(target()->activeBuildConfiguration() != NULL, return);

    ProjectExplorer::ToolChain *toolChain = ProjectExplorer::ToolChainKitAspect::toolChain(target()->kit(), ProjectExplorer::Constants::CXX_LANGUAGE_ID);

    Utils::Environment env = target()->activeBuildConfiguration()->environment();
    static_cast<PathAspect*>(aspect(Utils::Id(Constants::MakeExeId)))->setDefaultValue(toolChain->makeCommand(env));

    if (dynamic_cast<ProjectExplorer::GccToolChain*>(toolChain) != NULL) {
        setTargetToolChain(GCC_BASED_TOOL_CHAIN);
    } else if (dynamic_cast<ProjectExplorer::CustomToolChain*>(toolChain) != NULL) {
        setTargetToolChain(CUSTOM_TOOL_CHAIN);
    } else if ((toolChain->typeId() == ProjectExplorer::Constants::MSVC_TOOLCHAIN_TYPEID) || (toolChain->typeId() == ProjectExplorer::Constants::CLANG_CL_TOOLCHAIN_TYPEID)) {
        if (ProjectExplorer::ProjectExplorerPlugin::projectExplorerSettings().useJom)
            setTargetToolChain(JOM_MSVC_TOOL_CHAIN);
        else
            setTargetToolChain(NMMAKE_MSVC_TOOL_CHAIN);
    }
}
Utils::FilePath TestRunConfiguration::makefile(void) const
{
    Utils::FilePath makefilePath = static_cast<PathAspect*>(aspect(Utils::Id(Constants::MakefileId)))->value();
    if (macroExpander() != NULL)
        makefilePath = Utils::FilePath::fromString(macroExpander()->expand(makefilePath.toString()));
    if (makefilePath.isEmpty())
        makefilePath = static_cast<PathAspect*>(aspect(Utils::Id(Constants::MakefileId)))->defaultValue();
    return makefilePath;
}

void TestRunConfiguration::setMakefile(const Utils::FilePath& makefile)
{
    qDebug() << __func__ << makefile;

    if (!makefile.isEmpty())
        static_cast<PathAspect*>(aspect(Utils::Id(Constants::MakefileId)))->setDefaultValue(makefile);
    else if ((target() != NULL) && (target()->activeBuildConfiguration() != NULL))
        static_cast<PathAspect*>(aspect(Utils::Id(Constants::MakefileId)))->setDefaultValue(target()->activeBuildConfiguration()->buildDirectory().pathAppended("Makefile"));
}

void TestRunConfiguration::setTargetToolChain(unsigned char newToolChain)
{
    mTargetToolChain = newToolChain;

    aspect(Utils::Id(Constants::WorkingDirectoryId))->setVisible((mTargetToolChain == GCC_BASED_TOOL_CHAIN) || (mTargetToolChain == NMMAKE_MSVC_TOOL_CHAIN) || (mTargetToolChain == JOM_MSVC_TOOL_CHAIN));
    aspect(Utils::Id(Constants::MakefileId))->setVisible((mTargetToolChain == GCC_BASED_TOOL_CHAIN) || (mTargetToolChain == NMMAKE_MSVC_TOOL_CHAIN) || (mTargetToolChain == JOM_MSVC_TOOL_CHAIN));
    aspect(Utils::Id(Constants::MakeJobNumberId))->setVisible((mTargetToolChain == GCC_BASED_TOOL_CHAIN) || (mTargetToolChain == NMMAKE_MSVC_TOOL_CHAIN) || (mTargetToolChain == JOM_MSVC_TOOL_CHAIN));
}

ProjectExplorer::Runnable TestRunConfiguration::runnable(void) const
{
    ProjectExplorer::Runnable runnable;

    Utils::FilePath makeExe = static_cast<PathAspect*>(aspect(Utils::Id(Constants::MakeExeId)))->value();
    if (macroExpander() != nullptr)
        makeExe = Utils::FilePath::fromString(macroExpander()->expand(makeExe.toString()));
    if (makeExe.isEmpty())
        makeExe = static_cast<PathAspect*>(aspect(Utils::Id(Constants::MakeExeId)))->defaultValue();

    runnable.executable = makeExe;
    runnable.commandLineArguments = commandLineArguments();
    runnable.workingDirectory = workingDirectory().toString();
    runnable.environment = aspect<ProjectExplorer::LocalEnvironmentAspect>()->environment();
    runnable.device = ProjectExplorer::DeviceManager::instance()->defaultDevice(ProjectExplorer::Constants::DESKTOP_DEVICE_TYPE);

    qDebug() << runnable.workingDirectory << runnable.executable << runnable.commandLineArguments;

    return runnable;
}

Utils::FilePath TestRunConfiguration::workingDirectory(void) const
{
    if ((mTargetToolChain == CUSTOM_TOOL_CHAIN) || (mTargetToolChain == UNSUPPORTED_TOOL_CHAIN)) {
        QTC_ASSERT(target()->activeBuildConfiguration() != NULL, return Utils::FilePath::fromString("."));
        return target()->activeBuildConfiguration()->buildDirectory();
    }

    Utils::FilePath wd = static_cast<PathAspect*>(aspect(Utils::Id(Constants::WorkingDirectoryId)))->value();
    if (macroExpander() != NULL)
        wd = Utils::FilePath::fromString(macroExpander()->expand(wd.toString()));

    return wd;
}

QString TestRunConfiguration::commandLineArguments(void) const
{
    QStringList cmdArgs;

    // Makefile path for make, nmake and jom
    QString makefilePath = makefile().toString();
    makefilePath.replace(QLatin1Char('\"'), QLatin1String("\\\""));
    if (makefilePath.contains(QLatin1Char(' ')))
        makefilePath = QLatin1Char('\"') + makefilePath + QLatin1Char('\"');

    if ((mTargetToolChain == NMMAKE_MSVC_TOOL_CHAIN) || (mTargetToolChain == JOM_MSVC_TOOL_CHAIN))
        cmdArgs << QLatin1String("/F") << makefilePath;
    else if (mTargetToolChain == GCC_BASED_TOOL_CHAIN)
        cmdArgs << QLatin1String("-f") << makefilePath;

    // Number of jobs for make and jom
    int jobNumber = static_cast<ProjectExplorer::BaseIntegerAspect*>(aspect(Utils::Id(Constants::MakeJobNumberId)))->value();
    if (jobNumber > 1) {
        if (mTargetToolChain == JOM_MSVC_TOOL_CHAIN)
            cmdArgs << QString(QLatin1String("/J %1")).arg(jobNumber);
        else if (mTargetToolChain != GCC_BASED_TOOL_CHAIN)
            cmdArgs << QString(QLatin1String("-j%1")).arg(jobNumber);
        // NOTE other targets don't support setting job number.
    }

    cmdArgs << QLatin1String("check");

    // Test runner:
    QString testRunner = static_cast<PathAspect*>(aspect(Utils::Id(Constants::TestRunnerId)))->value().toString();
    if (macroExpander() != NULL)
        testRunner = macroExpander()->expand(testRunner);
    if (!testRunner.isEmpty()) {
        if (Utils::HostOsInfo::isWindowsHost() && (mTargetToolChain == GCC_BASED_TOOL_CHAIN)) {
            // NOTE The path must be converted to MSYS path as used in the Makefile
            QString mSysTestRunner = testRunner;

            mSysTestRunner.replace(QLatin1Char('\\'), QLatin1Char('/'));
            if (QDir(testRunner).isAbsolute())
                mSysTestRunner.replace(QRegExp(QLatin1String("^([^:]+):/")), QLatin1String("/\\1/"));

            qDebug() << testRunner << "--msys>>" << mSysTestRunner;
            testRunner = mSysTestRunner;
        }
        cmdArgs << QString(QLatin1String("TESTRUNNER=\"%1\"")).arg(testRunner);
    }

    // Test arguments:
    Q_ASSERT(aspect<TestExtraAspect>() != NULL);
    QString testCmdArgs = aspect<TestExtraAspect>()->commandLineArguments().join(QLatin1Char(' '));
    if (macroExpander() != NULL)
        testCmdArgs = macroExpander()->expandProcessArgs(testCmdArgs);
    if (!testCmdArgs.isEmpty())
        cmdArgs << QString(QLatin1String("TESTARGS=\"%1\"")).arg(testCmdArgs);

    qDebug() << "Run config command line arguments:" << cmdArgs;
    return cmdArgs.join(QLatin1Char(' '));
}

} // Internal
} // QTestLibPlugin
