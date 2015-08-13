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
    explicit PlainTextQTestLibParser(QObject *parent = 0);

    TestModelFactory::ParseResult parseStdoutLine(ProjectExplorer::RunControl* runControl, const QString& line);
    QAbstractItemModel *getModel(void) const {return mModel;}
signals:
    /*void lineParsed(ProjectExplorer::RunControl* runControl,
                    const QString& messageType,
                    const QString& className,
                    const QString& functionName,
                    const QString& rowTitle,
                    const QString& message);*/
private:
    QTestLibModel *mModel;
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // PLAINTEXTQTESTLIBPARSER_H
