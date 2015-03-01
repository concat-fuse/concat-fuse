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

#include "control_file.hpp"

#include <sys/stat.h>
#include <fuse.h>

namespace {

uint64_t make_fh(size_t v)
{
  return v + 1;
}

size_t fh2idx(uint64_t handle)
{
  return handle - 1;
}

} // namespace

ControlFile::ControlFile(ConcatVFS& vfs) :
  File(vfs),
  m_tmpbuf()
{
}

ControlFile::~ControlFile()
{
}

int
ControlFile::getattr(const char* path, struct stat* stbuf)
{
  stbuf->st_mode = S_IFREG | 0644;
  stbuf->st_nlink = 2;
  stbuf->st_size = 0;
  return 0;
}

int
ControlFile::utimens(const char* path, const struct timespec tv[2])
{
  return 0;
}

int
ControlFile::open(const char* path, struct fuse_file_info* fi)
{
  m_tmpbuf.push_back({});
  fi->fh = make_fh(m_tmpbuf.size());
  fi->direct_io = 1;
  return 0;
}

int
ControlFile::read(const char* path, char* buf, size_t len, off_t offset,
                  struct fuse_file_info* fi)
{
  return 0;
}

int
ControlFile::write(const char* path, const char* buf, size_t len, off_t offset,
                   struct fuse_file_info* fi)
{
  m_tmpbuf[fh2idx(fi->fh)].append(buf, len);
  return 0;
}

int
ControlFile::truncate(const char* path, off_t offsite)
{
  return 0;
}

int
ControlFile::flush(const char* path, struct fuse_file_info* fi)
{
  return 0;
}

int
ControlFile::release(const char* path, struct fuse_file_info* fi)
{
  return 0;
}

/* EOF */
