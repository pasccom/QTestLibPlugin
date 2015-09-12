# Ensure orject is compiled in C++ 11
CONFIG += c++11

# Add the root directory of QtCreatorFake to the include path
INCLUDEPATH += $$PWD

# Parse dependencies
contains(QTCREATOR_DEPS, "projectexplorer") {
    SOURCES += $$PWD/projectexplorer/runconfiguration.cpp
    HEADERS += $$PWD/projectexplorer/runconfiguration.h
}
