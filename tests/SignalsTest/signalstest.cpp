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

#include <QString>
#include <QtTest>

namespace NameSpace {

class EmitterInNameSpace : public QObject
{
    Q_OBJECT
public:
    inline EmitterInNameSpace(QObject* parent =  NULL) : QObject(parent) {emit objectConstructed();}
    inline ~EmitterInNameSpace(void) {emit objectDestroyed();}
signals:
    void objectConstructed(void);
    void objectDestroyed(void);
};

class ReceiverInNameSpace : public QObject
{
    Q_OBJECT
public:
    inline ReceiverInNameSpace(QObject* parent =  NULL) : QObject(parent) {}
public slots:
    inline void objectConstructed(void) {qDebug() << sender() << "constructed";}
    inline void objectDestroyed(void) {qDebug() << sender() << "destroyed";}
};

} // NameSpace

class Emitter : public QObject
{
    Q_OBJECT
public:
    inline Emitter(QObject* parent = NULL) : QObject(parent) {emit objectConstructed();}
    inline ~Emitter(void) {emit objectDestroyed();}
signals:
    void objectConstructed(void);
    void objectDestroyed(void);
};

class Receiver : public QObject
{
    Q_OBJECT
public:
    inline Receiver(QObject* parent =  NULL) : QObject(parent) {}
public slots:
    inline void objectConstructed(void) {qDebug() << sender() << "constructed";}
    inline void objectDestroyed(void) {qDebug() << sender() << "destroyed";}
};

class SignalsTest : public QObject
{
    Q_OBJECT

public:
    inline SignalsTest(void) {}
signals:
    void signal1(void);
    void signal2(const QString& data);
    void signal3(int i, int j);
private Q_SLOTS:
    void firstFunction(void);
    void secondFunction(void);
    void thirdFunction(void);
    void multiline(void);
    void emitterStack(void);
    void emitterHeap(void);
    void emitterInNameSpaceStack(void);
    void emitterInNameSpaceHeap(void);
    void receiverStack(void);
    void receiverHeap(void);
    void receiverInNameSpaceStack(void);
    void receiverInNameSpaceHeap(void);
};

void SignalsTest::firstFunction(void)
{
    emit signal1();
    emit signal2("firstFunction");
    emit signal3(18, 36);
}

void SignalsTest::secondFunction(void)
{
    emit signal2("secondFunction");
    emit signal3(54, 72);
    emit signal1();
}

void SignalsTest::thirdFunction(void)
{
    emit signal3(90, 108);
    emit signal1();
    emit signal2("thirdFunction");
}

void SignalsTest::multiline(void)
{
    emit signal2("Multiline message test:\nPASS   : fun()\nFAIL!  : other_fun(test)");
}

void SignalsTest::emitterStack(void)
{
    Emitter obj;

    qDebug() << &obj;
}

void SignalsTest::emitterInNameSpaceStack(void)
{
    NameSpace::EmitterInNameSpace obj;

    qDebug() << &obj;
}

void SignalsTest::emitterHeap(void)
{
    Emitter *obj = new Emitter(this);

    qDebug() << obj;

    delete obj;
}

void SignalsTest::emitterInNameSpaceHeap(void)
{
    NameSpace::EmitterInNameSpace* obj = new NameSpace::EmitterInNameSpace(this);

    qDebug() << obj;

    delete obj;
}

void SignalsTest::receiverStack(void)
{
    Emitter emitter;
    Receiver *receiver = new Receiver(this);

    connect(&emitter, SIGNAL(objectDestroyed()), receiver, SLOT(objectDestroyed()));

    receiver->deleteLater();
}

void SignalsTest::receiverHeap(void)
{
    Emitter *emitter = new Emitter(this);
    Receiver *receiver = new Receiver(this);

    connect(emitter, SIGNAL(objectDestroyed()), receiver, SLOT(objectDestroyed()));

    delete emitter;
    receiver->deleteLater();
}

void SignalsTest::receiverInNameSpaceStack(void)
{
    NameSpace::EmitterInNameSpace emitter;
    NameSpace::ReceiverInNameSpace *receiver = new NameSpace::ReceiverInNameSpace(this);

    connect(&emitter, SIGNAL(objectDestroyed()), receiver, SLOT(objectDestroyed()));

    receiver->deleteLater();
}

void SignalsTest::receiverInNameSpaceHeap(void)
{
    NameSpace::EmitterInNameSpace *emitter = new NameSpace::EmitterInNameSpace(this);
    NameSpace::ReceiverInNameSpace *receiver = new NameSpace::ReceiverInNameSpace(this);

    connect(emitter, SIGNAL(objectDestroyed()), receiver, SLOT(objectDestroyed()));

    delete emitter;
    receiver->deleteLater();
}

QTEST_APPLESS_MAIN(SignalsTest)

#include "signalstest.moc"
