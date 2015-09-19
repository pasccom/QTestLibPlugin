TARGET = testsuitemodeltest
TEMPLATE = app

CONFIG  += console
CONFIG  -= app_bundle
QT      += testlib
QT      += xml

DEFINES += TESTS_DIR=\\\"$$PWD/..\\\"

SOURCES += testsuitemodeltest.cpp

# Files to be tested
SOURCES += ../../plaintextqtestlibparser.cpp \
           ../../qtestlibmodel.cpp \
           ../../testmodelfactory.cpp \
           ../../testsuitemodel.cpp
HEADERS += ../../plaintextqtestlibparser.h \
           ../../qtestlibmodel.h \
           ../../testmodelfactory.h \
           ../../testsuitemodel.h

# QtCreator dependencies
QTCREATOR_DEPS += "projectexplorer"

# Fake QtCreator tree
include(../QtCreatorFake/QtCreatorFake.pri)

# The directory where to put MOC-generated files :
MOC_DIR = ./.moc

# Changing output directories :
win32 {
    OBJECTS_DIR = ./.obj_win
    CONFIG(debug, debug|release):DESTDIR = ./debug
    else:DESTDIR = ./release
}
unix {
    OBJECTS_DIR = ./.obj_unix
    CONFIG(debug, debug|release):DESTDIR = ./debug
    else:DESTDIR = ./release
}

