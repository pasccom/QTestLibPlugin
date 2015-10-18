#ifndef QTCREATOR_FAKE_GLOBAL_H
#define QTCREATOR_FAKE_GLOBAL_H

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
