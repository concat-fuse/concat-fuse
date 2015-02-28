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

class Directory
{
public:
  Directory() {}
  virtual ~Directory() {}

  virtual int getattr(const char* path, struct stat* stbuf) = 0;
  virtual int utimens(const char* path, const struct timespec tv[2]) = 0;

  virtual int opendir(const char* path, struct fuse_file_info*) = 0;
  virtual int readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset,
                      struct fuse_file_info* fi) = 0;
  virtual int releasedir(const char* path, struct fuse_file_info* fi) = 0;

private:
  Directory(const Directory&) = delete;
  Directory& operator=(const Directory&) = delete;
};

class SimpleDirectory : public Directory
{
private:
  std::unordered_map<std::string, DirectoryPtr> m_directories;
  std::unordered_map<std::string, FilePtr> m_files;

public:
  SimpleDirectory() :
    m_directories(),
    m_files()
  {}

  virtual ~SimpleDirectory() {}

  const std::unordered_map<std::string, DirectoryPtr>& get_directories() const
  {
    return m_directories;
  }

  const std::unordered_map<std::string, FilePtr>& get_files() const
  {
    return m_files;
  }

  void add_file(const std::string& name, FilePtr file)
  {
    m_files[name] = std::move(file);
  }

  void add_directory(const std::string& name, DirectoryPtr directory)
  {
    m_directories[name] = std::move(directory);
  }

  int getattr(const char* path, struct stat* stbuf) override
  {
    stbuf->st_mode = S_IFDIR | 0755;
    stbuf->st_nlink = 2;
    return 0;
  }

  int utimens(const char* path, const struct timespec tv[2]) override
  {
    return 0;
  }

  int opendir(const char* path, struct fuse_file_info* fi) override
  {
    return 0;
  }

  int readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset,
              struct fuse_file_info* fi) override
  {
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    for(const auto& it : m_directories)
    {
      filler(buf, it.first.c_str(), NULL, 0);
    }

    for(const auto& it : m_files)
    {
      filler(buf, it.first.c_str(), NULL, 0);
    }

    return 0;
  }

  int releasedir(const char* path, struct fuse_file_info* fi) override
  {
    return 0;
  }
};

#endif

/* EOF */
