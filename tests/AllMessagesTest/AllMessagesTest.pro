TARGET  = allmessagestest
TEMPLATE = app

QT       += testlib
QT       -= gui
CONFIG += console
CONFIG -= app_bundle

SOURCES += allmessagestest.cpp

DISTFILES += BLACKLIST \
             allmessagestest.xml

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
