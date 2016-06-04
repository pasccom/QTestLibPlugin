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

TEMPLATE = lib
TARGET   = qtestlibplugintest

CONFIG  += c++11
CONFIG  += static
QT      += network
QT      += widgets
QT      += testlib
QT      += xml

SOURCES +=  \
    testhelper.cpp \
    testmodelfactorytest.cpp \
    testsuitemodeltest.cpp \
    testactionstest.cpp \
    testrunconfigurationfactorytest.cpp \
    qmakelightxmlqtestlibparserfactorytest.cpp \
    forcelightxmlqtestlibparserfactorytest.cpp \
    qmakeplaintextqtestlibparserfactorytest.cpp \
    forceplaintextqtestlibparserfactorytest.cpp \
    qmakexmlqtestlibparserfactorytest.cpp \
    forcexmlqtestlibparserfactorytest.cpp \
    qmakexunitxmlqtestlibparserfactorytest.cpp \
    forcexunitxmlqtestlibparserfactorytest.cpp \

HEADERS += \
    testhelper.h \
    testmodelfactorytest.h \
    testsuitemodeltest.h \
    testactionstest.h \
    testrunconfigurationfactorytest.h \
    qmakelightxmlqtestlibparserfactorytest.h \
    forcelightxmlqtestlibparserfactorytest.h \
    qmakeplaintextqtestlibparserfactorytest.h \
    forceplaintextqtestlibparserfactorytest.h \
    qmakexmlqtestlibparserfactorytest.h \
    forcexmlqtestlibparserfactorytest.h \
    qmakexunitxmlqtestlibparserfactorytest.h \
    forcexunitxmlqtestlibparserfactorytest.h \

include(../../../QTestLibPlugin.pri)
SOURCES += $$QTESTLIBPLUGIN_TESTS/common/qtestlibmodeltester.cpp
HEADERS += $$QTESTLIBPLUGIN_TESTS/common/qtestlibmodeltester.h

INCLUDEPATH += $$QTESTLIBPLUGIN_SRC
INCLUDEPATH += $$QTESTLIBPLUGIN_TESTS

DEFINES += BUILD_TESTS
DEFINES += TESTS_DIR=\\\"$$QTESTLIBPLUGIN_TESTS\\\"

CONFIG(debug, debug|release):DESTDIR = $$QTESTLIBPLUGIN_LIB/debug
else:DESTDIR = $$QTESTLIBPLUGIN_LIB/release

# Qt Creator from environment
# Set the QTC_SOURCE environment variable to override the setting here
QTCREATOR_SOURCES = $$(QTC_SOURCE)
# Set the QTC_BUILD environment variable to override the setting here
IDE_BUILD_TREE = $$(QTC_BUILD)

# Qt Creator from local pri file if it exists
exists(../../../QtCreator.local.pri) {
    include(../../../QtCreator.local.pri)
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

