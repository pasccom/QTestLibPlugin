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

for (SOURCE, SOURCES) {
    system("echo -e \"$$PWD/$$SOURCE\" >> \"$$QTESTLIBPLUGIN_I18N/sources.lst\"")
}
for (HEADER, HEADERS) {
    system("echo -e \"$$PWD/$$HEADER\" >> \"$$QTESTLIBPLUGIN_I18N/sources.lst\"")
}

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

include($$QTCREATOR_SOURCES/src/qtcreatorplugin.pri)
INSTALLS =

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
    !isEmpty(USE_USER_DESTDIR) {
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
