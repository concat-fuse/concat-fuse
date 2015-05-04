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

#include <fuse.h>
#include <sstream>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "util.hpp"

MultiFileStream::MultiFileStream(std::vector<FileInfo>& files) :
  m_files(files)
{
}

size_t
MultiFileStream::get_size() const
{
  size_t size = 0;
  for(const auto& file : m_files)
  {
    size += file.size;
  }
  return size;
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

int
MultiFileStream::find_file(size_t* offset) const
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
MultiFileStream::read_subfile(const std::string& filename, size_t offset, char* buf, size_t count) const
{
  int fd = ::open(filename.c_str(), O_RDONLY);
  if (fd < 0)
  {
    std::ostringstream str;
    str << filename << ": open() in read_subfile() failed: " << strerror(errno);
    throw std::runtime_error(str.str());
  }
  else
  {
    if (::lseek(fd, offset, SEEK_SET) < 0)
    {
      ::close(fd);

      std::ostringstream str;
      str << filename << ": lseek() in read_subfile() failed: " << strerror(errno);
      throw std::runtime_error(str.str());
    }

    if (::read(fd, buf, count) < 0)
    {
      ::close(fd);

      std::ostringstream str;
      str << filename << ": read() in read_subfile() failed: " << strerror(errno);
      throw std::runtime_error(str.str());
    }

    ::close(fd);
  }
}

/* EOF */
