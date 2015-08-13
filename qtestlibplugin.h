#ifndef QTESTLIBPLUGIN_H
#define QTESTLIBPLUGIN_H

#include "qtestlibplugin_global.h"

#include <extensionsystem/iplugin.h>

namespace QTestLibPlugin {
namespace Internal {

class TestSuiteModel;

class QTestLibPluginPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "QTestLibPlugin.json")

public:
    QTestLibPluginPlugin();
    ~QTestLibPluginPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private slots:
    void triggerAction();
private:
    TestSuiteModel* mModel;
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // QTESTLIBPLUGIN_H

