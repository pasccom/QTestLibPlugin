#ifndef XMLQTESTLIBPARSERFACTORY_H
#define XMLQTESTLIBPARSERFACTORY_H

#include "testmodelfactory.h"
#include "xmlqtestlibparser.h"

namespace QTestLibPlugin {
namespace Internal {

/*!
 * \brief The XMLQTestLibParserFactory class is a factory for XMLQTestLibParser
 *
 * As an implementation of AbstractTestParserFactory,
 * this class is in charge of checking that the associated parser, XMLQTestLibParser,
 * may parse the ProjectExplorer::RunConfiguration output and
 * allocate instances of the associated parser if needed.
 */
class XMLQTestLibParserFactory : public AbstractTestParserFactory
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * Just call the parent class constructor.
     *
     * \param parent The parent object of the factory.
     */
    inline XMLQTestLibParserFactory(QObject *parent = NULL):
        AbstractTestParserFactory(parent) {}
    inline bool canParse(ProjectExplorer::RunConfiguration *runConfiguration) const {return canParseModule(runConfiguration);}
    AbstractTestParser* getParserInstance(ProjectExplorer::RunConfiguration *runConfiguration) const;
private:
    /*!
     * \internal
     * \brief Check if the project uses \c testlib Qt module.
     *
     * As the associated parser can only parse QTestLib output,
     * this method check that the \c testlib module is used by the project.
     * This check should avoid parsing almost all undesired projects.
     *
     * \param runConfiguration The run configuration to check.
     * \return \c true, if the parser may parse the test.
     */
    bool canParseModule(ProjectExplorer::RunConfiguration *runConfiguration) const;
    /*!
     * \internal
     * \brief Check the command line arguments
     *
     * Once the \c testlib module is found, the arguments are checked
     * to see whether the output may be XML.
     *
     * \param runConfiguration The run configuration to check.
     * \return \c true, if the parser may parse the test.
     */
    bool canParseArguments(ProjectExplorer::RunConfiguration *runConfiguration) const;
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // XMLQTESTLIBPARSERFACTORY_H
