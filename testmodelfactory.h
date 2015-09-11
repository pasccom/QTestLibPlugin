#ifndef TESTMODELFACTORY_H
#define TESTMODELFACTORY_H

#include <QObject>
#include <QLinkedList>
#include <utils/outputformat.h>

class QAbstractItemModel;

namespace ProjectExplorer {
    class RunControl;
}

namespace QTestLibPlugin {
namespace Internal {


class AbstractTestParser;

class TestModelFactory : public QObject
{
    Q_OBJECT
public:
    TestModelFactory(ProjectExplorer::RunControl *runControl, QObject *parent = NULL);
    typedef enum {
        Unsure,
        MagicFound,
        MagicNotFound
    } ParseResult;
signals:
    void modelFound(QAbstractItemModel *model);
    void modelPopulated(QAbstractItemModel *model);
private slots:
    void parseTestOutput(ProjectExplorer::RunControl* runControl, const QString& msg, Utils::OutputFormat format);
    void runControlFinished(void);
private:
    void parseStdoutLine(ProjectExplorer::RunControl* runControl, const QString& line);
    QLinkedList<AbstractTestParser *> mParsers;
    bool mModelFound;
};

class AbstractTestParser : public QObject
{
    Q_OBJECT
public:
    AbstractTestParser(QObject *parent = NULL) : QObject(parent) {}
    virtual TestModelFactory::ParseResult parseStdoutLine(ProjectExplorer::RunControl* runControl, const QString& line) = 0;
    virtual TestModelFactory::ParseResult parseStderrLine(ProjectExplorer::RunControl* runControl, const QString& line) = 0;
    virtual QAbstractItemModel *getModel(void) const = 0;
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // TESTMODELFACTORY_H
