/* Copyright 2015-2016 Pascal COMBES <pascom@orange.fr>
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
