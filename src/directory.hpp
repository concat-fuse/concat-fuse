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

#ifndef HEADER_DIRECTORY_HPP
#define HEADER_DIRECTORY_HPP

#include <memory>
#include <unordered_map>
#include <fuse.h>

#include "file.hpp"

class Directory;

using DirectoryPtr = std::unique_ptr<Directory>;

class Directory : public Entry
{
private:
  Directory* m_parent;

public:
  Directory() : m_parent(nullptr) {}
  virtual ~Directory() {}

  void set_parent(Directory* parent);
  Directory* get_parent() const;

  virtual int opendir(const char* path, struct fuse_file_info*) { return -ENOSYS; }
  virtual int readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset,
                      struct fuse_file_info* fi) { return -ENOSYS; }
  virtual int releasedir(const char* path, struct fuse_file_info* fi) { return -ENOSYS; }

private:
  Directory(const Directory&) = delete;
  Directory& operator=(const Directory&) = delete;
};

#endif

/* EOF */
