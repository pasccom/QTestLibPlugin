#include <QString>
#include <QtTest>

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

QTEST_APPLESS_MAIN(SignalsTest)

#include "signalstest.moc"
