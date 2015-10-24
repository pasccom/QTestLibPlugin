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
