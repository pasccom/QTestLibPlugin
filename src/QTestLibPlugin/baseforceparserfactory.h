#ifndef BASEFORCEPARSERFACTORY_H
#define BASEFORCEPARSERFACTORY_H

#include "testmodelfactory.h"
#include "qtestlibargsparser.h"
#include "testoutputpane.h"

namespace QTestLibPlugin {
namespace Internal {

/*!
 * \brief The BaseForceParserFactory class is a base for parsers factories relying user.
 *
 * As an implementation of AbstractTestParserFactory,
 * this class is in charge of checking that a parser
 * may parse the ProjectExplorer::RunConfiguration output.
 * It does not implement the parser creation method,
 * as it is not associated to any particular parser.
 * It is only intended to be injected into a parser factory.
 *
 * It does not use any smart heuristics; it just relies on the user,
 * who can thus force a parser. For that, it uses the combo box located
 * in the tool bar of TestOutputPane.
 */
class BaseForceParserFactory : public AbstractTestParserFactory
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * Just call the parent class constructor.
     *
     * \param format The test output format accepted by the parser
     * \param outputPane The TestOutputPane where the user can force the parsers
     */
    inline BaseForceParserFactory(QTestLibArgsParser::TestOutputFormat format, TestOutputPane *outputPane):
        AbstractTestParserFactory(outputPane), mOutputPane(outputPane), mFormat(format) {}
    /*!
     * \brief \copybrief AbstractTestParserFactory::canParse()
     *
     * Tests whether the project can be parsed by a parser with the given output format.
     * This factory just checks whether the user forced the parser in the TestOutputPane.
     * \param runConfiguration The run configuration to test (unused)
     * \return \c true, if the iuser forced the activation of the parser
     */
    bool canParse(ProjectExplorer::RunConfiguration *runConfiguration) const;
    /*!
     * \brief \copybrief AbstractTestParserFactory::getParserInstance()
     *
     * Always returns \c nullptr, since this factory is a base (not associated with any parser).
     * \param runConfiguration Unused.
     * \return Always \c nullptr.
     */
    inline AbstractTestParser *getParserInstance(ProjectExplorer::RunConfiguration *runConfiguration) const {Q_UNUSED(runConfiguration); return nullptr;};
    /*!
     * \brief Set the format accepted by the parser.
     *
     * Set the format accepted by the parser.
     * \note Child classes must call these method in their constructors.
     * It defaults to QTestLibArgsParser::NoneFormat, which makes an invalid factory.
     * \param acceptedFormat The format accepted by the associated parser.
     */
    inline void setAcceptedFormat(QTestLibArgsParser::TestOutputFormat acceptedFormat) {mFormat = acceptedFormat;}
private:
    TestOutputPane* mOutputPane;                    /*!< The test output pane (where the user can force parsers). */
    QTestLibArgsParser::TestOutputFormat mFormat;   /*!< The test output format accepted by the associated parser. */
};


} // Internal
} // QTestLibPlugin

#endif // BASEFORCEPARSERFACTORY_H
