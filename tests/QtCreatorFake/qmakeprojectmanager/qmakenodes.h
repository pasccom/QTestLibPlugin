/* Copyright 2015 Pascal COMBES <pascom@orange.fr>
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

#ifndef QMAKENODES_H
#define QMAKENODES_H

#include <profileevaluator.h>
#include <qmakeparser.h>

namespace QmakeProjectManager {

class QmakeProject;

enum QmakeProjectType {
    InvalidProject = 0,
    ApplicationTemplate,
    StaticLibraryTemplate,
    SharedLibraryTemplate,
    ScriptTemplate,
    AuxTemplate,
    SubDirsTemplate
};

enum QmakeVariable {
    DefinesVar = 1,
    IncludePathVar,
    CppFlagsVar,
    CppHeaderVar,
    CppSourceVar,
    ObjCSourceVar,
    ObjCHeaderVar,
    ResourceVar,
    ExactResourceVar,
    UiDirVar,
    MocDirVar,
    PkgConfigVar,
    PrecompiledHeaderVar,
    LibDirectoriesVar,
    ConfigVar,
    QtVar,
    QmlImportPathVar,
    Makefile,
    ObjectExt,
    ObjectsDir,
    VersionVar,
    TargetExtVar,
    TargetVersionExtVar,
    StaticLibExtensionVar,
    ShLibExtensionVar,
    AndroidArchVar,
    AndroidDeploySettingsFile,
    AndroidPackageSourceDir,
    AndroidExtraLibs,
    IsoIconsVar
};

class QmakeProFileNode : public QMakeParser, public ProFileEvaluator, public QMakeHandler
{
public:
    QmakeProFileNode(QmakeProject *project, const QString& proFile);
    inline virtual ~QmakeProFileNode(void) {}
    QString displayName(void) const;
    QStringList variableValue(const QmakeVariable var) const;

    inline bool isValid(void) const {return mEvalSuceeded;}
    inline bool isParsing(void) const {return mParsing;}

    void message(int type, const QString &msg, const QString &fileName = QString(), int lineNo = 0);
    void fileMessage(const QString &msg);
    void aboutToEval(ProFile *parent, ProFile *proFile, EvalFileType type);
    void doneWithEval(ProFile *parent);

private:
    QmakeProject *mProject;
    QString mProFileName;
    bool mParsing;
    bool mEvalSuceeded;
};

} // QmakeProjectManager

#endif // QMAKENODES_H
