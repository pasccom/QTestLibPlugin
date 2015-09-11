#include <QtTest>
#include <QtDebug>

class AllMessagesTest : public QObject
{
    Q_OBJECT

public:
    inline AllMessagesTest(void) {}

private Q_SLOTS:
    void result(void);
    void pass(void);
    void bpass(void);
    void xpass(void);
    void fail(void);
    void bfail(void);
    void xfail(void);
    void skip(void);
    void qdebug(void);
    void info(void);
    void qsystem(void);
    void warning(void);
    void qwarning(void);
    void qfatal(void);
};

void AllMessagesTest::result(void)
{
    QBENCHMARK {
        QString::localeAwareCompare("qBenchmark reslut test", "qBenchmark reslut test");
    }
}

void AllMessagesTest::pass(void)
{
    QVERIFY2(true, "This test should pass");
}

void AllMessagesTest::bpass(void)
{
    QVERIFY2(true, "This test is blacklisted but it should pass");
}

void AllMessagesTest::xpass(void)
{
    QEXPECT_FAIL("", "This test should pass even if it is marked as expecting failure.", Abort);
    QVERIFY2(true, "This test should succeed");
}

void AllMessagesTest::fail(void)
{
    QVERIFY2(false, "This test should fail");
}

void AllMessagesTest::bfail(void)
{
    QVERIFY2(false, "This test is blacklisted but it should fail");
}

void AllMessagesTest::xfail(void)
{
    QEXPECT_FAIL("", "This test should fail as it is marked as expecting failure.", Abort);
    QVERIFY2(false, "This test should fail");
}

void AllMessagesTest::skip(void)
{
    QSKIP("This test should be skipped");
    QVERIFY2(true, "This test is skipped");
}

void AllMessagesTest::qdebug(void)
{
    qDebug() << "qDebug message";
}

void AllMessagesTest::info(void)
{
    QTest::ignoreMessage(QtDebugMsg, "pascal COMBES");
}

void AllMessagesTest::qsystem(void)
{
    qCritical() << "qCritical message";
}

void AllMessagesTest::warning(void)
{
    QWARN("QWARN message");
}

void AllMessagesTest::qwarning(void)
{
    qWarning() << "qWarning message";
}

void AllMessagesTest::qfatal(void)
{
    qFatal("qFatal message");
}

QTEST_APPLESS_MAIN(AllMessagesTest)

#include "allmessagestest.moc"
