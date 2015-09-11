#include <QString>
#include <QtTest>

class OneClassTest : public QObject
{
    Q_OBJECT

public:
    inline OneClassTest(void) {}

private Q_SLOTS:
    void firstFunction_data(void);
    void firstFunction(void);
    void secondFunction_data(void);
    void secondFunction(void);
};

void OneClassTest::firstFunction_data(void)
{
    QTest::addColumn<QString>("data");
    QTest::newRow("firstData") << "firstData of firstFunction";
    QTest::newRow("secondData") << "secondData of firstFunction";
    QTest::newRow("thirdData") << "thirdData of firstFunction";
}

void OneClassTest::firstFunction(void)
{
    QFETCH(QString, data);

    QVERIFY(data.endsWith("Data of firstFunction"));
}

void OneClassTest::secondFunction_data(void)
{
    QTest::addColumn<QString>("data");
    QTest::newRow("firstData") << "firstData of secondFunction";
    QTest::newRow("secondData") << "secondData of secondFunction";
    QTest::newRow("thirdData") << "thirdData of secondFunction";
}

void OneClassTest::secondFunction(void)
{
    QFETCH(QString, data);

    QVERIFY(data.endsWith("Data of firstFunction"));
}

QTEST_APPLESS_MAIN(OneClassTest)

#include "oneclasstest.moc"
