# Copyright 2015-2016 Pascal COMBES <pascom@orange.fr>
# 
# This file is part of QTestLibPlugin.
# 
# QTestLibPlugin is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# QTestLibPlugin is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with QTestLibPlugin. If not, see <http://www.gnu.org/licenses/>

TARGET = plaintextqtestlibparsertest
TEMPLATE = app

CONFIG  += console
CONFIG  -= app_bundle
CONFIG  += testcase
CONFIG  += no_testcase_installs
#CONFIG  += gcov
QT      += testlib
QT      += xml
QT      += widgets
QT      += network

include(../../QTestLibPlugin.pri)

DEFINES += TESTS_DIR=\\\"$$QTESTLIBPLUGIN_TESTS\\\"

include(../../make.pri)

# Test files
SOURCES += plaintextqtestlibparsertest.cpp \
           plaintextqtestlibparserfactoryfake.cpp
HEADERS += plaintextqtestlibparserfactoryfake.h

# The tester libraries
CONFIG(release, debug|release) {
    LIBS += ../common/release/libtestcommon.a
    PRE_TARGETDEPS += ../common/release/libtestcommon.a
} else {
    LIBS += ../common/debug/libtestcommon.a
    PRE_TARGETDEPS += ../common/debug/libtestcommon.a
}

# The libraries to test
CONFIG(gcov) {
    SOURCES += $$QTESTLIBPLUGIN_SRC/plaintextqtestlibparser.cpp
    HEADERS += $$QTESTLIBPLUGIN_SRC/plaintextqtestlibparser.h
}
CONFIG(release, debug|release) {
    LIBS += $$QTESTLIBPLUGIN_LIB/release/libqtestlibplugin.a
    PRE_TARGETDEPS += $$QTESTLIBPLUGIN_LIB/release/libqtestlibplugin.a
} else {
    LIBS += $$QTESTLIBPLUGIN_LIB/debug/libqtestlibplugin.a
    PRE_TARGETDEPS += $$QTESTLIBPLUGIN_LIB/debug/libqtestlibplugin.a
}

# Files to be tested are in src folder
INCLUDEPATH += $$QTESTLIBPLUGIN_SRC

# QtCreator tree
include(../../QtCreator.local.pri)
include($$QTESTLIBPLUGIN_LIB/QTestLibPlugin_dependencies.pri)
include($$QTCREATOR_SOURCES/qtcreator.pri)
DEFINES -= QT_CREATOR QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_ASCII
unix {
    LIBS += "-Wl,-rpath=$$IDE_PLUGIN_PATH"
    LIBS += "-Wl,-rpath=$$IDE_LIBRARY_PATH"
}

# The directory where to put MOC-generated files
MOC_DIR = ./.moc

# Changing output directories
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

include(../../gcov.pri)

DISTFILES += ../../qtestliboutput.dtd

