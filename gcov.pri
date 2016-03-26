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

# CONFIG += gcov does not work properly when OBJECTS_DIR is set.
# Indeed a relative path to the source file is registered in gcov files
# by default. The base is the build directory. Then when gcov is used,
# it does not find the original source file and the *.gcov files are less
# usefull.
#
# This file proposes a solution to this problem. It add a pre target
# and a post target to the initial target. The pre target creates
# a modified version of the Makefile, which is called Makefile.gcov:
#   -.gcov is appended to the target name
#   -_gcov is appended to the OBJECTS_DIR
#   -The necessary flags (-fprofile-arcs -ftest-coverage) are added
#   -The necessary libraries are added (-lgcov).
# The post target runs the modified makefile, Makefile.gcov.
# For convenience it also improves the clean target so that
# it removes the *.o, the *.gcno and the *.gcda files created by gcov.
#
# The user can then run the *.gcov executable and get the result with
# gcov -o OBJDIR SRCFILE
# To simplify even more this process a special forced target is provided
# in the make file. It is called "gcov". When run, this target creates and
# populates a folder called "gcov/" containing the annotated source files
# and even the header files if available.
#
# When included in subdirs project, this file also creates a recursive "gcov"
# target which allows to run the "gcov" targets in subdirs recursively.
#
# This is only a hack. I hope it works more properly than Qt original option.

