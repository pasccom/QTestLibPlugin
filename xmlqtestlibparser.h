#ifndef XMLQTESTLIBPARSER_H
#define XMLQTESTLIBPARSER_H

#include "testmodelfactory.h"
#include "qtestlibmodel.h"

#include <QStack>
#include <QMap>
#include <QXmlStreamReader>

namespace ProjectExplorer {
    class RunControl;
}

namespace QTestLibPlugin {
namespace Internal {

class QTestLibModel;

class XMLQTestLibParser : public AbstractTestParser
{
    Q_OBJECT
public:
    inline ~XMLQTestLibParser(void) {delete mReader;}
    TestModelFactory::ParseResult parseStdoutLine(ProjectExplorer::RunControl* runControl, const QString& line);
    TestModelFactory::ParseResult parseStderrLine(ProjectExplorer::RunControl* runControl, const QString& line);
    QAbstractItemModel *getModel(void) const {return mModel;}
protected:
    XMLQTestLibParser(QObject *parent = NULL);
private:
    TestModelFactory::ParseResult startElementParsed(ProjectExplorer::RunControl* runControl, const QStringRef& tag);
    TestModelFactory::ParseResult endElementParsed(ProjectExplorer::RunControl* runControl, const QStringRef& tag);
    TestModelFactory::ParseResult textParsed(ProjectExplorer::RunControl* runControl);
    void saveAttributes(const QXmlStreamAttributes& attrs);
    QTestLibModel::MessageType currentMessageType(void);

    QTestLibModel *mModel;
    QXmlStreamReader *mReader;
    bool mParserActive;

    QString mCurrentClass;
    QString mCurrentFunction;
    QString mCurrentRow;
    QString mCurrentDescription;

    QStack<QString> mCurrentElement;
    QMap<QString, QString> mCurrentAttributes;

    QString mQtVersion;
    QString mQtBuild;
    QString mQTestLibVersion;

    friend class XMLQTestLibParserFactory;
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // XMLQTESTLIBPARSER_H
