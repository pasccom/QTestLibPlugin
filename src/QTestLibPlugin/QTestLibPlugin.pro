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
    Utils/filetypevalidatinglineedit.cpp \
    Test/testactionstest.cpp \
    Test/testrunconfigurationfactorytest.cpp

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
    Utils/filetypevalidatinglineedit.h \
    Test/testactionstest.h \
    Test/testrunconfigurationfactorytest.h

RESOURCES += \
    qtestlibplugin.qrc

TRANSLATIONS += \
    qtestlibplugin_en.ts \
    qtestlibplugin_fr.ts

include(../../QTestLibPlugin.pri)

!isEmpty(BUILD_TESTS) {
    DEFINES += BUILD_TESTS
    DEFINES += TESTS_DIR=\\\"$$QTESTLIBPLUGIN_TESTS\\\"
    SOURCES +=
    HEADERS +=
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

## uncomment to build plugin into user config directory
## <localappdata>/plugins/<ideversion>
##    where <localappdata> is e.g.
##    "%LOCALAPPDATA%\QtProject\qtcreator" on Windows Vista and later
##    "$XDG_DATA_HOME/data/QtProject/qtcreator" or "~/.local/share/data/QtProject/qtcreator" on Linux
##    "~/Library/Application Support/QtProject/Qt Creator" on Mac
USE_USER_DESTDIR = yes

include($$QTCREATOR_SOURCES/src/qtcreatorplugin.pri)
INSTALLS =

###### Translation files generation (not handled by Qt)
isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

updateqm.input = TRANSLATIONS
updateqm.output = $$QTESTLIBPLUGIN_BIN/${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm $$QTESTLIBPLUGIN_BIN/${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link

QMAKE_EXTRA_COMPILERS += updateqm
PRE_TARGETDEPS += compiler_updateqm_make_all

###### Translation files installation (not handled yet by Qt Creator)
COMPILED_TRANSLATIONS =
for (TRANSLATION, TRANSLATIONS) {
    COMPILED_TRANSLATIONS += "$$QTESTLIBPLUGIN_BIN/$$replace(TRANSLATION, ".ts", ".qm")"
}
isEmpty(USE_USER_DESTDIR) {
    translations.path = "$$IDE_DATA_PATH/translations"
} else {
    win32 {
        USERDATAAPPNAME = "qtcreator"
        USERDATABASE = "$$(APPDATA)"
        isEmpty(USERDATABASE):USERDATABASE="$$(USERPROFILE)\Local Settings\Application Data"
    } else:macx {
        USERDATAAPPNAME = "Qt Creator"
        USERDATABASE = "$$(HOME)/Library/Application Support"
    } else:unix {
        USERDATAAPPNAME = "qtcreator"
        USERDATABASE = "$$(XDG_DATA_HOME)"
        isEmpty(USERDATABASE):USERDATABASE = "$$(HOME)/.config"
        else:USERDATABASE = "$$USERDATABASE/data"
    }
    translations.path = "$$USERDATABASE/QtProject/$$USERDATAAPPNAME/translations"
}
translations.files = $$COMPILED_TRANSLATIONS
INSTALLS += translations

###### Special tuning for output dir on Win32
win32 {
    MOC_DIR = ./.moc
    OBJECTS_DIR = ./.obj_win
}

###### Special tunings for DESTDIR on Win32
win32 {
    DESTDIRAPPNAME = "qtcreator"
    DESTDIRBASE = "$$(LOCALAPPDATA)"
    isEmpty(DESTDIRBASE):DESTDIRBASE="$$(USERPROFILE)\Local Settings\Application Data"
    DESTDIR = "$$DESTDIRBASE/data/QtProject/$$DESTDIRAPPNAME/plugins/$$QTCREATOR_VERSION"
}

###### Added stuff to bypass IDE_BUILD_TREE
target.path = $$DESTDIR
INSTALLS += target
DESTDIR = $$QTESTLIBPLUGIN_BIN

unix {
    LIBS+= -L$$IDE_BUILD_TREE
    LIBS+= -L$$IDE_BUILD_TREE/plugins
} else:win32  {
    LIBS+= -L$$IDE_BUILD_TREE/bin
}
