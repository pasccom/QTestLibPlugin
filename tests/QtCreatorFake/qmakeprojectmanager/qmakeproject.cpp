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

#include "qmakeproject.h"
#include "qmake_paths.local.h"

#include <qmakevfs.h>
#include <profileevaluator.h>

namespace QmakeProjectManager {

QmakeProject::QmakeProject(const QString& proFile, QObject *parent) :
    ProjectExplorer::Project(parent)
{
    mVfs = new QMakeVfs;
    mGlobals = new ProFileGlobals;
    mGlobals->qmake_abslocation = QMAKE_ABSLOCATION;
    mGlobals->qmakespec = QMAKESPEC;
    mGlobals->xqmakespec = XQMAKESPEC;
    mGlobals->sysroot = QString::null;

    QHash<QString, QString> qtVersionInfo;
    qtVersionInfo.insert("QT_SYSROOT", QT_SYSROOT);
    qtVersionInfo.insert("QT_INSTALL_PREFIX", QT_INSTALL_PREFIX);
    qtVersionInfo.insert("QT_INSTALL_ARCHDATA", QT_INSTALL_ARCHDATA);
    qtVersionInfo.insert("QT_INSTALL_DATA", QT_INSTALL_DATA);
    qtVersionInfo.insert("QT_INSTALL_DOCS", QT_INSTALL_DOCS);
    qtVersionInfo.insert("QT_INSTALL_HEADERS", QT_INSTALL_HEADERS);
    qtVersionInfo.insert("QT_INSTALL_LIBS", QT_INSTALL_LIBS);
    qtVersionInfo.insert("QT_INSTALL_LIBEXECS", QT_INSTALL_LIBEXECS);
    qtVersionInfo.insert("QT_INSTALL_BINS", QT_INSTALL_BINS);
    qtVersionInfo.insert("QT_INSTALL_TESTS", QT_INSTALL_TESTS);
    qtVersionInfo.insert("QT_INSTALL_PLUGINS", QT_INSTALL_TESTS);
    qtVersionInfo.insert("QT_INSTALL_IMPORTS", QT_INSTALL_IMPORTS);
    qtVersionInfo.insert("QT_INSTALL_QML", QT_INSTALL_QML);
    qtVersionInfo.insert("QT_INSTALL_TRANSLATIONS", QT_INSTALL_TRANSLATIONS);
    qtVersionInfo.insert("QT_INSTALL_CONFIGURATION", QT_INSTALL_CONFIGURATION);
    qtVersionInfo.insert("QT_INSTALL_EXAMPLES", QT_INSTALL_EXAMPLES);
    qtVersionInfo.insert("QT_INSTALL_DEMOS", QT_INSTALL_DEMOS);
    qtVersionInfo.insert("QT_HOST_PREFIX", QT_HOST_PREFIX);
    qtVersionInfo.insert("QT_HOST_DATA", QT_HOST_DATA);
    qtVersionInfo.insert("QT_HOST_BINS", QT_HOST_BINS);
    qtVersionInfo.insert("QT_HOST_LIBS", QT_HOST_LIBS);
    qtVersionInfo.insert("QMAKE_VERSION", QMAKE_VERSION);
    qtVersionInfo.insert("QT_VERSION", QT_VERSION_STR);
    mGlobals->setProperties(qtVersionInfo);

    mRootProFileNode = new QmakeProFileNode(this, proFile);
    mProFileNodes << mRootProFileNode;
}

QmakeProject::~QmakeProject(void)
{
    delete mRootProFileNode;

    delete mVfs;
    delete mGlobals;
}

} // QmakeProject
