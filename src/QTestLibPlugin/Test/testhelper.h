/* Copyright 2016-2020 Pascal COMBES <pascom@orange.fr>
 * 
 * This file is part of QTestLibPlugin.
 * 
 * QTestLibPlugin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * QTestLibPlugin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with QTestLibPlugin. If not, see <http://www.gnu.org/licenses/>
 */

#ifndef TESTHELPER_H
#define TESTHELPER_H

namespace ProjectExplorer {
    class Project;
}

namespace Utils {
    class FilePath;
}

namespace QTestLibPlugin {
namespace Test {

bool removeProjectUserFiles(const Utils::FilePath& projectPath);
bool openQMakeProject(const Utils::FilePath& projectFilePath, ProjectExplorer::Project** project);
bool closeProject(ProjectExplorer::Project* project);

} // Test
} // QTestLibPlugin

#endif // TESTHELPER_H
