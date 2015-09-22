TARGET = testmodelfactorytest
TEMPLATE = app

CONFIG  += console
CONFIG  -= app_bundle
QT      += testlib
QT      += xml

DEFINES += TESTS_DIR=\\\"$$PWD/..\\\"

SOURCES += testmodelfactorytest.cpp

# Files to be tested
SOURCES += ../../plaintextqtestlibparser.cpp \
           ../../plaintextqtestlibparserfactory.cpp \
           ../../qtestlibmodel.cpp \
           ../../testmodelfactory.cpp
HEADERS += ../../plaintextqtestlibparser.h \
           ../../plaintextqtestlibparserfactory.h \
           ../../qtestlibmodel.h \
           ../../testmodelfactory.h

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

DISTFILES += ../../qtestliboutput.dtd
