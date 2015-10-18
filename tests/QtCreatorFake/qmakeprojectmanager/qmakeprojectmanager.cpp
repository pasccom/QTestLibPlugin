#include "qmakeprojectmanager.h"

#include <qmakeparser.h>
#include <profileevaluator.h>

namespace QmakeProjectManager {

void QmakeProjectManager::initialize(void)
{
    QMakeParser::initialize();
    ProFileEvaluator::initialize();
}

} // QmakeProject
