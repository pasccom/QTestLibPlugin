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

#ifndef QTCREATOR_FAKE_GLOBAL_H
#define QTCREATOR_FAKE_GLOBAL_H

//#define QTC_ASSERT(_cond_, _fail_) if (!(_cond_)) _fail_

#if defined(QTCREATOR_FAKE_DEBUG_LEVEL) && (QTCREATOR_FAKE_DEBUG_LEVEL >= 1)
#define debugMsg(_msg_) qDebug() << _msg_ << qPrintable(QString("(%1:%2)").arg(__FILE__).arg(__LINE__))
#define USED_ONLY_FOR_DEBUG(_arg_)
#else
#define debugMsg(_msg_) qtCreatorFakeNoOp()
#define USED_ONLY_FOR_DEBUG(_arg_) Q_UNUSED(_arg_)
#endif

#if defined(QTCREATOR_FAKE_DEBUG_LEVEL) && (QTCREATOR_FAKE_DEBUG_LEVEL >= 2)
#define traceMsg(_msg_) qDebug() << _msg_ << qPrintable(QString("(%1:%2)").arg(__FILE__).arg(__LINE__))
#define USED_ONLY_FOR_TRACE(_arg_)
#else
#define traceMsg(_msg_) qtCreatorFakeNoOp()
#define USED_ONLY_FOR_TRACE(_arg_) Q_UNUSED(_arg_)
#endif

inline void qtCreatorFakeNoOp(void)
{
}

#endif // QTCREATOR_FAKE_GLOBAL_H
