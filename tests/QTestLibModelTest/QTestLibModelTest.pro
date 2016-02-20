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

TARGET = qtestlibmodeltest
TEMPLATE = app

CONFIG  += console
CONFIG  -= app_bundle
CONFIG  += testcase
CONFIG  += no_testcase_installs
QT      += testlib
QT      += xml

include(../../QTestLibPlugin.pri)

DEFINES += TESTS_DIR=\\\"$$QTESTLIBMODEL_TESTS\\\"

SOURCES += qtestlibmodeltest.cpp

# Model tester class
SOURCES += ../common/qtestlibmodeltester.cpp
HEADERS += ../common/qtestlibmodeltester.h

# Files to be tested
SOURCES += $$QTESTLIBPLUGIN_SRC/qtestlibmodel.cpp
HEADERS += $$QTESTLIBPLUGIN_SRC/qtestlibmodel.h
INCLUDEPATH += $$QTESTLIBPLUGIN_SRC

# Dependencies on Qt Creator libs
QTC_LIB_DEPENDS += utils

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
    OBJECTS_DIR = ./.obj_win
    CONFIG(debug, debug|release):DESTDIR = ./debug
    else:DESTDIR = ./release
}

# Script shells to generate tests:
DISTFILES += \
    gentest.sh \
    testlist.sh

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

# Test case files
#DISTFILES += tests/addOneItemNoClass/addoneitemnoclasstest.xml \
#             tests/addOneItemClass/addoneitemclasstest.xml \
#             tests/addOneItemFunction/addoneitemfunctiontest.xml \
#             tests/addOneItemDataRow/addoneitemdatarowtest.xml
# Test result files
#DISTFILES += tests/addOneItemNoClass/addoneitemnoclass.xml \
#             tests/addOneItemClass/addoneitemclass.xml \
#             tests/addOneItemFunction/addoneitemfunction.xml \
#             tests/addOneItemDataRow/addoneitemdatarow.xml
