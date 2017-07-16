# Copyright 2015-2016 Pascal COMBES <pascom@orange.fr>
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

QTESTLIBPLUGIN_BIN = $$PWD/bin
QTESTLIBPLUGIN_LIB = $$PWD/plugin/QTestLibPlugin
QTESTLIBPLUGIN_SRC = $$PWD/src/QTestLibPlugin
QTESTLIBPLUGIN_I18N = $$PWD/translations
QTESTLIBPLUGIN_TESTS = $$PWD/tests
QTESTLIBMODEL_TESTS = $$PWD/tests/QTestLibModelTest/tests

# By default do not build tests:
BUILD_TESTS=
# By default do not run gcov:
USE_GCOV=
# By default install into user config directory:
USE_USER_DESTDIR=yes

exists(QTestLibPlugin.local.pri) {
    include(QTestLibPlugin.local.pri)
}