unix {
    CONFIG(gcov) {
        isEmpty(OBJECTS_DIR) {
            TARGET_GCOV=$$DESTDIR/$$TARGET
            OBJECTS_DIR_GCOV="."
        } else {
            message("Detected gcov with OBJECTS dir")
            TARGET_GCOV=$$DESTDIR/$${TARGET}.gcov
            OBJECTS_DIR_GCOV=$${OBJECTS_DIR}_gcov
            isEmpty(MAKEFILE): MAKEFILE=Makefile

            gcov_makefile.commands += "sed -e 's/^\\([\\t ]\\+\$\$(CXX) -c \$\$(CXXFLAGS) \$\$(INCPATH)\\) -o $$replace(OBJECTS_DIR, "./", "\\")\\(\\/.*\\) \\(.*[^)]\\)\$\$/\\1 -o $$replace(OBJECTS_DIR, "./", "")_gcov\\2 \$\$(abspath \\3)/' \
                                           -e 's/^\\([LC]X\\?X\\?FLAGS[\\t ]\\+= .*\\)\$\$/\\1 -fprofile-arcs -ftest-coverage/' \
                                           -e 's/^\\(LIBS[\\t ]\\+= .*\\)\$\$/\\1 -lgcov/' \
                                           -e 's/\\($$replace(OBJECTS_DIR, "./", "\\")\\)\\//\\1_gcov\\//' \
                                           -e 's/\\(TARGET[\\t ]\\+= .*\\)\$\$/\\1.gcov/' \
                                           -e 's/^\$\$(TARGET):\\(.*\\) gcov_makefile \\(.*\\) gcov_target\$\$/\$\$(TARGET): \\1 \\2/' \
                                           -e 's/^\$\$(TARGET):\\(.*\\) gcov_makefile \\(.*\\) gcov_target \\(.*\\)\$\$/\$\$(TARGET): \\1 \\2 \\3/' \
                                           $$MAKEFILE > $${MAKEFILE}.gcov;"
            gcov_makefile.commands += "test -d $$OBJECTS_DIR_GCOV || mkdir $$OBJECTS_DIR_GCOV;"

            gcov_target.depends += gcov_makefile
            gcov_target.commands += "$(MAKE) -f $${MAKEFILE}.gcov;"

            QMAKE_EXTRA_TARGETS += gcov_makefile gcov_target
            PRE_TARGETDEPS += gcov_makefile
            POST_TARGETDEPS += gcov_target

        for (SOURCE, SOURCES) {
            SOURCE=$$basename(SOURCE)
                    QMAKE_CLEAN += $${OBJECTS_DIR}_gcov/$$replace(SOURCE, .cpp, .o)
                    QMAKE_CLEAN += $${OBJECTS_DIR}_gcov/$$replace(SOURCE, .cpp, .gcno)
                    QMAKE_CLEAN += $${OBJECTS_DIR}_gcov/$$replace(SOURCE, .cpp, .gcda)
        }

            CONFIG -= gcov

            gcov.depends += gcov_target
        }

        gcov.depends += \$(TARGET)
        gcov.depends += FORCE
        gcov.commands += "test -d .gcov || mkdir .gcov;"
        gcov.commands += "test -d gcov || mkdir gcov;"
        gcov.commands += "./$$TARGET_GCOV;"
        gcov.commands += "cd .gcov;"
        for (SOURCE, SOURCES) {
            # NOTE In qMake there is no easy way to guess if the path is absolute or relative.
            # Thus we use the following heuristic, which seems reasonable
            #   -If the SOURCE file exists, then it is an absolute path
            #   -Otherwise it is a relative path base on parent directory.
            exists($$SOURCE) {
                # The path to SOUCE is absolute:
                gcov.commands += "gcov -o \"../$$OBJECTS_DIR_GCOV\" \"$$SOURCE\" > /dev/null;"
            } else {
                # The path to SOURCE is relative:
                gcov.commands += "gcov -o \"../$$OBJECTS_DIR_GCOV\" \"../$$SOURCE\" > /dev/null;"
            }
            SOURCE=$$basename(SOURCE)
            gcov.commands += "test -f \"$${SOURCE}.gcov\" && mv -f \"$${SOURCE}.gcov\" ../gcov/;"
            gcov.commands += "test -f \"$$replace(SOURCE, .cpp, .h).gcov\" && mv -f \"$$replace(SOURCE, .cpp, .h).gcov\" ../gcov/;"
            gcov.commands += "rm -f *.gcov;"
        }
        gcov.commands += "cd ..;"
        gcov.commands += "rm -Rf .gcov/;"

        QMAKE_EXTRA_TARGETS += gcov
        QMAKE_CLEAN += gcov/*.gcov
    }

    equals(TEMPLATE, subdirs) {
        gcov.depends=
        for (SUBDIR, SUBDIRS) {
            SUBDIR_PRO_NAME=$$basename(SUBDIR)
            exists("$$SUBDIR/$${SUBDIR_PRO_NAME}.pro") {
                TEST_CONFIG=$$system("grep -P \"^CONFIG[\\t ]+\\+=[\\t ]+gcov\$\" \"$$SUBDIR/$${SUBDIR_PRO_NAME}.pro\"")
                TEST_TEMPLATE=$$system("grep -P \"^TEMPLATE[\\t ]+=[\\t ]+subdirs\$\" \"$$SUBDIR/$${SUBDIR_PRO_NAME}.pro\"")
                TEST_INCLUDE=$$system("grep -P \"^include\\(.*gcov.pri\\)\$\" \"$$SUBDIR/$${SUBDIR_PRO_NAME}.pro\"")
            } else {
                exists("$$_PRO_FILE_PWD_/$$SUBDIR/$${SUBDIR_PRO_NAME}.pro") {
                    TEST_CONFIG=$$system("grep -P \"^CONFIG[\\t ]+\\+=[\\t ]+gcov\$\" \"$$_PRO_FILE_PWD_/$$SUBDIR/$${SUBDIR_PRO_NAME}.pro\"")
                    TEST_TEMPLATE=$$system("grep -P \"^TEMPLATE[\\t ]+=[\\t ]+subdirs\$\" \"$$_PRO_FILE_PWD_/$$SUBDIR/$${SUBDIR_PRO_NAME}.pro\"")
                    TEST_INCLUDE=$$system("grep -P \"^include\\(.*gcov.pri\\)\$\" \"$$_PRO_FILE_PWD_/$$SUBDIR/$${SUBDIR_PRO_NAME}.pro\"")
                } else {
                    warning("Subdir \"$$SUBDIR\" project file does not exist")
                }
            }
            isEmpty(TEST_INCLUDE) {
                TEST_TEMPLATE=
            }
            !isEmpty(TEST_CONFIG) | !isEmpty(TEST_TEMPLATE) {
                message(Subdir with gcov or subdirs: $$SUBDIR -> $$SUBDIR_PRO_NAME)
                SUB_TARGET_NAME=$$replace(SUBDIR, :, )
                SUB_TARGET_NAME=sub-$$replace(SUB_TARGET_NAME, /, -)-gcov
                $${SUB_TARGET_NAME}.depends = FORCE
                $${SUB_TARGET_NAME}.commands =
                $${SUB_TARGET_NAME}.commands += "test -d \"$$SUBDIR\" | mkdir -p \"$$SUBDIR\";"
                $${SUB_TARGET_NAME}.commands += "cd \"$$SUBDIR\" && ( test -e Makefile || $(QMAKE) $${SUBDIR_PRO_NAME}.pro -o Makefile ) && $(MAKE) -f Makefile gcov;"
                QMAKE_EXTRA_TARGETS += $$SUB_TARGET_NAME
                gcov.depends += $$SUB_TARGET_NAME
            }
        }
        QMAKE_EXTRA_TARGETS += gcov
    }
} else {
    # Disables gcov on any other OS
    CONFIG -= gcov
}
