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

TEMPLATE = lib
TARGET   = qtestlibplugin

CONFIG  += c++11
CONFIG  += static
QT      += network
QT      += widgets
QT      += testlib

DEFINES += QTESTLIBPLUGIN_LIBRARY

SOURCES += \
    testoutputpane.cpp \
    testsuitemodel.cpp \
    qtestlibmodel.cpp \
    testmodelfactory.cpp \
    plaintextqtestlibparser.cpp \
    plaintextqtestlibparserfactory.cpp \
    basexmlqtestlibparser.cpp \
    xmlqtestlibparser.cpp \
    xmlqtestlibparserfactory.cpp \
    lightxmlqtestlibparser.cpp \
    lightxmlqtestlibparserfactory.cpp \
    xunitxmlqtestlibparser.cpp \
    xunitxmlqtestlibparserfactory.cpp \
    qtestlibargsparser.cpp \
    testproxymodel.cpp \
    testrunconfiguration.cpp \
    qmaketestrunconfigurationfactory.cpp \
    testrunconfigurationextraaspect.cpp \
    Widgets/filetypevalidatinglineedit.cpp

HEADERS += \
    qtestlibplugin_global.h \
    qtestlibpluginconstants.h \
    testoutputpane.h \
    testsuitemodel.h \
    qtestlibmodel.h \
    testmodelfactory.h \
    plaintextqtestlibparser.h \
    plaintextqtestlibparserfactory.h \
    basexmlqtestlibparser.h \
    xmlqtestlibparser.h \
    xmlqtestlibparserfactory.h \
    lightxmlqtestlibparser.h \
    lightxmlqtestlibparserfactory.h \
    xunitxmlqtestlibparser.h \
    xunitxmlqtestlibparserfactory.h \
    qtestlibargsparser.h \
    testproxymodel.h \
    testrunconfiguration.h \
    qmaketestrunconfigurationfactory.h \
    testrunconfigurationextraaspect.h \
    Widgets/filetypevalidatinglineedit.h

include(../../QTestLibPlugin.pri)

CONFIG(debug, debug|release):DESTDIR = $$QTESTLIBPLUGIN_LIB/debug
else:DESTDIR = $$QTESTLIBPLUGIN_LIB/release

!isEmpty(BUILD_TESTS) {
    DEFINES += BUILD_TESTS
    DEFINES += TESTS_DIR=\\\"$$QTESTLIBPLUGIN_TESTS\\\"
    SOURCES +=  \
        Test/testactionstest.cpp \
        Test/testrunconfigurationfactorytest.cpp
    HEADERS += \
        Test/testactionstest.h \
        Test/testrunconfigurationfactorytest.h
}

# Qt Creator from environment
# Set the QTC_SOURCE environment variable to override the setting here
QTCREATOR_SOURCES = $$(QTC_SOURCE)
# Set the QTC_BUILD environment variable to override the setting here
IDE_BUILD_TREE = $$(QTC_BUILD)

# Qt Creator from local pri file if it exists
exists(../../QtCreator.local.pri) {
    include(../../QtCreator.local.pri)
}

INCLUDEPATH += $$QTCREATOR_SOURCES/src/libs
INCLUDEPATH += $$QTCREATOR_SOURCES/src/plugins
INCLUDEPATH += $$QTCREATOR_SOURCES/src/shared

# The directory where to put MOC-generated files :
MOC_DIR = ./.moc

# Changing output directories :
win32 {
    OBJECTS_DIR = ./.obj_win
}
unix {
    OBJECTS_DIR = ./.obj_unix
}
