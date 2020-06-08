# Copyright 2016-2020 Pascal COMBES <pascom@orange.fr>
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

# Set the path to make executable.
# You have three solutions:
#   -1. You can force it in QtCreator.local.pri by setting FORCE_MAKE_EXE
#   -2. It is autodetected in your system PATH
#   -3. You set a default value it in QtCreator.local.pri

exists($$PWD/QtCreator.local.pri) {
    include($$PWD/QtCreator.local.pri)
    MAKE_EXE="$$FORCE_MAKE_EXE"
}
isEmpty(MAKE_EXE) {
    # Try to auto-detect make in system PATH
    PATH=$$(PATH)
    win32:PATH=$$split(PATH, ;)
    else:PATH=$$split(PATH, :)
    MAKE_EXE=
    for (DIR, PATH) {
        isEmpty(MAKE_EXE) {
            win32:MAKE_EXE=$$DIR/mingw32-make.exe
            else:MAKE_EXE=$$DIR/make
        }
        !exists($$MAKE_EXE) {
            MAKE_EXE=
        }
    }
    !isEmpty(MAKE_EXE): message("Auto-detected \"make\": $$MAKE_EXE")
}
isEmpty(MAKE_EXE) {
    exists($$PWD/QtCreator.local.pri) {
        include($$PWD/QtCreator.local.pri)
    }
}
isEmpty(MAKE_EXE): error("\"make\" executable cannot be found. Set it in QtCreator.local.pri")

DEFINES += MAKE_EXECUATBLE=\\\"$$replace(MAKE_EXE, \\\\, /)\\\"
