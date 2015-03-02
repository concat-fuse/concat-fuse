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

#include "glob_file_list.hpp"

#include <glob.h>

#include "util.hpp"

GlobFileList::GlobFileList(const std::vector<std::string>& globs) :
  m_globs(globs)
{
}

std::vector<FileInfo>
GlobFileList::scan() const
{
  std::vector<FileInfo> result;

  for(const auto& pattern : m_globs)
  {
    glob_t glob_data;

    int ret = glob(pattern.c_str(), 0, nullptr, &glob_data);
    if (ret == GLOB_NOMATCH)
    {
      log_debug("no matching files found for pattern: \"{}\"", pattern);
    }
    else
    {
      for(size_t i = 0; i < glob_data.gl_pathc; ++i)
      {
        result.push_back({glob_data.gl_pathv[i], get_file_size(glob_data.gl_pathv[i])});
      }
    }
    globfree(&glob_data);
  }

  return result;
}

/* EOF */
