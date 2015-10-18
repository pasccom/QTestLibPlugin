#ifndef TARGET_H
#define TARGET_H

#include <QObject>
#include "project.h"

namespace ProjectExplorer {

class Kit;

class Target : public QObject
{
    Q_OBJECT
public:
    inline Target(Project *project, Kit *kit) :
        QObject(project), mProject(project), mKit(kit) {}

    inline Kit *kit(void) const {return mKit;}
    inline Project *project(void) const {return mProject;}
private:
    Project *mProject;
    Kit *mKit;
};

} // ProjectExplorer

#endif // TARGET_H
