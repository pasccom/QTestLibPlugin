/* Copyright 2015-2020 Pascal COMBES <pascom@orange.fr>
 * 
 * This file is part of QTestLibPlugin.
 * 
 * QTestLibPlugin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * QTestLibPlugin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with QTestLibPlugin. If not, see <http://www.gnu.org/licenses/>
 */

#ifndef PLAINTEXTQTESTLIBPARSERFACTORY_H
#define PLAINTEXTQTESTLIBPARSERFACTORY_H

#include "qtestlibpluginconstants.h"
#include "testmodelfactory.h"
#include "plaintextqtestlibparser.h"
#include "qtestlibargsparser.h"

#include <projectexplorer/runcontrol.h>

namespace QTestLibPlugin {
namespace Internal {

/*!
 * \brief The PlainTextQTestLibParserFactory class is a factory for PlainTextQTestLibParser
 *
 * As an implementation of AbstractTestParserFactory,
 * this class is in charge of checking that the associated parser, PlainTextQTestLibParser,
 * may parse the ProjectExplorer::RunControl output and
 * allocate instances of the associated parser if needed.
 * For this it uses the methods of a base factory injected in the constructor.
 */
template<class B>
class PlainTextQTestLibParserFactory : public AbstractTestParserFactory
{
public:
    /*!
     * \brief Constructor
     *
     * Just initializes members.
     */
    inline PlainTextQTestLibParserFactory(void) :
        mBase(QTestLibArgsParser::TxtFormat) {}
    /*!
     * \copydoc AbstractTestParserFactory::id()
     */
    inline Utils::Id id() const override {return Utils::Id(Constants::PlainTextQTestLibParserFactoryId).withSuffix(mBase.id().toString());}
    /*!
     * \brief Base factory
     *
     * Returns the base factory (used by the canParse() method).
     * \return The base factory
     */
    inline B& base(void) {return mBase;}
    /*!
     * \brief \copybrief AbstractTestParserFactory::canParse()
     *
     * Uses the base factory canParse() method, to tell whether the associated parser
     * can parse the test.
     * \param runControl The run control to test
     * \return true, if the associated parser may parse the test output.
     */
    inline bool canParse(ProjectExplorer::RunControl* runControl) const override {return mBase.canParse(runControl);}
    /*!
     * \copydoc AbstractTestParserFactory::getParserInstance()
     */
    AbstractTestParser* getParserInstance(ProjectExplorer::RunControl* runControl) const override;
private:
    B mBase; /*!< The base factory, which provides the implementation of canParse() method */
};

template<class B>
AbstractTestParser* PlainTextQTestLibParserFactory<B>::getParserInstance(ProjectExplorer::RunControl* runControl) const
{
    Q_ASSERT(runControl != nullptr);

    if (!canParse(runControl))
        return NULL;
    qDebug() << "PlainTextQTestLibParser can parse this file";
    return new PlainTextQTestLibParser(runControl);
}


} // namespace Internal
} // namespace QTestLibPlugin

#endif // PLAINTEXTQTESTLIBPARSERFACTORY_H
