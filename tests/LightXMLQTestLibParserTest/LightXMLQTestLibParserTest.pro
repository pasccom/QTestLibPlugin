# Copyright 2015 Pascal COMBES <pascom@orange.fr>
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

TARGET = lightxmlqtestlibparsertest
TEMPLATE = app

CONFIG  += console
CONFIG  -= app_bundle
CONFIG  += testcase
CONFIG  += no_testcase_installs
QT      += testlib
QT      += xml
QT      += widgets

include(../../QTestLibPlugin.pri)

DEFINES += TESTS_DIR=\\\"$$QTESTLIBPLUGIN_TESTS\\\"

SOURCES += lightxmlqtestlibparsertest.cpp

# Model tester class
SOURCES += ../common/qtestlibmodeltester.cpp
HEADERS += ../common/qtestlibmodeltester.h

# Files to be tested
SOURCES += $$QTESTLIBPLUGIN_SRC/basexmlqtestlibparser.cpp \
           $$QTESTLIBPLUGIN_SRC/lightxmlqtestlibparser.cpp \
           $$QTESTLIBPLUGIN_SRC/lightxmlqtestlibparserfactory.cpp \
           $$QTESTLIBPLUGIN_SRC/qtestlibargsparser.cpp \
           $$QTESTLIBPLUGIN_SRC/qtestlibmodel.cpp \
           $$QTESTLIBPLUGIN_SRC/testmodelfactory.cpp \
           $$QTESTLIBPLUGIN_SRC/testrunconfiguration.cpp \
           $$QTESTLIBPLUGIN_SRC/testrunconfigurationextraaspect.cpp \
           $$QTESTLIBPLUGIN_SRC/Widgets/filetypevalidatinglineedit.cpp
HEADERS += $$QTESTLIBPLUGIN_SRC/basexmlqtestlibparser.h \
           $$QTESTLIBPLUGIN_SRC/lightxmlqtestlibparser.h \
           $$QTESTLIBPLUGIN_SRC/lightxmlqtestlibparserfactory.h \
           $$QTESTLIBPLUGIN_SRC/qtestlibargsparser.h \
           $$QTESTLIBPLUGIN_SRC/qtestlibmodel.h \
           $$QTESTLIBPLUGIN_SRC/testmodelfactory.h \
           $$QTESTLIBPLUGIN_SRC/testrunconfiguration.h \
           $$QTESTLIBPLUGIN_SRC/testrunconfigurationextraaspect.h \
           $$QTESTLIBPLUGIN_SRC/Widgets/filetypevalidatinglineedit.h
INCLUDEPATH += $$QTESTLIBPLUGIN_SRC

# Fake QtCreator tree
include($$QTESTLIBPLUGIN_LIB/QTestLibPlugin_dependencies.pri)
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
    OBJECTS_DIR = ./.obj_win
    CONFIG(debug, debug|release):DESTDIR = ./debug
    else:DESTDIR = ./release
}

DISTFILES += ../../qtestliboutput.dtd
