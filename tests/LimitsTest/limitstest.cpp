#include <QString>
#include <QtTest>

class LimitsTest : public QObject
{
    Q_OBJECT

public:
    inline LimitsTest(void) {}

private Q_SLOTS:
    void p(void);
    void f(void);
    void testRowNames_data(void);
    void testRowNames(void);
};

void LimitsTest::p(void)
{
    QVERIFY2(true, "This test should pass.");
}

void LimitsTest::f(void)
{
    QVERIFY2(false, "This test should fail.");
}

void LimitsTest::testRowNames_data(void)
{
    QTest::addColumn<QString>("data");

    QTest::newRow("simple") << "simple";
    QTest::newRow("a)") << "a)";
    QTest::newRow("fun()") << "fun()";
    QTest::newRow("fun(int test)") << "fun(int test)";
    QTest::newRow("str.at().unicode()") << "str.at().unicode()";
    QTest::newRow("( ( (nested) ) )") << "( ( (nested) ) )";
}

void LimitsTest::testRowNames(void)
{
    QFETCH(QString, data);

    qDebug() << data;
}


QTEST_APPLESS_MAIN(LimitsTest)

#include "limitstest.moc"
