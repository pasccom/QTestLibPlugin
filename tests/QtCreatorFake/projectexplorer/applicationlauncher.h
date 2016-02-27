#ifndef APPLICATIONLAUNCHER_H
#define APPLICATIONLAUNCHER_H

namespace ProjectExplorer {

class ApplicationLauncher : public QObject
{
public:
    enum Mode {
        Console,
        Gui
    };
};

} // ProjectExplorer

#endif // APPLICATIONLAUNCHER_H
