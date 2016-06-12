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

include(../QTestLibPlugin.pri)

TRANSLATIONS += \
    qtestlibplugin_en.ts \
    qtestlibplugin_fr.ts

# Qt Creator from environment
# Set the QTC_SOURCE environment variable to override the setting here
QTCREATOR_SOURCES = $$(QTC_SOURCE)
# Set the QTC_BUILD environment variable to override the setting here
IDE_BUILD_TREE = $$(QTC_BUILD)

# Qt Creator from local pri file if it exists
exists(../QtCreator.local.pri) {
    include(../QtCreator.local.pri)
}

QTC_PLUGIN_NAME=QTestLibPlugin
include($$QTCREATOR_SOURCES/src/qtcreatorplugin.pri)
TARGET      = .target
DESTDIR     =
INSTALLS    =
TEMPLATE    = aux

###### Translation files update (not handled by Qt)
isEmpty(QMAKE_LUPDATE) {
    win32:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]\lupdate.exe
    else:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate
}

for (TRANSLATION, TRANSLATIONS) {
    system("echo -e \"$$TRANSLATION\" >> \"$$QTESTLIBPLUGIN_I18N/translations.lst\"")
}

lupdate.depends += $$QTESTLIBPLUGIN_I18N/sources.lst
lupdate.depends += $$QTESTLIBPLUGIN_I18N/translations.lst
lupdate.commands = $$QMAKE_LUPDATE \"@$$QTESTLIBPLUGIN_I18N/sources.lst\" -ts \"@$$QTESTLIBPLUGIN_I18N/translations.lst\"

QMAKE_EXTRA_TARGETS += lupdate

###### Translation files generation (not handled by Qt)
isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

lrelease.input = TRANSLATIONS
lrelease.output = $$QTESTLIBPLUGIN_BIN/${QMAKE_FILE_BASE}.qm
lrelease.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm $$QTESTLIBPLUGIN_BIN/${QMAKE_FILE_BASE}.qm
lrelease.CONFIG += no_link

QMAKE_EXTRA_COMPILERS += lrelease
POST_TARGETDEPS += compiler_lrelease_make_all

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
