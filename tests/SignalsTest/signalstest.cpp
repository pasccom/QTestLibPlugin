#include <QString>
#include <QtTest>

namespace NameSpace {

class ClassInNameSpace : public QObject
{
    Q_OBJECT
public:
    inline ClassInNameSpace(QObject* parent =  NULL) : QObject(parent) {emit constructed();}
    ~ClassInNameSpace(void) {emit destroyed();}
signals:
    void constructed(void);
    void destroyed(void);
};

} // NameSpace

class OtherClass : public QObject
{
    Q_OBJECT
public:
    inline OtherClass(QObject* parent = NULL) : QObject(parent) {emit constructed();}
    ~OtherClass(void) {emit destroyed();}
signals:
    void constructed(void);
    void destroyed(void);
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
    void otherClass(void);
    void otherClassNamespace(void);
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

void SignalsTest::otherClass(void)
{
    OtherClass obj;

    qDebug() << &obj;
}

void SignalsTest::otherClassNamespace(void)
{
    NameSpace::ClassInNameSpace obj;

    qDebug() << &obj;
}

QTEST_APPLESS_MAIN(SignalsTest)

#include "signalstest.moc"
