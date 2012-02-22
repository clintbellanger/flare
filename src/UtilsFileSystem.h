/*
Copyright © 2011-2012 Clint Bellanger

This file is part of FLARE.

FLARE is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

FLARE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
FLARE.  If not, see http://www.gnu.org/licenses/
*/

/**
 * UtilsFileSystem
 *
 * Various file system function wrappers. Abstracted here to hide OS-specific implementations
 */

#ifndef UTILS_FILE_SYSTEM_H
#define UTILS_FILE_SYSTEM_H

#include <string>
#include <vector>

bool dirExists(std::string const& path);
void createDir(std::string const& path);
bool fileExists(std::string const& filename);
int getFileList(std::string const& dir, std::string const& ext, std::vector<std::string> &files);



#endif
