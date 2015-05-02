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
  m_handles()
{
  // FIXME: insert code to calculate size
}

MultiFile::~MultiFile()
{
}

size_t
MultiFile::get_size() const
{
  return m_size;
}

struct timespec
MultiFile::get_mtime() const
{
  return m_mtime;
}

int
MultiFile::getattr(const char* path, struct stat* stbuf)
{
  stbuf->st_mode = S_IFREG | 0444;
  stbuf->st_nlink = 2;
  stbuf->st_size = get_size();
  stbuf->st_mtim = get_mtime();
  return 0;
}

void
MultiFile::refresh()
{
  for(const auto& h : m_handles)
  {
    h.second->refresh();
  }
  clock_gettime(CLOCK_REALTIME, &m_mtime);
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
    fi->fh = m_handles.store(make_unique<MultiFileStream>(*m_file_list));
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
  m_handles.drop(fi->fh);
  return 0;
}

int
MultiFile::read(const char* path, char* buf, size_t len, off_t offset,
                struct fuse_file_info* fi)
{
  return static_cast<int>(m_handles.get(fi->fh)->read(static_cast<size_t>(offset), buf, len));
}

/* EOF */
