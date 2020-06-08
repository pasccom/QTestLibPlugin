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

#include "qtversion.h"

#include <QtCore>

QtVersion::QtVersion(const QString& qtString)
{
    majorNumber = 0;
    minorNumber = 0;
    bugFixNumber = 0;

    QRegExp qtRegExp("(\\d+)\\.(\\d+)\\.(\\d+)");
    if (!qtRegExp.exactMatch(qtString))
        qWarning() << "Failed to parse Qt Version:" << qtString;

    bool okMajor = false, okMinor = false, okBugFix = false;
    unsigned int maj = qtRegExp.cap(1).toUInt(&okMajor, 10);
    unsigned int min = qtRegExp.cap(2).toUInt(&okMinor, 10);
    unsigned int bug = qtRegExp.cap(3).toUInt(&okBugFix, 10);

    if (!okMajor || !okMinor || !okBugFix)
        qWarning() << "Failed to parse in Qt Version:" << qtRegExp.cap(1) << qtRegExp.cap(2) << qtRegExp.cap(3);

    if (maj > 0xFF)
        qWarning() << "Major number too large" << maj;
    majorNumber = qMin(0xFFu, maj);

    if (min > 0xFF)
        qWarning() << "Minor number too large" << min;
    minorNumber = qMin(0xFFu, min);

    if (bug > 0xFF)
        qWarning() << "Bug fix number too large" << bug;
    bugFixNumber = qMin(0xFFu, bug);
}

QtVersion::QtVersion(unsigned int qtInt)
{
    majorNumber = (qtInt & 0xFF0000u) >> 16;
    minorNumber = (qtInt & 0x00FF00u) >> 8;
    bugFixNumber = (qtInt & 0x0000FFu);
}
