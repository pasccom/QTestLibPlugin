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

TEMPLATE = subdirs

include(QTestLibPlugin.pri)

SUBDIRS = \
    $$QTESTLIBPLUGIN_SRC \
    $$QTESTLIBPLUGIN_LIB

system("test -e \"$$QTESTLIBPLUGIN_I18N/sources.lst\" && rm \"$$QTESTLIBPLUGIN_I18N/sources.lst\"")

!isEmpty(BUILD_TESTS) {
    SUBDIRS += $$QTESTLIBPLUGIN_TESTS
}


###### Translation files update (not handled by Qt)

lupdate.commands = ( test -d $$QTESTLIBPLUGIN_LIB || mkdir -p $$QTESTLIBPLUGIN_LIB ) && \
                   cd $$QTESTLIBPLUGIN_LIB &&  \
                   ( test -e Makefile || $$QMAKE $$QTESTLIBPLUGIN_LIB/QTestLibPlugin.pro -o Makefile ) && \
                   make -f Makefile lupdate

QMAKE_EXTRA_TARGETS += lupdate

###### Translation files generation (not handled by Qt)


lrelease.commands = ( test -d $$QTESTLIBPLUGIN_LIB || mkdir -p $$QTESTLIBPLUGIN_LIB ) && \
                    cd $$QTESTLIBPLUGIN_LIB &&  \
                    ( test -e Makefile || $$QMAKE $$QTESTLIBPLUGIN_LIB/QTestLibPlugin.pro -o Makefile ) && \
                    make -f Makefile compiler_lrelease_make_all

QMAKE_EXTRA_TARGETS += lrelease
