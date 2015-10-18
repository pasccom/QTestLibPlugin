QTC_PLUGIN_NAME = ProjectExplorer
QTC_LIB_DEPENDS += \
    utils
QTC_PLUGIN_DEPENDS += \
    coreplugin 

QTC_PLUGIN_RECOMMENDS += \
    # Nothing
    
SOURCES += \
    $$PWD/runconfiguration.cpp \
    $$PWD/localapplicationrunconfiguration.cpp \
    $$PWD/localapplicationruncontrol.cpp \
    $$PWD/target.cpp \
    $$PWD/kit.cpp \
    $$PWD/project.cpp

HEADERS += \
    $$PWD/runconfiguration.h \
    $$PWD/localapplicationrunconfiguration.h \
    $$PWD/localapplicationruncontrol.h \
    $$PWD/target.h \
    $$PWD/kit.h \
    $$PWD/project.h
