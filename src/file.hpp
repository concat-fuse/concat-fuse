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

#ifndef HEADER_FILE_HPP
#define HEADER_FILE_HPP

#include <memory>
#include <errno.h>

#include "entry.hpp"

class File;

using FilePtr = std::unique_ptr<File>;

class File : public Entry
{
private:
public:
  File() {}
  virtual ~File() {}

  virtual int open(const char* path, struct fuse_file_info* fi) = 0;
  virtual int release(const char* path, struct fuse_file_info* fi) = 0;

  virtual int read(const char* path, char* buf, size_t len, off_t offset,
                   struct fuse_file_info* fi) { return -ENOSYS; }
  virtual int write(const char* path, const char* buf, size_t len, off_t offset,
                    struct fuse_file_info* fi) { return -ENOSYS; }
  virtual int truncate(const char* path, off_t offsite) { return -ENOSYS; }
  virtual int flush(const char* path, struct fuse_file_info* fi) { return -ENOSYS; }

private:
  File(const File&) = delete;
  File& operator=(const File&) = delete;
};

#endif

/* EOF */
