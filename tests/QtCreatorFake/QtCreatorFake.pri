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
#          else:win32:RET = $$member(LIBRARY_NAME, 0)d
          else:win32:RET = $$member(LIBRARY_NAME, 0)
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
QTC_PLUGIN_DIRS=$$PWD

INCLUDEPATH += $$QTCREATOR_SOURCES/src/libs
for(dir, QTC_PLUGIN_DIRS) {
    INCLUDEPATH += $$dir
}

# Parse plugins dependencies ##################################################
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
    !isEmpty(QTC_LIB_DEPENDS):QMAKE_LFLAGS += -Wl,-rpath=$${IDE_BUILD_TREE/bin}
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

