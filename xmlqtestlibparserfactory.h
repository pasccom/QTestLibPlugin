#ifndef XMLQTESTLIBPARSERFACTORY_H
#define XMLQTESTLIBPARSERFACTORY_H

#include "testmodelfactory.h"
#include "xmlqtestlibparser.h"

namespace QTestLibPlugin {
namespace Internal {

class XMLQTestLibParserFactory : public AbstractTestParserFactory
{
    Q_OBJECT
public:
    inline XMLQTestLibParserFactory(QObject *parent = NULL):
        AbstractTestParserFactory(parent) {}
    inline bool canParse(ProjectExplorer::RunConfiguration *runConfiguration) const {return canParseModule(runConfiguration);}
    AbstractTestParser* getParserInstance(ProjectExplorer::RunConfiguration *runConfiguration) const;
private:
    bool canParseModule(ProjectExplorer::RunConfiguration *runConfiguration) const;
    bool canParseArguments(ProjectExplorer::RunConfiguration *runConfiguration) const;
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // XMLQTESTLIBPARSERFACTORY_H
