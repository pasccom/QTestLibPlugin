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
