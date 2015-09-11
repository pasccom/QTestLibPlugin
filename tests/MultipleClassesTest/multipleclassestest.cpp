#include <QtTest>

class FirstClassTest : public QObject
{
    Q_OBJECT

public:
    inline FirstClassTest(void) {}

private Q_SLOTS:
    void firstPass(void);
    void secondPass(void);
};

class SecondClassTest : public QObject
{
    Q_OBJECT

public:
    inline SecondClassTest() {}

private Q_SLOTS:
    void firstPass(void);
    void secondPass_data(void);
    void secondPass(void);
};

class ThirdClassTest : public QObject
{
    Q_OBJECT

public:
    inline ThirdClassTest(void) {}

private Q_SLOTS:
    void firstFail(void);
    void secondPass(void);
};

void FirstClassTest::firstPass(void)
{
    QVERIFY2(true, "Test should pass");
}

void FirstClassTest::secondPass(void)
{
    QVERIFY2(true, "Test should pass");
}

void SecondClassTest::firstPass(void)
{
    QVERIFY2(true, "Test should pass");
}

void SecondClassTest::secondPass_data(void)
{
    QTest::addColumn<QString>("data");
    QTest::newRow("firstData") << "firstData";
    QTest::newRow("secondData") << "secondData";
}

void SecondClassTest::secondPass(void)
{
    QFETCH(QString, data);
    qDebug() << data;

    QVERIFY2(data.endsWith("Data"), "Test should pass");
}

void ThirdClassTest::firstFail(void)
{
    QVERIFY2(false, "Test should fail");
}

void ThirdClassTest::secondPass(void)
{
    QVERIFY2(true, "Test should pass");
}

int main(int argc, char** argv)
{
    int result = 0;

    FirstClassTest firstObj;
    result += QTest::qExec(&firstObj, argc, argv);

    SecondClassTest secondObj;
    result += QTest::qExec(&secondObj, argc, argv);

    ThirdClassTest thirdObj;
    result += QTest::qExec(&thirdObj, argc, argv);

    return result;
}

#include "multipleclassestest.moc"
