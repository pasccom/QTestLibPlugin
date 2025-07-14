cmake_minimum_required(VERSION 3.16)

find_package(QtCreator COMPONENTS Utils REQUIRED)
find_package(Qt6 COMPONENTS Test REQUIRED)

add_executable(qtestlibargsparsertest
    qtestlibargsparsertest.cpp
)

target_include_directories(qtestlibargsparsertest PRIVATE "${SRC_DIR}")
target_link_libraries(qtestlibargsparsertest
    libQTestLibPlugin
    Qt6::Test
    QtCreator::Utils
)
