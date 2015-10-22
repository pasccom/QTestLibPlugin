DEFINES += QTESTLIBPLUGIN_LIBRARY

# QTestLibPlugin files

SOURCES += \
    testoutputpane.cpp \
    testsuitemodel.cpp \
    qtestlibmodel.cpp \
    qtestlibplugin.cpp \
    testmodelfactory.cpp \
    plaintextqtestlibparser.cpp \
    plaintextqtestlibparserfactory.cpp \
    xmlqtestlibparser.cpp \
    xmlqtestlibparserfactory.cpp \
    qtestlibargsparser.cpp

HEADERS += \
        qtestlibplugin_global.h \
        qtestlibpluginconstants.h \
    testoutputpane.h \
    testsuitemodel.h \
    qtestlibmodel.h \
    qtestlibplugin.h \
    testmodelfactory.h \
    plaintextqtestlibparser.h \
    plaintextqtestlibparserfactory.h \
    xmlqtestlibparser.h \
    xmlqtestlibparserfactory.h \
    qtestlibargsparser.h

# Qt Creator linking

## set the QTC_SOURCE environment variable to override the setting here
QTCREATOR_SOURCES = $$(QTC_SOURCE)
isEmpty(QTCREATOR_SOURCES):QTCREATOR_SOURCES=/home/pascal/Téléchargements/Logiciels/Dev/qt-creator-opensource-src-3.4.2

## set the QTC_BUILD environment variable to override the setting here
IDE_BUILD_TREE = $$(QTC_BUILD)
isEmpty(IDE_BUILD_TREE):IDE_BUILD_TREE=/usr/lib64/qtcreator

## uncomment to build plugin into user config directory
## <localappdata>/plugins/<ideversion>
##    where <localappdata> is e.g.
##    "%LOCALAPPDATA%\QtProject\qtcreator" on Windows Vista and later
##    "$XDG_DATA_HOME/data/QtProject/qtcreator" or "~/.local/share/data/QtProject/qtcreator" on Linux
##    "~/Library/Application Support/QtProject/Qt Creator" on Mac
USE_USER_DESTDIR = yes

include($$QTCREATOR_SOURCES/src/qtcreatorplugin.pri)

###### Added stuff to bypass IDE_BUILD_TREE
target.path = $$DESTDIR
INSTALLS =
INSTALLS += target
DESTDIR = ./bin

LIBS+= -L/usr/lib64/qtcreator
LIBS+= -L/usr/lib64/qtcreator/plugins

RESOURCES += \
    qtestlibplugin.qrc
