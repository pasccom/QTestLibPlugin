/* Copyright 2015-2020 Pascal COMBES <pascom@orange.fr>
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

#ifndef TESTSUITEMODELTEST_H
#define TESTSUITEMODELTEST_H

#include <QObject>
#include <common/qttestsubfunction.h>
#include <common/qtestlibmodeltester.h>

namespace ProjectExplorer {
    class Project;
}

namespace QTestLibPlugin {

namespace Internal {
    class TestSuiteModel;
}

namespace Test {

class TestSuiteModelTest : public QObject
{
    Q_OBJECT
    HAS_SUB_TEST_FUNCTIONS
public:
    TestSuiteModelTest(void);
private Q_SLOTS:
    void init();
    void cleanup();

    void testZeroRemoveBad(void);
    void testZeroClear(void);

    void testAppendOne(void);
    void testAppendOneRemoveOne(void);
    void testRemoveBadAppendOne(void);
    void testAppendOneRemoveBad(void);
    void testAppendOneClear(void);

    void testAppendTwo(void);
    void testAppendTwoRemoveFirst(void);
    void testAppendTwoRemoveSecond(void);
    void testAppendTwoRemoveBad(void);
    void testAppendOneRemoveAppendOne(void);
    void testAppendOneRemoveBadAppendOne(void);
    void testAppendTwoClear(void);
    void testAppendOneClearAppendOne(void);
private:
    struct TestRunData {
        QString testName;
        QString parserFormat;
        QTestLibModelTester::Verbosity testVerbosity;

        inline TestRunData(const QString& name, const QString& format, QTestLibModelTester::Verbosity verbosity) :
            testName(name), parserFormat(format), testVerbosity(verbosity) {}
    };

    void parseSuiteRoot(const QAbstractItemModel* model, const QList< TestRunData* >& tests);

    void checkSignalArguments(const QString& signal, const QList<QVariant>& args, int first, int last);
    void appendTest(QTestLibPlugin::Internal::TestSuiteModel *model, const QString& test, const QString& format, QTestLibModelTester::Verbosity verbosity);
    void removeTestAt(QTestLibPlugin::Internal::TestSuiteModel *model, int i);
    void clearTests(QTestLibPlugin::Internal::TestSuiteModel *model);

    QStringList commandLineArguments(const QString& format, QTestLibModelTester::Verbosity verbosity);

    QStringList mTests;
    QStringList mParserFormats;

    QLinkedList<ProjectExplorer::Project*> mOpenProjects;
};

} // Test
} // QTestLibPlugin

#endif // TESTSUITEMODELTEST_H
