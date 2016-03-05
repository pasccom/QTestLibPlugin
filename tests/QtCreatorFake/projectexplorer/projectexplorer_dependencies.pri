# Copyright 2015 Pascal COMBES <pascom@orange.fr>
# 
# This file is part of QTestLibPlugin.
# 
# QTestLibPlugin is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# QTestLibPlugin is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with QTestLibPlugin. If not, see <http://www.gnu.org/licenses/>

QTC_PLUGIN_NAME = ProjectExplorer
QTC_LIB_DEPENDS += \
    ssh
QTC_PLUGIN_DEPENDS += \
    coreplugin \
    texteditor

QTC_PLUGIN_RECOMMENDS += \
    # Nothing
    
SOURCES += \
    $$PWD/localapplicationrunconfigurationfake.cpp \
    $$PWD/localapplicationruncontrolfake.cpp \
#    $$PWD/runconfiguration.cpp \
#    $$PWD/localapplicationrunconfiguration.cpp \
#    $$PWD/localapplicationruncontrol.cpp \
#    $$PWD/project.cpp \
#    $$PWD/target.cpp \
#    $$PWD/kit.cpp \
#    $$PWD/kitinformation.cpp \
#    $$PWD/buildconfiguration.cpp \
#    $$PWD/localenvironmentaspect.cpp \
#    $$PWD/toolchain.cpp

HEADERS += \
    $$PWD/localapplicationrunconfigurationfake.h \
    $$PWD/localapplicationruncontrolfake.h \
#    $$PWD/runconfiguration.h \
#    $$PWD/localapplicationrunconfiguration.h \
#    $$PWD/localapplicationruncontrol.h \
#    $$PWD/applicationlauncher.h \
#    $$PWD/project.h \
#    $$PWD/target.h \
#    $$PWD/kit.h \
#    $$PWD/kitinformation.h \
#    $$PWD/buildconfiguration.h \
#    $$PWD/localenvironmentaspect.h \
#    $$PWD/toolchain.h
