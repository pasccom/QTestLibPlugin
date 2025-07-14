cmake_minimum_required(VERSION 3.20)

find_package(Qt6 COMPONENTS Test Xml REQUIRED)

add_executable(testsuitemodelsignaltest
    testsuitemodelsignaltest.cpp
)

cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH _TESTS_DIR)
set(TESTS_DIR "${_TESTS_DIR}/QTestLibModelTest/tests/")

#target_compile_definitions(testsuitemodelsignaltest PRIVATE MAKE_EXECUATBLE="${MAKE_EXE}")
target_compile_definitions(testsuitemodelsignaltest PRIVATE TESTS_DIR="${TESTS_DIR}")
target_link_libraries(testsuitemodelsignaltest
    libQTestLibPlugin
    libTestCommon
    Qt6::Test
    Qt6::Xml
)

if (EXISTS "${TESTS_DIR}/tests.lst")
    target_compile_definitions(testsuitemodelsignaltest PRIVATE TESTS_LST_EXISTS)
else()
    message(SEND_ERROR "Test data file not found. Run \"${_TESTS_DIR}/QTestLibModelTest/testlist.sh\"")
endif()
