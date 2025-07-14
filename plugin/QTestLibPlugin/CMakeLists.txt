cmake_minimum_required(VERSION 3.16)

find_package(QtCreator COMPONENTS Core ProjectExplorer QmakeProjectManager REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED LinguistTools)

if(WITH_TESTS)
  find_package(Qt6 REQUIRED COMPONENTS Test)
  # Tell CMake functions like add_qtc_plugin about the QtTest component.
  set(IMPLICIT_DEPENDS Qt::Test)
endif()

add_qtc_plugin(QTestLibPlugin
  PLUGIN_PATH
    "${PLUGIN_DESTDIR}"
  PLUGIN_DEPENDS
    QtCreator::Core
    QtCreator::ProjectExplorer
    QtCreator::QmakeProjectManager
  DEPENDS
    Qt::Widgets
    QtCreator::ExtensionSystem
    QtCreator::Utils
    libQTestLibPlugin
  SOURCES
    #README.md
    #Doxyfile
    #Doxyfile.internal
    qtestlibplugin.h
    qtestlibplugin.cpp
    qtestlibplugin.qrc
)

extend_qtc_plugin(QTestLibPlugin
  CONDITION WITH_TESTS
  DEFINES BUILD_TESTS
)

if (USE_USER_DESTDIR)
  set_target_properties(QTestLibPlugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${IDE_PLUGIN_PATH}"
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${IDE_PLUGIN_PATH}"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${IDE_PLUGIN_PATH}"
  )
endif()
