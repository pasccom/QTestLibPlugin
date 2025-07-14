cmake_minimum_required(VERSION 3.16)

find_package(QtCreator COMPONENTS Utils REQUIRED)
find_package(Qt6 COMPONENTS Test Xml REQUIRED)

add_library(libTestCommon STATIC
    qttestsubfunction.h
    qtestlibmodeltester.h
    qtestlibmodeltester.cpp
    qtversion.h
    qtversion.cpp
)

target_include_directories(libTestCommon INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(libTestCommon PUBLIC "${SRC_DIR}")
target_link_libraries(libTestCommon
    Qt6::Test
    Qt6::Xml
    QtCreator::Utils
)
set_target_properties(libTestCommon PROPERTIES POSITION_INDEPENDENT_CODE True)
