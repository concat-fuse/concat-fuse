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

#include "multi_file.hpp"

#include <glob.h>
#include <assert.h>
#include <ctype.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <glob.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define log_debug(...) fprintf(stderr, "[DEBUG] " __VA_ARGS__)
//#define log_debug(...)

namespace {

size_t get_file_size(const char* filename)
{
  int fd = open(filename, O_RDONLY);
  if (fd < 0)
  {
    perror(filename);
    return 0;
  }
  else
  {
    off_t ret = lseek(fd, 0, SEEK_END);
    close(fd);
    return ret;
  }
}

} // namespace

MultiFile::MultiFile(const std::string& pattern) :
  m_pattern(pattern),
  m_pos(0),
  m_files(),
  m_total_size(0)
{
  collect_file_info();
}

void
MultiFile::collect_file_info()
{
  glob_t glob_data;

  int ret = glob(m_pattern.c_str(), 0, NULL, &glob_data);
  if (ret == GLOB_NOMATCH)
  {
    log_debug("concat.so: no matching files found for pattern: %s\n", m_pattern.c_str());
  }
  else
  {
    m_total_size = 0;
    for(size_t i = 0; i < glob_data.gl_pathc; ++i)
    {
      m_files.push_back({glob_data.gl_pathv[i], get_file_size(glob_data.gl_pathv[i])});
        
      m_total_size += m_files.back().size;
    }
  }
  globfree(&glob_data);
}

int
MultiFile::find_file(size_t* offset)
{
  for(size_t i = 0; i < m_files.size(); ++i)
  {
    if (*offset >= m_files[i].size)
    {
      *offset -= m_files[i].size;
    }
    else
    {
      return i;
    }
  }
  return -1;
}

ssize_t
MultiFile::read(size_t pos, char* buf, size_t count)
{
  log_debug("magicfile_read(%zu, %p, %zu)\n", pos, buf, count);

  size_t total_count = 0;
  while(count != 0)
  {
    size_t offset = pos;
    int idx = find_file(&offset);
    log_debug("found file: %zu %d %zu %zu\n", pos, idx, offset, count);
    if (idx < 0)
    {
      log_debug("EOF reached\n");
      return total_count;
    }
    else
    {
      size_t read_count = count;
      if (m_files[idx].size - offset < count)
      {
        read_count = m_files[idx].size - offset;
      }
      read_subfile(m_files[idx].filename,
                   offset, buf, read_count);
      pos += read_count;
      buf += read_count;
      count -= read_count;
      total_count += read_count;
    }
  }
  log_debug("count = %zu\n", count);
  return total_count;
}

void
MultiFile::read_subfile(const std::string& filename, size_t offset, char* buf, size_t count)
{
  // FIXME: insert error handling here
  int fd = open(filename.c_str(), O_RDONLY);
  if (fd < 0)
  {
    perror(filename.c_str());
  }
  else
  {
    ::lseek(fd, offset, SEEK_SET);
    ::read(fd, buf, count);
    ::close(fd);
  }
}

/* EOF */
