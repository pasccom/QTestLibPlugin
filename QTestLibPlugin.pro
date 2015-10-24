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

DEFINES += QTESTLIBPLUGIN_LIBRARY

# QTestLibPlugin files

SOURCES += \
    testoutputpane.cpp \
    testsuitemodel.cpp \
    qtestlibmodel.cpp \
    qtestlibplugin.cpp \
    testmodelfactory.cpp \
    plaintextqtestlibparser.cpp \
    plaintextqtestlibparserfactory.cpp \
    xmlqtestlibparser.cpp \
    xmlqtestlibparserfactory.cpp \
    qtestlibargsparser.cpp

HEADERS += \
        qtestlibplugin_global.h \
        qtestlibpluginconstants.h \
    testoutputpane.h \
    testsuitemodel.h \
    qtestlibmodel.h \
    qtestlibplugin.h \
    testmodelfactory.h \
    plaintextqtestlibparser.h \
    plaintextqtestlibparserfactory.h \
    xmlqtestlibparser.h \
    xmlqtestlibparserfactory.h \
    qtestlibargsparser.h

RESOURCES += \
    qtestlibplugin.qrc

# Qt Creator from environment
# Set the QTC_SOURCE environment variable to override the setting here
QTCREATOR_SOURCES = $$(QTC_SOURCE)
# Set the QTC_BUILD environment variable to override the setting here
IDE_BUILD_TREE = $$(QTC_BUILD)

# Qt Creator from local pri file if it exists
exists(QtCreator.local.pri) {
    include(QtCreator.local.pri)
}

## uncomment to build plugin into user config directory
## <localappdata>/plugins/<ideversion>
##    where <localappdata> is e.g.
##    "%LOCALAPPDATA%\QtProject\qtcreator" on Windows Vista and later
##    "$XDG_DATA_HOME/data/QtProject/qtcreator" or "~/.local/share/data/QtProject/qtcreator" on Linux
##    "~/Library/Application Support/QtProject/Qt Creator" on Mac
USE_USER_DESTDIR = yes

include($$QTCREATOR_SOURCES/src/qtcreatorplugin.pri)

###### Added stuff to bypass IDE_BUILD_TREE
target.path = $$DESTDIR
INSTALLS =
INSTALLS += target
DESTDIR = ./bin

LIBS+= -L$$IDE_BUILD_TREE
LIBS+= -L$$IDE_BUILD_TREE/plugins
