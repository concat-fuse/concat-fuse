// concat-fuse
// Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
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

#include "simple_file_list.hpp"

#include "util.hpp"

std::unique_ptr<SimpleFileList>
SimpleFileList::from_file0(const std::string& data)
{
  return std::make_unique<SimpleFileList>(split(data, '\0'));
}

std::unique_ptr<SimpleFileList>
SimpleFileList::from_file(const std::string& data)
{
  return std::make_unique<SimpleFileList>(split(data, '\n'));
}

std::vector<FileInfo>
SimpleFileList::scan() const
{
  std::vector<FileInfo> result;

  for(const auto& filename : m_files)
  {
    result.push_back({filename, get_file_size(filename)});
  }

  return result;
}

/* EOF */
