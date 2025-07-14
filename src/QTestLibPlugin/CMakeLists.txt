cmake_minimum_required(VERSION 3.16)

find_package(QtCreator COMPONENTS Core ProjectExplorer QmakeProjectManager REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_library(libQTestLibPlugin STATIC
    qtestlibpluginconstants.h
    qtestlibplugin_global.h
    qtestlibargsparser.h
    qtestlibargsparser.cpp
    qtestlibmodel.h
    qtestlibmodel.cpp
    testmodelfactory.h
    testmodelfactory.cpp
    testsuitemodel.h
    testsuitemodel.cpp
    testproxymodel.h
    testproxymodel.cpp
    testrunconfiguration.h
    testrunconfiguration.cpp
    testrunworkerfactory.h
    testrunworkerfactory.cpp
    testextraaspect.h
    testextraaspect.cpp
    testoutputpane.h
    testoutputpane.cpp
    pathaspect.h
    pathaspect.cpp
    baseforceparserfactory.h
    baseforceparserfactory.cpp
    baseqmakeqtestlibparserfactory.h
    baseqmakeqtestlibparserfactory.cpp
    qmaketestrunconfigurationfactory.h
    qmaketestrunconfigurationfactory.cpp
    basexmlqtestlibparser.h
    basexmlqtestlibparser.cpp
    plaintextqtestlibparser.h
    plaintextqtestlibparser.cpp
    plaintextqtestlibparserfactory.h
    xunitxmlqtestlibparser.h
    xunitxmlqtestlibparser.cpp
    xunitxmlqtestlibparserfactory.h
    xmlqtestlibparser.h
    xmlqtestlibparser.cpp
    xmlqtestlibparserfactory.h
    lightxmlqtestlibparser.h
    lightxmlqtestlibparser.cpp
    lightxmlqtestlibparserfactory.h
    Widgets/filetypevalidatinglineedit.h
    Widgets/filetypevalidatinglineedit.cpp
)

if (WITH_TESTS)
    find_package(Qt6 REQUIRED COMPONENTS Test)
    target_link_libraries(libQTestLibPlugin libTestCommon Qt::Test)
    target_compile_definitions(libQTestLibPlugin PRIVATE TESTS_DIR=\"${TESTS_DIR}\")
    message(STATUS "TESTS_DIR=\"${TESTS_DIR}\"")
    target_include_directories(libQTestLibPlugin PRIVATE "${TESTS_DIR}")
    target_sources(libQTestLibPlugin PRIVATE
        Test/testhelper.h
        Test/testhelper.cpp
        Test/qmakeplaintextqtestlibparserfactorytest.h
        Test/qmakeplaintextqtestlibparserfactorytest.cpp
        Test/qmakexmlqtestlibparserfactorytest.h
        Test/qmakexmlqtestlibparserfactorytest.cpp
        Test/qmakexunitxmlqtestlibparserfactorytest.h
        Test/qmakexunitxmlqtestlibparserfactorytest.cpp
        Test/qmakelightxmlqtestlibparserfactorytest.h
        Test/qmakelightxmlqtestlibparserfactorytest.cpp
        Test/forceplaintextqtestlibparserfactorytest.h
        Test/forceplaintextqtestlibparserfactorytest.cpp
        Test/forcexmlqtestlibparserfactorytest.h
        Test/forcexmlqtestlibparserfactorytest.cpp
        Test/forcexunitxmlqtestlibparserfactorytest.h
        Test/forcexunitxmlqtestlibparserfactorytest.cpp
        Test/forcelightxmlqtestlibparserfactorytest.h
        Test/forcelightxmlqtestlibparserfactorytest.cpp
        Test/testmodelfactorytest.h
        Test/testmodelfactorytest.cpp
        Test/testsuitemodeltest.h
        Test/testsuitemodeltest.cpp
        Test/testactionstest.h
        Test/testactionstest.cpp
        Test/testrunconfigurationfactorytest.h
        Test/testrunconfigurationfactorytest.cpp
    )
endif()

target_include_directories(libQTestLibPlugin INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(libQTestLibPlugin
    Qt6::Widgets
    QtCreator::Core
    QtCreator::ProjectExplorer
    QtCreator::QmakeProjectManager
)
set_target_properties(libQTestLibPlugin PROPERTIES POSITION_INDEPENDENT_CODE True)
