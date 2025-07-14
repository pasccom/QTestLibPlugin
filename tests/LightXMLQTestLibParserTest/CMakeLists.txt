cmake_minimum_required(VERSION 3.16)

find_package(Qt6 COMPONENTS Test Xml REQUIRED)

add_executable(lightxmlqtestlibparsertest
    lightxmlqtestlibparserfactoryfake.h
    lightxmlqtestlibparsertest.cpp
)

target_compile_definitions(lightxmlqtestlibparsertest PRIVATE MAKE_EXECUATBLE="${MAKE_EXE}")
target_compile_definitions(lightxmlqtestlibparsertest PRIVATE TESTS_DIR="${TESTS_DIR}")
target_include_directories(lightxmlqtestlibparsertest PRIVATE "${SRC_DIR}")
target_link_libraries(lightxmlqtestlibparsertest
    libQTestLibPlugin
    libTestCommon
    Qt6::Test
    Qt6::Xml
)
