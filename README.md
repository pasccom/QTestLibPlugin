REPOSITORY DESCRIPTION
----------------------

This repository contains the code for a plugin to efficiently run QTestLib
tests in Qt Creator. The plugin connects to "Start run" signal and captures
the ouput if it thinks it is a QTestLib test. It then models test output so
that it can be easily read in a simple QTreeview.

FEATURES
--------

Here is a list of the current features of the plugin:
- Allows to parse plain text (`txt`) QTestLib output
- Allows to parse XML (`XML`) QTestLib output
- Check beforehand if the parser may parse the test 
- Model for a QTestLib test
- Merge models for multiple tests
- Display the models in a QTreeView 

INSTALLATION
------------

To compile this file, you will need to give some information to qMake about
the location of Qt Creator files on your system. For this, you have two
alternatives:
- Either set the environment variables `QTC_BUILD` and `QTC_SOURCES` to the
path to Qt Creator build tree and Qt Creator source tree respectively; or
- Write a file called QtCreator.local.pri containing the following code:
```qmake
# Qt Creator source tree:
isEmpty(QTCREATOR_SOURCES):QTCREATOR_SOURCES=/path/to/QtCreator/source/tree
# Qt Creator build tree:
isEmpty(IDE_BUILD_TREE):IDE_BUILD_TREE=/path/to/QtCreator/build/tree
```

PLANNED DEVELOPMENTS
--------------------

Here are some ideas I plan to implement later:
- Replace QTreeView by a specially designed view as QTreeView is not suitable
for lengthy test messages.
- Add filters to filter the test model.
- Complete information of the model by parsing test files.
- Allow to easily select test cases and data row.
- Add parsers for other QTestLib output formats.
- Have parser read in QTestLib output files (and not only `stdout`).
- Add parsers and models for other test libraries (gtest, cppunit)

If you have any other feature you will be interested in, please let me know.
I will be pleased to develop it if I think it is a mush have.

If you want to implement extension, also tell me please. Admittedly you
can do what you desire with the code (under the constraints stated in the
section LICENSING INFORMATION below), but this will avoid double work.

LICENSING INFORMATION
---------------------

QTestLibPlugin is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

QTestLibPlugin is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with QTestLibPlugin. If not, see http://www.gnu.org/licenses/