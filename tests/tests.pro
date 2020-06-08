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
CONFIG  += ordered

# Common testing library:
SUBDIRS +=  common

# Mock tests:
SUBDIRS +=  AllMessagesTest     \
            OneClassTest        \
            MultipleClassesTest \
            SignalsTest         \
            LimitsTest          \           
            #OneSubTest          \
            #TwoSubTests         \
            # FIXME Uncomment these lines on first generation with make
            # So that the projects are compiled (they are used in tests)

# Parsers tests:
SUBDIRS +=  PlainTextQTestLibParserTest \
            XMLQTestLibParserTest       \
            LightXMLQTestLibParserTest  \
            XUnitXMLQTestLibParserTest  \

# Signals tests:
SUBDIRS += QTestLibModelTest        \
           TestSuiteModelSignalTest \

# Test proxy test:
SUBDIRS += TestProxyModelTest

# Argument parser test:
SUBDIRS += QTestLibArgsParserTest

include(../gcov.pri)
