# Copyright 2016 Pascal COMBES <pascom@orange.fr>
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

# Initialisation ##############################################################
# Ensure object is compiled in C++ 11
CONFIG += c++11

HEADERS += $$PWD/qtcreatorfake_global.h

# Needeed functions ###########################################################
defineReplace(qtLibraryName) {
   unset(LIBRARY_NAME)
   LIBRARY_NAME = $$1
   CONFIG(debug, debug|release) {
      !debug_and_release|build_pass {
          mac:RET = $$member(LIBRARY_NAME, 0)_debug
          else:win32:RET = $$member(LIBRARY_NAME, 0)d
      }
   }
   isEmpty(RET):RET = $$LIBRARY_NAME
   return($$RET)
}

# Pathes for libs and fake plugins ############################################
# Qt Creator from environment
# Set the QTC_SOURCE environment variable to override the setting here
QTCREATOR_SOURCES = $$(QTC_SOURCE)
# Set the QTC_BUILD environment variable to override the setting here
IDE_BUILD_TREE = $$(QTC_BUILD)

# Qt Creator from local pri file if it exists
exists(../../QtCreator.local.pri) {
    include(../../QtCreator.local.pri)
}
QTC_PLUGIN_DIRS=
QTC_PLUGIN_DIRS += "$$PWD"
QTC_PLUGIN_DIRS += "$$QTCREATOR_SOURCES/src/plugins"

INCLUDEPATH += "$$QTCREATOR_SOURCES/src/libs"
#INCLUDEPATH += "$$QTCREATOR_SOURCES/src/plugins"
for(dir, QTC_PLUGIN_DIRS) {
    INCLUDEPATH += "$$dir"
}

# Parse plugins dependencies ##################################################
# Link against real libraries
unix {
    !isEmpty(QTC_PLUGIN_DEPENDS):LIBS *= -L$$IDE_BUILD_TREE/plugins
    !isEmpty(QTC_PLUGIN_DEPENDS):QMAKE_LFLAGS += -Wl,-rpath=$${IDE_BUILD_TREE}/plugins
} else:win32 {
    !isEmpty(QTC_PLUGIN_DEPENDS):LIBS *= -L$$IDE_BUILD_TREE/lib/qtcreator/plugins
    !isEmpty(QTC_PLUGIN_DEPENDS):QMAKE_LFLAGS += -Wl,-rpath=$${IDE_BUILD_TREE}/lib/qtcreator/plugins
}
# Recursively resolve library deps
done_plugins =
for(ever) {
    isEmpty(QTC_PLUGIN_DEPENDS): \
        break()
    done_plugins += $$QTC_PLUGIN_DEPENDS
    for(dep, QTC_PLUGIN_DEPENDS) {
        dependencies_file =
        for(dir, QTC_PLUGIN_DIRS) {
            exists($$dir/$$dep/$${dep}_dependencies.pri) {
                dependencies_file = $$dir/$$dep/$${dep}_dependencies.pri
                break()
            }
        }
        isEmpty(dependencies_file): \
            error("Plugin dependency $$dep not found")
        include($$dependencies_file)
        LIBS += -l$$qtLibraryName($$QTC_PLUGIN_NAME)
    }
    QTC_PLUGIN_DEPENDS = $$unique(QTC_PLUGIN_DEPENDS)
    QTC_PLUGIN_DEPENDS -= $$unique(done_plugins)
}

# Parse libs dependencies #####################################################
# Link against real libraries
unix {
    !isEmpty(QTC_LIB_DEPENDS):LIBS *= -L$$IDE_BUILD_TREE
    !isEmpty(QTC_LIB_DEPENDS):QMAKE_LFLAGS += -Wl,-rpath=$${IDE_BUILD_TREE}
} else:win32 {
    !isEmpty(QTC_LIB_DEPENDS):LIBS *= -L$$IDE_BUILD_TREE/bin
    #!isEmpty(QTC_LIB_DEPENDS):QMAKE_LFLAGS += -Wl,-rpath=$$IDE_BUILD_TREE/bin
}
# Recursively resolve library deps
done_libs =
for(ever) {
    isEmpty(QTC_LIB_DEPENDS): \
        break()
    done_libs += $$QTC_LIB_DEPENDS
    for(dep, QTC_LIB_DEPENDS) {
        include($$QTCREATOR_SOURCES/src/libs/$$dep/$${dep}_dependencies.pri)
        LIBS += -l$$qtLibraryName($$QTC_LIB_NAME)
    }
    QTC_LIB_DEPENDS = $$unique(QTC_LIB_DEPENDS)
    QTC_LIB_DEPENDS -= $$unique(done_libs)
}

