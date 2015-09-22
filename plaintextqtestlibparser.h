#ifndef PLAINTEXTQTESTLIBPARSER_H
#define PLAINTEXTQTESTLIBPARSER_H

#include "testmodelfactory.h"
#include "qtestlibmodel.h"

namespace ProjectExplorer {
    class RunControl;
}

namespace QTestLibPlugin {
namespace Internal {

class QTestLibModel;

class PlainTextQTestLibParser : public AbstractTestParser
{
    Q_OBJECT
public:
    TestModelFactory::ParseResult parseStdoutLine(ProjectExplorer::RunControl* runControl, const QString& line);
    TestModelFactory::ParseResult parseStderrLine(ProjectExplorer::RunControl* runControl, const QString& line);
    QAbstractItemModel *getModel(void) const {return mModel;}
protected:
    PlainTextQTestLibParser(QObject *parent = 0) :
        AbstractTestParser(parent), mModel(NULL) {}
private:
    QTestLibModel *mModel;

    friend class PlainTextQTestLibParserFactory;
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // PLAINTEXTQTESTLIBPARSER_H
