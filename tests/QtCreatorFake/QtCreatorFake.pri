# Ensure orject is compiled in C++ 11
CONFIG += c++11

# Add the root directory of QtCreatorFake to the include path
INCLUDEPATH += $$PWD

# Parse dependencies ##########################################################
# Project explorer
contains(QTCREATOR_DEPS, "projectexplorer") {
    SOURCES += $$PWD/projectexplorer/runconfiguration.cpp
    HEADERS += $$PWD/projectexplorer/runconfiguration.h
}
# Utils
contains(QTCREATOR_DEPS, "utils") {
    SOURCES += $$PWD/utils/fileutils.cpp
    HEADERS += $$PWD/utils/fileutils.h
}
