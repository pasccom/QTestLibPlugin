# Copyright 2015-2020 Pascal COMBES <pascom@orange.fr>
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

CONFIG += ordered

include(QTestLibPlugin.pri)

exists($$QTESTLIBPLUGIN_I18N/translations.lst) {
    system("rm \"$$QTESTLIBPLUGIN_I18N/translations.lst\"")
}
exists($$QTESTLIBPLUGIN_I18N/sources.lst) {
    system("rm \"$$QTESTLIBPLUGIN_I18N/sources.lst\"")
}


SUBDIRS = $$QTESTLIBPLUGIN_SRC

!isEmpty(BUILD_TESTS) {
    SUBDIRS += $$QTESTLIBPLUGIN_SRC/Test
    SUBDIRS += $$QTESTLIBPLUGIN_TESTS
}

SUBDIRS += $$QTESTLIBPLUGIN_LIB
SUBDIRS += $$QTESTLIBPLUGIN_I18N

OTHER_FILES +=  README.md \
                QtCreator.local.pri.example \
                QTestLibPlugin.local.pri.example \
                Doxyfile \
                Doxyfile.internal

include(gcov.pri)

###### Translation files update (not handled by Qt)

lupdate.commands = ( test -d $$QTESTLIBPLUGIN_I18N || mkdir -p $$QTESTLIBPLUGIN_I18N ) && \
                   cd $$QTESTLIBPLUGIN_I18N &&  \
                   ( test -e Makefile || $$QMAKE $$QTESTLIBPLUGIN_I18N/translations.pro -o Makefile ) && \
                   make -f Makefile lupdate

QMAKE_EXTRA_TARGETS += lupdate

###### Translation files generation (not handled by Qt)

lrelease.commands = ( test -d $$QTESTLIBPLUGIN_I18N || mkdir -p $$QTESTLIBPLUGIN_I18N ) && \
                    cd $$QTESTLIBPLUGIN_I18N &&  \
                    ( test -e Makefile || $$QMAKE $$QTESTLIBPLUGIN_I18N/translations.pro -o Makefile ) && \
                    make -f Makefile compiler_lrelease_make_all

QMAKE_EXTRA_TARGETS += lrelease
