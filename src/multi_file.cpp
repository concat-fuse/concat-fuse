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

#include <fuse.h>

#include "multi_file_stream.hpp"
#include "util.hpp"

MultiFile::MultiFile(std::unique_ptr<FileList> file_list) :
  m_size(),
  m_mtime(),
  m_file_list(std::move(file_list)),
  m_files(),
  m_stream(),
  m_mutex()
{
  refresh();
}

MultiFile::~MultiFile()
{
}

int
MultiFile::getattr(const char* path, struct stat* stbuf)
{
  stbuf->st_mode = S_IFREG | 0444;
  stbuf->st_nlink = 2;
  stbuf->st_size = static_cast<off_t>(m_size);
  stbuf->st_mtim = m_mtime;
  return 0;
}

void
MultiFile::refresh()
{
  std::lock_guard<std::shared_timed_mutex> lock(m_mutex);

  clock_gettime(CLOCK_REALTIME, &m_mtime);

  m_files = m_file_list->scan();
  m_stream = std::make_unique<MultiFileStream>(m_files);
  m_size = m_stream->get_size();
}

int
MultiFile::utimens(const char* path, const struct timespec tv[2])
{
  refresh();
  return 0;
}

int
MultiFile::open(const char* path, struct fuse_file_info* fi)
{
  if ((fi->flags & O_ACCMODE) == O_RDONLY)
  {
    return 0;
  }
  else
  {
    return -EACCES;
  }
}

int
MultiFile::release(const char* path, struct fuse_file_info* fi)
{
  return 0;
}

int
MultiFile::read(const char* path, char* buf, size_t len, off_t offset,
                struct fuse_file_info* fi)
{
  std::shared_lock<std::shared_timed_mutex> lock(m_mutex);
  return static_cast<int>(m_stream->read(static_cast<size_t>(offset), buf, len));
}

/* EOF */
