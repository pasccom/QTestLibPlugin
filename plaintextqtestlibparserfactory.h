#ifndef PLAINTEXTQTESTLIBPARSERFACTORY_H
#define PLAINTEXTQTESTLIBPARSERFACTORY_H

#include "testmodelfactory.h"
#include "plaintextqtestlibparser.h"

namespace QTestLibPlugin {
namespace Internal {

class PlainTextQTestLibParserFactory : public AbstractTestParserFactory
{
    Q_OBJECT
public:
    inline PlainTextQTestLibParserFactory(QObject *parent = NULL):
        AbstractTestParserFactory(parent) {}
    inline bool canParse(ProjectExplorer::RunConfiguration *runConfiguration) const {return canParseModule(runConfiguration);}
    AbstractTestParser* getParserInstance(ProjectExplorer::RunConfiguration *runConfiguration) const;
private:
    bool canParseModule(ProjectExplorer::RunConfiguration *runConfiguration) const;
    bool canParseArguments(ProjectExplorer::RunConfiguration *runConfiguration) const;
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // PLAINTEXTQTESTLIBPARSERFACTORY_H
