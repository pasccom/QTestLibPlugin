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
