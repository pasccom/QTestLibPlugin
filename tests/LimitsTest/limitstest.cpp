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

#include <iostream>

class LimitsTest : public QObject
{
    Q_OBJECT

public:
    LimitsTest(void);
    ~LimitsTest(void);

private Q_SLOTS:
    void p(void);
    void f(void);
    void multiline(void);
    void testRowNames_data(void);
    void testRowNames(void);
};

LimitsTest::LimitsTest(void)
{
    std::cout << "Subreptive constructor stdout message" << std::endl;
    qDebug() << "Subreptive constructor debug message";
}

LimitsTest::~LimitsTest(void)
{
    std::cout << "Subreptive destructor stdout message" << std::endl;
    qDebug() << "Subreptive destructor debug message";
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
