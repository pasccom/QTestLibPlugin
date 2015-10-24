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

#ifndef QMAKEPROJECT_H
#define QMAKEPROJECT_H

#include <projectexplorer/project.h>

#include "qmakenodes.h"

#include <QList>

class QMakeVfs;
class ProFileGlobals;

namespace QmakeProjectManager {

class QmakeProject : public ProjectExplorer::Project
{
    Q_OBJECT
public:
    enum Parsing {
        ExactParse,
        ExactAndCumulativeParse
    };
    QmakeProject(const QString& proFile, QObject *parent = NULL);
    ~QmakeProject(void);

    inline QmakeProFileNode *rootQmakeProjectNode(void) const {return mRootProFileNode;}
    inline QList<QmakeProFileNode *> allProFiles(const QList<QmakeProjectType> &projectTypes = QList<QmakeProjectType>(),
                                                 Parsing parse = ExactParse) const {Q_UNUSED(projectTypes); Q_UNUSED(parse); return mProFileNodes;}

    QMakeVfs *vfs() const {return mVfs;}
    ProFileGlobals *globals() const {return mGlobals;}
signals:
    void aboutToEval(const QString& file);
    void doneWithEval(const QString& file);
private:
    QmakeProFileNode *mRootProFileNode;
    QList<QmakeProFileNode *> mProFileNodes;

    QMakeVfs *mVfs;
    ProFileGlobals *mGlobals;
};

} // QmakeProjectManager

#endif // QMAKEPROJECT_H
