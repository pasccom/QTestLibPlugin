#ifndef TESTMODELFACTORY_H
#define TESTMODELFACTORY_H

#include <QObject>
#include <QLinkedList>
#include <utils/outputformat.h>

class QAbstractItemModel;

namespace ProjectExplorer {
    class RunControl;
    class RunConfiguration;
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
    virtual TestModelFactory::ParseResult parseStdoutLine(ProjectExplorer::RunControl* runControl, const QString& line) = 0;
    virtual TestModelFactory::ParseResult parseStderrLine(ProjectExplorer::RunControl* runControl, const QString& line) = 0;
    virtual QAbstractItemModel *getModel(void) const = 0;
protected:
    AbstractTestParser(QObject *parent = NULL) :
        QObject(parent) {}
};

class AbstractTestParserFactory : public QObject
{
    Q_OBJECT
public:
    AbstractTestParserFactory(QObject *parent = NULL) :
        QObject(parent) {}
    virtual bool canParse(ProjectExplorer::RunConfiguration *runConfiguration) const = 0;
    virtual AbstractTestParser* getParserInstance(QObject *parent) const = 0;
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // TESTMODELFACTORY_H
