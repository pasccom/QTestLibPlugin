cmake_minimum_required(VERSION 3.16)

find_package(Qt6 COMPONENTS Test Xml REQUIRED)

add_executable(xunitxmlqtestlibparsertest
    xunitxmlqtestlibparserfactoryfake.h
    xunitxmlqtestlibparsertest.cpp
)

target_compile_definitions(xunitxmlqtestlibparsertest PRIVATE MAKE_EXECUATBLE="${MAKE_EXE}")
target_compile_definitions(xunitxmlqtestlibparsertest PRIVATE TESTS_DIR="${TESTS_DIR}")
target_include_directories(xunitxmlqtestlibparsertest PRIVATE "${SRC_DIR}")
target_link_libraries(xunitxmlqtestlibparsertest
    libQTestLibPlugin
    libTestCommon
    Qt6::Test
    Qt6::Xml
)
