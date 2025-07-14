cmake_minimum_required(VERSION 3.16)

find_package(Qt6 COMPONENTS Test Xml REQUIRED)

add_executable(xmlqtestlibparsertest
    xmlqtestlibparserfactoryfake.h
    xmlqtestlibparsertest.cpp
)

target_compile_definitions(xmlqtestlibparsertest PRIVATE MAKE_EXECUATBLE="${MAKE_EXE}")
target_compile_definitions(xmlqtestlibparsertest PRIVATE TESTS_DIR="${TESTS_DIR}")
target_include_directories(xmlqtestlibparsertest PRIVATE "${SRC_DIR}")
target_link_libraries(xmlqtestlibparsertest
    libQTestLibPlugin
    libTestCommon
    Qt6::Test
    Qt6::Xml
)
