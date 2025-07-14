cmake_minimum_required(VERSION 3.16)

project(QTestLibPlugin)

# Project options
option(WITH_TESTS "Builds with tests" NO)
option(USE_USER_DESTDIR "Install in user config directory" YES)
set(MAKE_EXE "" CACHE FILEPATH "Path to make executable")

# CMake options
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Installation paths
if (USE_USER_DESTDIR)
  if (WIN32)
    set(USER_APP_NAME qtcreator)
    set(USER_DATA_BASE "$ENV{APPDATA}")
    if (NOT USER_DATABASE)
      set(USER_DATA_BASE "$ENV{USERPROFILE}\Local Settings\Application Data")
    endif()
    set(USER_DESTDIR_BASE "$ENV{USERPROFILE}\Local Settings\Application Data")
  elseif (APPLE)
    set(USER_APP_NAME "Qt Creator")
    set(USER_DATA_BASE "$ENV{HOME}/Library/Application Support")
    set(USER_DESTDIR_BASE "$ENV{HOME}/Library/Application Support")
  else()
    set(USER_APP_NAME qtcreator)
    set(USER_DATA_BASE "$ENV{XDG_DATA_HOME}")
    if (NOT USER_DATA_BASE)
      set(USER_DATA_BASE "$ENV{HOME}/.config")
    else()
      set(USER_DATA_BASE "${USER_DATA_BASE}/data")
    endif()
    set(USER_DESTDIR_BASE "$ENV{XDG_DATA_HOME}")
    if (NOT USER_DESTDIR_BASE)
      set(USER_DESTDIR_BASE "$ENV{HOME}/.local/share/data")
    else()
      set(USER_DESTDIR_BASE "${USER_DESTDIR_BASE}/data")
    endif()
  endif()

  set(PLUGIN_DESTDIR "${USER_DESTDIR_BASE}/QtProject/${USER_APP_NAME}/plugins/${IDE_VERSION}")
  set(TRANSLATIONS_DESTDIR "${USER_DATA_BASE}/QtProject/${USER_APP_NAME}/translations")
else()
  set(PLUGIN_DESTDIR "${IDE_PLUGIN_PATH}")
  set(TRANSLATIONS_DESTDIR "${IDE_DATA_PATH}/translations")
endif()

message(STATUS "Plugin install path:       \"${PLUGIN_DESTDIR}\"")
message(STATUS "Translations install path: \"${TRANSLATIONS_DESTDIR}\"")

# Subdirectories
if (WITH_TESTS)
    set(TESTS_DIR "${CMAKE_CURRENT_LIST_DIR}/tests/")
    set(SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/src/QTestLibPlugin/")
    if (NOT MAKE_EXE)
        set(MAKE_EXE "${CMAKE_MAKE_PROGRAM}")
    endif()
    message(STATUS "Make executable: ${MAKE_EXE}")
    add_subdirectory(tests/common)
    add_subdirectory(tests/QTestLibArgsParserTest/)
    add_subdirectory(tests/QTestLibModelTest/)
    add_subdirectory(tests/PlainTextQTestLibParserTest/)
    add_subdirectory(tests/XMLQTestLibParserTest/)
    add_subdirectory(tests/XUnitXMLQTestLibParserTest/)
    add_subdirectory(tests/LightXMLQTestLibParserTest/)
    add_subdirectory(tests/TestProxyModelTest/)
    add_subdirectory(tests/TestSuiteModelSignalTest/)
endif()
add_subdirectory(src/QTestLibPlugin)
add_subdirectory(plugin/QTestLibPlugin)

# Translations
add_translation_targets("translations/qtestlibplugin"
  TS_LANGUAGES fr en
  QM_LANGUAGES fr en
  OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${IDE_DATA_PATH}/translations"
  INSTALL_DESTINATION "${TRANSLATIONS_DESTDIR}"
)
