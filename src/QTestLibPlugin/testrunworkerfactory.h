#ifndef TESTRUNWORKERFACTORY_H
#define TESTRUNWORKERFACTORY_H

#include <projectexplorer/runcontrol.h>

namespace QTestLibPlugin {
namespace Internal {

/*!
 * \brief The TestRunWorkerFactory class creates run workers for run configurations
 * associated with tests target (e.g. `make check`).
 *
 * This class support normal and debug run modes on desktop devices.
 *
 * \sa TestRunConfiguration
 */
class TestRunWorkerFactory : public ProjectExplorer::RunWorkerFactory
{
public:
    /*!
     * \brief Constructor
     *
     * Constructs a new run worker factory instance for run configurations
     * associated with test targets (e.g. `make check).
     */
    TestRunWorkerFactory();
};

} // Internal
} // QTestLibPlugin

#endif // TESTRUNWORKERFACTORY_H
