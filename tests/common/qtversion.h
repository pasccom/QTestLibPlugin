/* Copyright 2020 Pascal COMBES <pascom@orange.fr>
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
