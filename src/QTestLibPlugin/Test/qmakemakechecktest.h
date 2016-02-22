#ifndef QMAKEMAKECHECKTEST_H
#define QMAKEMAKECHECKTEST_H

#include <QObject>

namespace QTestLibPlugin {
namespace Test {

class QMakeMakeCheckTest : public QObject
{
    Q_OBJECT
public:
    QMakeMakeCheckTest(void);

private Q_SLOTS:
    void testOpenProjectWithTests_data(void);
    void testOpenProjectWithTests(void);
    void testOpenProjectWithoutTests_data(void);
    void testOpenProjectWithoutTests(void);
};

} // Test
} // QTestLibPlugin

#endif // QMAKEMAKECHECKTEST_H
