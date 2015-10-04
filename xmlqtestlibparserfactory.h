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
    bool canParse(ProjectExplorer::RunConfiguration *runConfiguration) const;
    AbstractTestParser* getParserInstance(QObject *parent = NULL) const {return new XMLQTestLibParser((parent != NULL) ? parent : this->parent());}
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // XMLQTESTLIBPARSERFACTORY_H
