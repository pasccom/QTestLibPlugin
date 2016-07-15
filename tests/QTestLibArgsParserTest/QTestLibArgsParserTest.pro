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

TARGET = qtestlibargsparsertest
TEMPLATE = app

CONFIG  += console
CONFIG  -= app_bundle
CONFIG  += testcase
CONFIG  += no_testcase_installs
#CONFIG  += gcov
QT      += testlib
QT      += xml


include(../../QTestLibPlugin.pri)

SOURCES += qtestlibargsparsertest.cpp

# Files to be tested
SOURCES += $$QTESTLIBPLUGIN_SRC/qtestlibargsparser.cpp
HEADERS += $$QTESTLIBPLUGIN_SRC/qtestlibargsparser.h
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

include(../../gcov.pri)
