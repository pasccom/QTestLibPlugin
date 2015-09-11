TARGET = plaintextqtestlibparsertest
TEMPLATE = app

CONFIG  += console
CONFIG  -= app_bundle
QT      += testlib
QT      += xml

DEFINES += TESTS_DIR=\\\"$$PWD/..\\\"

SOURCES += plaintextqtestlibparsertest.cpp

# Allows to include needed files from QtCreator tree
QTCREATOR_SOURCES = $$(QTC_SOURCE)
isEmpty(QTCREATOR_SOURCES):QTCREATOR_SOURCES=/home/pascal/Téléchargements/Logiciels/Dev/qt-creator-opensource-src-3.4.2
INCLUDEPATH += $$QTCREATOR_SOURCES/src/libs

win32 {
    OBJECTS_DIR = ./.obj
}
unix {
    OBJECTS_DIR = ./.obj
}

CONFIG(debug, debug|release):MODE = debug
else: MODE = release

CONFIG(shared):LINKAGE = shared
else:LINKAGE = static

OBJECTS += ../../$${OBJECTS_DIR}/$${MODE}-$${LINKAGE}/plaintextqtestlibparser.o \
           ../../$${OBJECTS_DIR}/$${MODE}-$${LINKAGE}/moc_plaintextqtestlibparser.o \
           ../../$${OBJECTS_DIR}/$${MODE}-$${LINKAGE}/testmodelfactory.o \
           ../../$${OBJECTS_DIR}/$${MODE}-$${LINKAGE}/moc_testmodelfactory.o \
           ../../$${OBJECTS_DIR}/$${MODE}-$${LINKAGE}/qtestlibmodel.o \
           ../../$${OBJECTS_DIR}/$${MODE}-$${LINKAGE}/moc_qtestlibmodel.o

# The directory where to put MOC-generated files :
MOC_DIR = ./.moc

# Changing output directories :
win32 {
     CONFIG(debug, debug|release):DESTDIR = ./debug
     else:DESTDIR = ./release
}
unix {
     CONFIG(debug, debug|release):DESTDIR = ./debug
     else:DESTDIR = ./release
}

DISTFILES += qtestliboutput.dtd

