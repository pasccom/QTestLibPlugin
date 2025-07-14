cmake_minimum_required(VERSION 3.16)

find_package(Qt6 COMPONENTS Test Xml REQUIRED)

add_executable(qtestlibmodeltest
    qtestlibmodeltest.cpp
)

target_compile_definitions(qtestlibmodeltest PRIVATE TESTS_DIR="${CMAKE_CURRENT_LIST_DIR}/tests/")
target_include_directories(qtestlibmodeltest PRIVATE "${SRC_DIR}")
target_link_libraries(qtestlibmodeltest
    libQTestLibPlugin
    libTestCommon
    Qt6::Test
    Qt6::Xml
)

# Generation of tests
if (NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/tests/tests.lst")
    execute_process(
        COMMAND bash "${CMAKE_CURRENT_LIST_DIR}/testlist.sh"
        WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
    )
endif()

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/tests/tests.lst")
    target_compile_definitions(qtestlibmodeltest PRIVATE TESTS_LST_EXISTS)
else()
    message(SEND_ERROR "Test data file not found. Run \"${CMAKE_CURRENT_LIST_DIR}/testlist.sh\"")
endif()
