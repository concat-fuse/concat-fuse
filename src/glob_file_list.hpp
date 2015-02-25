// concat-fuse
// Copyright (C) 2015 Ingo Ruhnke <grumbel@gmx.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_GLOB_FILE_LIST_HPP
#define HEADER_GLOB_FILE_LIST_HPP

#include <vector>
#include <string>

#include "file_list.hpp"

class GlobFileList : public FileList
{
private:
  std::vector<std::string> m_globs;

public:
  GlobFileList(const std::vector<std::string>& globs);

  std::vector<FileInfo> scan() const override;

private:
  GlobFileList(const GlobFileList&) = delete;
  GlobFileList& operator=(const GlobFileList&) = delete;
};

#endif

/* EOF */
