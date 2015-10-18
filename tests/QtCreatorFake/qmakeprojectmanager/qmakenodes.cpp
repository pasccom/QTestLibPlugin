#include "qmakenodes.h"

#include "qmakeproject.h"

#include "../qtcreatorfake_global.h"

#include <qmakeparser.h>

#include <utils/hostosinfo.h>

#include <QtCore>
#include <QtDebug>

namespace QmakeProjectManager {

QmakeProFileNode::QmakeProFileNode(QmakeProject *project, const QString& proFile) :
    QMakeParser(NULL, project->vfs(), this),
    ProFileEvaluator(project->globals(), this, project->vfs(), this),
    mProject(project), mParsing(true), mEvalSuceeded(false)
{
    QDir proFileDir(proFile);
    mProFileName = QDir::cleanPath(proFileDir.absolutePath());

    debugMsg("Starting parse of:" << mProFileName);
    ProFile *parsed = parsedProFile(mProFileName, QMakeParser::ParseReportMissing);
    if (parsed->isOk()) {
        mEvalSuceeded = accept(parsed, QMakeEvaluator::LoadPreFiles);
        if (!mEvalSuceeded)
            debugMsg("Failed evaluation of:" << parsed->fileName());
    } else {
        debugMsg("Failed parsing of:" << mProFileName);
    }

    parsed->deref();
    mParsing = false;
}

QString QmakeProFileNode::displayName(void) const
{
    QFileInfo proFileInfo(mProFileName);
    return proFileInfo.baseName();
}

void QmakeProFileNode::message(int type, const QString& msg, const QString& fileName, int lineNo)
{
    QString location;
    if (!fileName.isNull()) {
        location = fileName;
        if (lineNo > 0)
            location.append(":").append(QString::number(lineNo, 10));
    }

    switch (type & QMakeParserHandler::CategoryMask) {
    case QMakeParserHandler::ErrorMessage:
        qCritical() << "QMake error:"
                    << (location.isNull() ? msg : QString("%1 (%2)").arg(msg).arg(location));
        break;
    case QMakeParserHandler::WarningMessage:
        qWarning() << "QMake warning:"
                   << (location.isNull() ? msg : QString("%1 (%2)").arg(msg).arg(location));
        break;
    default:
        debugMsg("QMake message:" << (location.isNull() ? msg : QString("%1 (%2)").arg(msg).arg(location)));
        break;
    }
}

void QmakeProFileNode::fileMessage(const QString& msg)
{
    USED_ONLY_FOR_DEBUG(msg);
    debugMsg("Project message:" << msg);
}

void QmakeProFileNode::aboutToEval(ProFile *parent, ProFile *proFile, EvalFileType type)
{
    switch (type) {
    case EvalProjectFile:
        traceMsg("About to eval:" << proFile->fileName());
        if (parent != NULL)
            traceMsg("for main project:" << parent->fileName());
        break;
    case EvalIncludeFile:
        traceMsg("About to eval:" << proFile->fileName());
        if (parent != NULL)
            traceMsg("included in project:" << parent->fileName());
        break;
    case EvalConfigFile:
        traceMsg("About to eval config file:" << proFile->fileName());
        if (parent != NULL)
            traceMsg("for project:" << parent->fileName());
        break;
    case EvalFeatureFile:
        traceMsg("About to eval feature file:" << proFile->fileName());
        if (parent != NULL)
            traceMsg("for project:" << parent->fileName());
        break;
    case EvalAuxFile:
        traceMsg("About to eval auxiliary file:" << proFile->fileName());
        if (parent != NULL)
            traceMsg("for project:" << parent->fileName());
        break;
    }

    if ((proFile != NULL) && (QString::compare(proFile->fileName(), mProFileName, Utils::HostOsInfo::fileNameCaseSensitivity()) == 0)) {
        emit mProject->aboutToEval(proFile->fileName());
    }
}

void QmakeProFileNode::doneWithEval(ProFile *parent)
{
    if (parent != NULL) {
        traceMsg("Done with eval of:" << parent->fileName());
        if (QString::compare(parent->fileName(), mProFileName, Utils::HostOsInfo::fileNameCaseSensitivity()) == 0)
            emit mProject->doneWithEval(parent->fileName());
    } else {
        traceMsg("Done with eval of some file");
    }
}

QStringList QmakeProFileNode::variableValue(const QmakeVariable var) const
{
    switch (var) {
    case DefinesVar:
        return values("DEFINES");
    /*case IncludePathVar:
        return values("INCLUDEPATH");*/
    case CppFlagsVar:
        return values("QMAKE_CXXFLAGS");
    case CppHeaderVar:
        return values("HEADERS");
    case CppSourceVar:
        return values("SOURCES");
    case ObjCSourceVar:
        return values("OBJECTIVE_SOURCES");
    case ObjCHeaderVar:
        return values("OBJECTIVE_HEADERS");
    case ResourceVar:
    case ExactResourceVar:
        return values("RESOURCES");
    case UiDirVar:
        return values("UI_DIR");
    case MocDirVar:
        return values("MOC_DIR");
    case PkgConfigVar:
        return values("PKGCONFIG");
    /*case PrecompiledHeaderVar:
        return values("PRECOMPILED_HEADER");*/
    /*case LibDirectoriesVar:
        return values("SOURCES");*/
    case ConfigVar:
        return values("CONFIG");
    case QtVar:
        return values("QT");
    /*case QmlImportPathVar:
        return values("SOURCES");*/
    case Makefile:
        return values("MAKEFILE");
    case ObjectExt:
        return values("QMAKE_EXT_OBJ");
    case ObjectsDir:
        return values("OBJECTS_DIR");
    case VersionVar:
        return values("VERSION");
    case TargetExtVar:
        return values("TARGET_EXT");
    case TargetVersionExtVar:
        return values("TARGET_VERSION_EXT");
    case StaticLibExtensionVar:
        return values("QMAKE_EXTENSION_STATICLIB");
    case ShLibExtensionVar:
        return values("QMAKE_EXTENSION_SHLIB");
    case AndroidArchVar:
        return values("ANDROID_TARGET_ARCH");
    case AndroidDeploySettingsFile:
        return values("ANDROID_DEPLOYMENT_SETTINGS_FILE");
    case AndroidPackageSourceDir:
        return values("ANDROID_PACKAGE_SOURCE_DIR");
    case AndroidExtraLibs:
        return values("ANDROID_EXTRA_LIBS");
    case IsoIconsVar:
        return values("ISO_ICONS");
    default:
        break;
    }

    return QStringList();
}

}
