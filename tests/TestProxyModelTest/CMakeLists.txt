cmake_minimum_required(VERSION 3.16)

find_package(Qt6 COMPONENTS Test REQUIRED)

add_executable(testproxymodeltest
    xmlqtestlibparserfactoryfake.h
    testproxymodeltest.cpp
)

target_compile_definitions(testproxymodeltest PRIVATE TESTS_DIR="${TESTS_DIR}")
target_link_libraries(testproxymodeltest
    libQTestLibPlugin
    libTestCommon
    Qt6::Test
)
