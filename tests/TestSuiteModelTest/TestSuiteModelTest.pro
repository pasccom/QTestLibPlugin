TARGET = testsuitemodeltest
TEMPLATE = app

CONFIG  += console
CONFIG  -= app_bundle
QT      += testlib
QT      += xml

DEFINES += TESTS_DIR=\\\"$$PWD/..\\\"

SOURCES += testsuitemodeltest.cpp

# Model tester class
SOURCES += ../common/qtestlibmodeltester.cpp
HEADERS += ../common/qtestlibmodeltester.h

# Files to be tested
SOURCES += ../../plaintextqtestlibparser.cpp \
           ../../plaintextqtestlibparserfactory.cpp \
           ../../xmlqtestlibparser.cpp \
           ../../xmlqtestlibparserfactory.cpp \
           ../../qtestlibargsparser.cpp \
           ../../qtestlibmodel.cpp \
           ../../testmodelfactory.cpp \
           ../../testsuitemodel.cpp
HEADERS += ../../plaintextqtestlibparser.h \
           ../../plaintextqtestlibparserfactory.h \
           ../../xmlqtestlibparser.h \
           ../../xmlqtestlibparserfactory.h \
           ../../qtestlibargsparser.h \
           ../../qtestlibmodel.h \
           ../../testmodelfactory.h \
           ../../testsuitemodel.h

# Fake QtCreator tree
include(../../QTestLibPlugin_dependencies.pri)
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
