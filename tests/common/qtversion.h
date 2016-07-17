#ifndef QTVERSION_H
#define QTVERSION_H

#include <QString>
#include <QtDebug>

class QtVersion
{
public:
    unsigned char majorNumber;
    unsigned char minorNumber;
    unsigned char bugFixNumber;

    QtVersion(void) :
        majorNumber(0), minorNumber(0), bugFixNumber(0) {}
    QtVersion(const QString& qtString);
    QtVersion(unsigned int qtInt);

    inline operator unsigned int(void) const {return ((unsigned int) majorNumber << 16) | ((unsigned int) minorNumber << 8) | (unsigned int) bugFixNumber;}
    inline operator QString(void) const {return QString("%1.%2.%3").arg(majorNumber)
                                                                   .arg(minorNumber)
                                                                   .arg(bugFixNumber);}
};

inline QDebug& operator<<(QDebug& dbg, const QtVersion qt)
{
    dbg.nospace() << "QtVersion(" << qt.majorNumber << ", " << qt.minorNumber << ", " << qt.bugFixNumber << ")";
    return dbg.space();
}


#endif // QTVERSION_H
