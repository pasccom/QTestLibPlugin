#include <QString>
#include <QtTest>

#include <iostream>

class LimitsTest : public QObject
{
    Q_OBJECT

public:
    LimitsTest(void);

private Q_SLOTS:
    void p(void);
    void f(void);
    void multiline(void);
    void testRowNames_data(void);
    void testRowNames(void);
};

LimitsTest::LimitsTest(void)
{
    std::cout << "Subreptive stdout message" << std::endl;
    qDebug() << "Subreptive debug message";
}

void LimitsTest::p(void)
{
    QVERIFY2(true, "This test should pass.");
}

void LimitsTest::f(void)
{
    QVERIFY2(false, "This test should fail.");
}

void LimitsTest::multiline(void)
{
    qDebug() << "Multiline message test:\nPASS   : fun()\nFAIL!  : other_fun(test)";
}

void LimitsTest::testRowNames_data(void)
{
    QTest::addColumn<QString>("data");

    QTest::newRow("simple") << "simple";
    QTest::newRow("a)") << "a)";
    QTest::newRow("fun()") << "fun()";
    QTest::newRow("fun(int test)") << "fun(int test)";
    QTest::newRow("str.at().unicode()") << "str.at().unicode()";
    QTest::newRow("( (a + b) + (c + d) ) + ( (e +f) + (g + h) )") << "( (a + b) + (c + d) ) + ( (e +f) + (g + h) )";
    QTest::newRow("a)( (a + b) + (c + d) ) + ( (e +f) + (g + h) )") << "a)( (a + b) + (c + d) ) + ( (e +f) + (g + h) )";
    QTest::newRow("( ( (nested) ) )") << "( ( (nested) ) )";
    QTest::newRow("a)( ( (nested) ) )") << "a)( ( (nested) ) )";
    QTest::newRow("( ( (double) ) ).( ( (nested) ) )") << "( ( (double) ) ).( ( (nested) ) )";
    QTest::newRow("a)( ( (double) ) ).( ( (nested) ) )") << "a)( ( (double) ) ).( ( (nested) ) )";
}

void LimitsTest::testRowNames(void)
{
    QFETCH(QString, data);

    qDebug() << data;
}


QTEST_APPLESS_MAIN(LimitsTest)

#include "limitstest.moc"
