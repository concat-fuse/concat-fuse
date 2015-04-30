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

#include <sys/stat.h>

ZipFile::ZipFile(const std::string& filename) :
  m_pos(0),
  m_size(0),
  m_mtime(),
  m_filename(filename)
{
}

ZipFile::~ZipFile()
{
}

ssize_t
ZipFile::read(size_t pos, char* buf, size_t count)
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
  stbuf->st_mode = S_IFREG | 0444;
  stbuf->st_nlink = 2;
  stbuf->st_size = get_size();
  stbuf->st_mtim = get_mtime();
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
  return 0;
}

int
ZipFile::release(const char* path, struct fuse_file_info* fi)
{
  return 0;
}

int
ZipFile::read(const char* path, char* buf, size_t len, off_t offset,
              struct fuse_file_info* fi)
{
  return static_cast<int>(read(static_cast<size_t>(offset), buf, len));
}

/* EOF */
