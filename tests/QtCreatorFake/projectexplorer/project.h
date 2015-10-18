#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>

namespace ProjectExplorer {

class Project : public QObject
{
    Q_OBJECT
public:
    inline Project(QObject *parent = NULL) :
        QObject(parent) {}
};

} // ProjectExplorer

#endif // PROJECT_H
