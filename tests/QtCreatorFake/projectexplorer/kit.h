#ifndef KIT_H
#define KIT_H

#include <QObject>

namespace ProjectExplorer {

class Kit : public QObject
{
    Q_OBJECT
public:
    inline Kit(QObject *parent = NULL) :
        QObject(parent) {}
};

} // ProjectExplorer

#endif // KIT_H
