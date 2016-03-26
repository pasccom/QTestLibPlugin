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

include(../../QTestLibPlugin.pri)

QT += xml

DEFINES += QTESTLIBPLUGIN_LIBRARY

SOURCES += qtestlibplugin.cpp
HEADERS += qtestlibplugin.h
RESOURCES += \
    qtestlibplugin.qrc
INCLUDEPATH += $$QTESTLIBPLUGIN_SRC

CONFIG(debug, debug|release) {
    LIBS += debug/libqtestlibplugin.a
    PRE_TARGETDEPS += debug/libqtestlibplugin.a
} else {
    LIBS += release/libqtestlibplugin.a
    PRE_TARGETDEPS += release/libqtestlibplugin.a
}

TRANSLATIONS += \
    $$QTESTLIBPLUGIN_I18N/qtestlibplugin_en.ts \
    $$QTESTLIBPLUGIN_I18N/qtestlibplugin_fr.ts

!isEmpty(BUILD_TESTS) {
    DEFINES += BUILD_TESTS
    INCLUDEPATH += $$QTESTLIBPLUGIN_TESTS
    CONFIG(debug, debug|release) {
        LIBS += debug/libqtestlibplugintest.a
        PRE_TARGETDEPS += debug/libqtestlibplugintest.a
    } else {
        LIBS += release/libqtestlibplugintest.a
        PRE_TARGETDEPS += release/libqtestlibplugintest.a
    }
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

###### Translation files update (not handled by Qt)
isEmpty(QMAKE_LUPDATE) {
    win32:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]\lupdate.exe
    else:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate
}


for (SOURCE, SOURCES) {
    system("echo -e \"$$PWD/$$SOURCE\" >> \"$$QTESTLIBPLUGIN_I18N/sources.lst\"")
}
for (HEADER, HEADERS) {
    system("echo -e \"$$PWD/$$HEADER\" >> \"$$QTESTLIBPLUGIN_I18N/sources.lst\"")
}
for (TRANSLATION, TRANSLATIONS) {
    system("echo -e \"$$TRANSLATION\" >> \"$$QTESTLIBPLUGIN_I18N/translations.lst\"")
}

#system("sort \"$$QTESTLIBPLUGIN_I18N/sources.lst\" | uniq > \"$$QTESTLIBPLUGIN_I18N/sources.lst.tmp\"")
#system("mv \"$$QTESTLIBPLUGIN_I18N/sources.lst.tmp\" \"$$QTESTLIBPLUGIN_I18N/sources.lst\"")

#system("sort \"$$QTESTLIBPLUGIN_I18N/translations.lst\" | uniq > \"$$QTESTLIBPLUGIN_I18N/translations.lst.tmp\"")
#system("mv \"$$QTESTLIBPLUGIN_I18N/translations.lst.tmp\" \"$$QTESTLIBPLUGIN_I18N/translations.lst\"")

lupdate.depends += QTestLibPlugin.pro
lupdate.depends += $$SOURCES
lupdate.depends += $$HEADERS
lupdate.commands = $$QMAKE_LUPDATE \"@$$QTESTLIBPLUGIN_I18N/sources.lst\" -ts \"@$$QTESTLIBPLUGIN_I18N/translations.lst\"

QMAKE_EXTRA_TARGETS += lupdate

###### Translation files generation (not handled by Qt)
isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

lrelease.input = TRANSLATIONS
lrelease.output = $$QTESTLIBPLUGIN_BIN/${QMAKE_FILE_BASE}.qm
updateqm.name = lrelease ${QMAKE_FILE_IN}
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

###### Special tuning for output dir on Win32
win32 {
    MOC_DIR = ./.moc
    OBJECTS_DIR = ./.obj_win
}

###### Special tunings for DESTDIR on Win32
# Bug in QtCreator source from commit 1d5091e48f6df341dbd4436843c25afe25cbf4ce
# corrected in 7ea6b21b6d9480f6ed5975d75b2d83273856944a (included in v3.5.1)
defineTest(qtCreatorVersionLessThan) {
    # Extracts major, minor and patch numbers
    QTC_VERSION=$$split(QTCREATOR_VERSION, .)
    MAJOR_QTC_VERSION=$$member(QTC_VERSION, 0)
    MINOR_QTC_VERSION=$$member(QTC_VERSION, 1)
    PATCH_QTC_VERSION=$$member(QTC_VERSION, 2)
    MAX_VERSION=$$1
    MAX_VERSION=$$split(MAX_VERSION, .)
    MAJOR_MAX_VERSION=$$member(MAX_VERSION, 0)
    MINOR_MAX_VERSION=$$member(MAX_VERSION, 1)
    PATCH_MAX_VERSION=$$member(MAX_VERSION, 2)
    #message(QTC: M:$$MAJOR_QTC_VERSION m:$$MINOR_QTC_VERSION p:$$PATCH_QTC_VERSION)
    #message(MAX: M:$$MAJOR_MAX_VERSION m:$$MINOR_MAX_VERSION p:$$PATCH_MAX_VERSION)
    # Test if QtCreator version is smaller than MAX
    greaterThan(MAJOR_MAX_VERSION, $$MAJOR_QTC_VERSION) {
        return(true)
    }
    isEqual(MAJOR_MAX_VERSION, $$MAJOR_QTC_VERSION) {
        greaterThan(MINOR_MAX_VERSION, $$MINOR_QTC_VERSION) {
            return(true)
        }
        isEqual(MINOR_MAX_VERSION, $$MINOR_QTC_VERSION) {
            greaterThan(PATCH_MAX_VERSION, $$PATCH_QTC_VERSION) {
                return(true)
            }
        }
    }
    return(false)
}

win32 {
    qtCreatorVersionLessThan(3.5.1) {
        !qtCreatorVersionLessThan(3.3.0) {
        message("Corrected bug affecting windows between verions 3.3.0 included and 3.5.1 excluded")
        DESTDIRAPPNAME = "qtcreator"
        DESTDIRBASE = "$$(LOCALAPPDATA)"
        isEmpty(DESTDIRBASE):DESTDIRBASE="$$(USERPROFILE)\Local Settings\Application Data"
        DESTDIR = "$$DESTDIRBASE/data/QtProject/$$DESTDIRAPPNAME/plugins/$$QTCREATOR_VERSION"
        }
    }
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
