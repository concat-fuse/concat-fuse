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

#include "simple_file.hpp"

#include <string.h>
#include <sys/stat.h>
#include <fuse.h>

#include "util.hpp"
#include "simple_file_stream.hpp"

SimpleFile::SimpleFile(const std::string& data) :
  m_stream(std::make_unique<SimpleFileStream>(data))
{
}

SimpleFile::~SimpleFile()
{
}

int
SimpleFile::getattr(const char* path, struct stat* stbuf)
{
  stbuf->st_mode = S_IFREG | 0444;
  stbuf->st_nlink = 2;
  stbuf->st_size = static_cast<off_t>(m_stream->get_size());
  return 0;
}

int
SimpleFile::read(const char* path, char* buf, size_t len, off_t offset,
                 struct fuse_file_info* fi)
{
  return static_cast<int>(m_stream->read(static_cast<size_t>(offset), buf, len));
}

int
SimpleFile::open(const char* path, struct fuse_file_info* fi)
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
SimpleFile::release(const char* path, struct fuse_file_info* fi)
{
  return 0;
}

/* EOF */
