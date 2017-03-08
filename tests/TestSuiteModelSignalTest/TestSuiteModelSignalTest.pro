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

TARGET = testsuitemodelsignaltest
TEMPLATE = app

CONFIG  += console
CONFIG  -= app_bundle
CONFIG  += testcase
CONFIG  += no_testcase_installs
QT      += testlib
QT      += xml
QT      += widgets
QT      += network

include(../../QTestLibPlugin.pri)

DEFINES += TESTS_DIR=\\\"$$QTESTLIBMODEL_TESTS\\\"

SOURCES += testsuitemodelsignaltest.cpp

# The tester libraries
CONFIG(release, debug|release) {
    LIBS += ../common/release/libtestcommon.a
    PRE_TARGETDEPS += ../common/release/libtestcommon.a
} else {
    LIBS += ../common/debug/libtestcommon.a
    PRE_TARGETDEPS += ../common/debug/libtestcommon.a
}

# Files to be tested
SOURCES += $$QTESTLIBPLUGIN_SRC/qtestlibmodel.cpp \
           $$QTESTLIBPLUGIN_SRC/testmodelfactory.cpp \
           $$QTESTLIBPLUGIN_SRC/testsuitemodel.cpp
HEADERS += $$QTESTLIBPLUGIN_SRC/qtestlibmodel.h \
           $$QTESTLIBPLUGIN_SRC/testmodelfactory.h \
           $$QTESTLIBPLUGIN_SRC/testsuitemodel.h
INCLUDEPATH += $$QTESTLIBPLUGIN_SRC

# QtCreator tree
include(../../QtCreator.local.pri)
include($$QTESTLIBPLUGIN_LIB/QTestLibPlugin_dependencies.pri)
include($$QTCREATOR_SOURCES/qtcreator.pri)
DEFINES -= QT_CREATOR QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_ASCII QT_RESTRICTED_CAST_FROM_ASCII

QMAKE_RPATHDIR += "$$IDE_PLUGIN_PATH"
QMAKE_RPATHDIR += "$$IDE_LIBRARY_PATH"
win32 {
    LIBS += "-L$$IDE_BUILD_TREE/bin"
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

# Script shells to generate tests:
DISTFILES += \
    ../QTestLibModelTest/gentest.sh \
    ../QTestLibModelTest/testlist.sh

# Include test file if found:
exists($$QTESTLIBMODEL_TESTS/tests.pri) {
    include($$QTESTLIBMODEL_TESTS/tests.pri)
} else {
    message("Test list file not found. Run \"$$QTESTLIBMODEL_TESTS/testlist.sh\"")
}
# Include test result file if found:
exists($$QTESTLIBMODEL_TESTS/results.pri) {
    include($$QTESTLIBMODEL_TESTS/results.pri)
} else {
    message("Test result list file not found. Run \"$$QTESTLIBMODEL_TESTS/testlist.sh\"")
}
# Test if test data file exists:
exists($$QTESTLIBMODEL_TESTS/tests.lst) {
    DEFINES += TESTS_LST_EXISTS
} else {
    message("Test data file not found. Run \"$$QTESTLIBMODEL_TESTS/testlist.sh\"")
}
