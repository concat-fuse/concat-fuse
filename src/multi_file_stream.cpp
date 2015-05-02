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

#include "multi_file_stream.hpp"

#include <sys/types.h>
#include <unistd.h>
#include <fuse.h>

#include "util.hpp"

MultiFileStream::MultiFileStream(FileList& file_list) :
  m_pos(0),
  m_file_list(file_list),
  m_files()
{
}

void
MultiFileStream::refresh()
{
  log_debug("MultiFile::refresh()");
  m_files = m_file_list.scan();
}

ssize_t
MultiFileStream::read(size_t pos, char* buf, size_t count)
{
  log_debug("magicfile_read({}, {}, {})", pos, static_cast<void*>(buf), count);

  size_t total_count = 0;
  while(count != 0)
  {
    size_t offset = pos;
    int idx = find_file(&offset);
    log_debug("found file: {} {} {} {}", pos, idx, offset, count);
    if (idx < 0)
    {
      log_debug("EOF reached: {}", total_count);
      return total_count;
    }
    else
    {
      size_t read_count = count;
      if (m_files[idx].size - offset < count)
      {
        read_count = m_files[idx].size - offset;
      }
      read_subfile(m_files[idx].filename, offset, buf, read_count);
      pos += read_count;
      buf += read_count;
      count -= read_count;
      total_count += read_count;
    }
  }
  log_debug("count = {}", count);
  return total_count;
}

size_t
MultiFileStream::get_size() const
{
  size_t total = 0;
  for(const auto& file : m_files)
  {
    total += file.size;
  }
  return total;
}


int
MultiFileStream::find_file(size_t* offset)
{
  for(size_t i = 0; i < m_files.size(); ++i)
  {
    if (*offset >= m_files[i].size)
    {
      *offset -= m_files[i].size;
    }
    else
    {
      return static_cast<int>(i);
    }
  }
  return -1;
}

void
MultiFileStream::read_subfile(const std::string& filename, size_t offset, char* buf, size_t count)
{
  // FIXME: insert error handling here
  int fd = ::open(filename.c_str(), O_RDONLY);
  if (fd < 0)
  {
    perror(filename.c_str());
  }
  else
  {
    ::lseek(fd, offset, SEEK_SET);
    if (::read(fd, buf, count) < 0)
    {
      return; // FIXME: error
    }
    ::close(fd);
  }
}

/* EOF */
