QTC_PLUGIN_NAME = QmakeProjectManager
QTC_LIB_DEPENDS += \
    utils
    
QTC_PLUGIN_DEPENDS += \
    coreplugin \
    projectexplorer \
    #qtsupport \
    #texteditor \
    #cpptools \
    #resourceeditor

QTC_PLUGIN_RECOMMENDS += \
    # Nothing

SOURCES += \
    $$PWD/qmakeprojectmanager.cpp \
    $$PWD/qmakeproject.cpp \
    $$PWD/qmakenodes.cpp
HEADERS += \
    $$PWD/qmakeprojectmanager.h \
    $$PWD/qmakeproject.h \
    $$PWD/qmakenodes.h

#DEFINES += PROEVALUATOR_DEBUG

include($$QTCREATOR_SOURCES/src/shared/proparser/proparser.pri)
