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
    bool canParse(ProjectExplorer::RunConfiguration *runConfiguration) const;
    AbstractTestParser* getParserInstance(QObject *parent = NULL) const {return new PlainTextQTestLibParser((parent != NULL) ? parent : this->parent());}
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // PLAINTEXTQTESTLIBPARSERFACTORY_H
