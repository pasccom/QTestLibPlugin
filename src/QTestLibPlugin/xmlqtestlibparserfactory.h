/* Copyright 2015-2016 Pascal COMBES <pascom@orange.fr>
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

#ifndef XMLQTESTLIBPARSERFACTORY_H
#define XMLQTESTLIBPARSERFACTORY_H

#include "qtestlibpluginconstants.h"
#include "testmodelfactory.h"
#include "xmlqtestlibparser.h"
#include "qtestlibargsparser.h"

#include <projectexplorer/runconfiguration.h>

namespace QTestLibPlugin {
namespace Internal {

/*!
 * \brief The XMLQTestLibParserFactory class is a factory for XMLQTestLibParser
 *
 * As an implementation of AbstractTestParserFactory,
 * this class is in charge of checking that the associated parser, XMLQTestLibParser,
 * may parse the ProjectExplorer::RunConfiguration output and
 * allocate instances of the associated parser if needed.
 * For this it uses the methods of a base factory injected in the constructor.
 */
template<class B>
class XMLQTestLibParserFactory : public AbstractTestParserFactory
{
public:
    /*!
     * \brief Constructor
     *
     * Just initializes members.
     */
    inline XMLQTestLibParserFactory(void) :
        mBase(QTestLibArgsParser::XmlFormat) {}
    /*!
     * \copydoc AbstractTestParserFactory::id()
     */
    inline Core::Id id() const override {return Core::Id(Constants::XmlQTestLibParserFactoryId).withSuffix(mBase.id().toString());}
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
     * \param runConfiguration he run configuration to test
     * \return true, if the associated parser may parse the test output.
     */
    inline bool canParse(ProjectExplorer::RunConfiguration* runConfiguration) const override {return mBase.canParse(runConfiguration);}
    /*!
     * \copydoc AbstractTestParserFactory::getParserInstance()
     */
    AbstractTestParser* getParserInstance(ProjectExplorer::RunConfiguration *runConfiguration) const override;
private:
    B mBase; /*!< The base factory, which provides the implementation of canParse() method */
};

template<class B>
AbstractTestParser* XMLQTestLibParserFactory<B>::getParserInstance(ProjectExplorer::RunConfiguration *runConfiguration) const
{
    Q_ASSERT(runConfiguration != NULL);

    if (!canParse(runConfiguration))
        return NULL;
    qDebug() << "XMLQTestLibParser can parse this file";
    return new XMLQTestLibParser(runConfiguration);
}

} // namespace Internal
} // namespace QTestLibPlugin

#endif // XMLQTESTLIBPARSERFACTORY_H
