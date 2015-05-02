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

#include "zip_file.hpp"

#include <fuse.h>

#include "util.hpp"
#include "zip_stream.hpp"

ZipFile::ZipFile(const std::string& filename) :
  m_filename(filename),
  m_size(),
  m_mtime(),
  m_handles(),
  m_mutex()
{
  // FIXME:
  // * options to filter zip file content and customize sorting should
  //   be provided to the user
  auto data = ZipStream::open(m_filename);
  m_size = data->get_size();
}

ZipFile::~ZipFile()
{
}

size_t
ZipFile::get_size() const
{
  return m_size;
}

struct timespec
ZipFile::get_mtime() const
{
  return m_mtime;
}

int
ZipFile::getattr(const char* path, struct stat* stbuf)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  stbuf->st_mode = S_IFREG | 0444;
  stbuf->st_nlink = 2;
  stbuf->st_size = m_size;
  stbuf->st_mtim = m_mtime;
  return 0;
}

int
ZipFile::utimens(const char* path, const struct timespec tv[2])
{
  return 0;
}

int
ZipFile::open(const char* path, struct fuse_file_info* fi)
{
  if ((fi->flags & O_ACCMODE) == O_RDONLY)
  {
    fi->fh = m_handles.store(ZipStream::open(m_filename));
    return 0;
  }
  else
  {
    return -EACCES;
  }
}

int
ZipFile::release(const char* path, struct fuse_file_info* fi)
{
  m_handles.drop(fi->fh);
  return 0;
}

int
ZipFile::read(const char* path, char* buf, size_t len, off_t offset,
              struct fuse_file_info* fi)
{
  return static_cast<int>(m_handles.get(fi->fh)->read(static_cast<size_t>(offset), buf, len));
}

/* EOF */
