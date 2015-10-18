#include "qmakeproject.h"

#include <qmakevfs.h>
#include <profileevaluator.h>

namespace QmakeProjectManager {

QmakeProject::QmakeProject(const QString& proFile, QObject *parent) :
    ProjectExplorer::Project(parent)
{
    mVfs = new QMakeVfs;
    mGlobals = new ProFileGlobals;
    mGlobals->qmake_abslocation = "/usr/bin/qmake-qt5";
    mGlobals->qmakespec = "/usr/lib64/qt5/mkspecs/linux-g++-64";
    mGlobals->xqmakespec = "/usr/lib64/qt5/mkspecs/linux-g++-64";
    mGlobals->sysroot = QString::null;

    QHash<QString, QString> qtVersionInfo;
    qtVersionInfo.insert("QT_SYSROOT", QString::null);
    qtVersionInfo.insert("QT_INSTALL_PREFIX", "/usr");
    qtVersionInfo.insert("QT_INSTALL_ARCHDATA", "/usr/lib64/qt5");
    qtVersionInfo.insert("QT_INSTALL_DATA", "/usr/share/qt5");
    qtVersionInfo.insert("QT_INSTALL_DOCS", "/usr/share/doc/packages/qt5");
    qtVersionInfo.insert("QT_INSTALL_HEADERS", "/usr/include/qt5");
    qtVersionInfo.insert("QT_INSTALL_LIBS", "/usr/lib64");
    qtVersionInfo.insert("QT_INSTALL_LIBEXECS", "/usr/lib64/qt5/libexec");
    qtVersionInfo.insert("QT_INSTALL_BINS", "/usr/lib64/qt5/bin");
    qtVersionInfo.insert("QT_INSTALL_TESTS", "/usr/tests");
    qtVersionInfo.insert("QT_INSTALL_PLUGINS", "/usr/lib64/qt5/plugins");
    qtVersionInfo.insert("QT_INSTALL_IMPORTS", "/usr/lib64/qt5/imports");
    qtVersionInfo.insert("QT_INSTALL_QML", "/usr/lib64/qt5/qml");
    qtVersionInfo.insert("QT_INSTALL_TRANSLATIONS", "/usr/share/qt5/translations");
    qtVersionInfo.insert("QT_INSTALL_CONFIGURATION", "/etc/xdg");
    qtVersionInfo.insert("QT_INSTALL_EXAMPLES", "/usr/lib64/qt5/examples");
    qtVersionInfo.insert("QT_INSTALL_DEMOS", "/usr/lib64/qt5/examples");
    qtVersionInfo.insert("QT_HOST_PREFIX", "/usr");
    qtVersionInfo.insert("QT_HOST_DATA", "/usr/lib64/qt5");
    qtVersionInfo.insert("QT_HOST_BINS", "/usr/lib64/qt5/bin");
    qtVersionInfo.insert("QT_HOST_LIBS", "/usr/lib64");
    qtVersionInfo.insert("QMAKE_VERSION", "3.0");
    qtVersionInfo.insert("QT_VERSION", "5.4.2");
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
