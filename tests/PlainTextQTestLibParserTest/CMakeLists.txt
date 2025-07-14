cmake_minimum_required(VERSION 3.16)

find_package(Qt6 COMPONENTS Test Xml REQUIRED)

add_executable(plaintextqtestlibparsertest
    plaintextqtestlibparserfactoryfake.h
    plaintextqtestlibparsertest.cpp
)

target_compile_definitions(plaintextqtestlibparsertest PRIVATE MAKE_EXECUATBLE="${MAKE_EXE}")
target_compile_definitions(plaintextqtestlibparsertest PRIVATE TESTS_DIR="${TESTS_DIR}")
target_include_directories(plaintextqtestlibparsertest PRIVATE "${SRC_DIR}")
target_link_libraries(plaintextqtestlibparsertest
    libQTestLibPlugin
    libTestCommon
    Qt6::Test
    Qt6::Xml
)
