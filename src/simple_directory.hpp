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

#ifndef HEADER_SIMPLE_DIRECTORY_HPP
#define HEADER_SIMPLE_DIRECTORY_HPP

#include "directory.hpp"

class SimpleDirectory : public Directory
{
private:
  std::unordered_map<std::string, DirectoryPtr> m_directories;
  std::unordered_map<std::string, FilePtr> m_files;

  std::function<void ()> m_on_change;

public:
  SimpleDirectory();
  virtual ~SimpleDirectory();

  void set_on_change(const std::function<void ()>& on_change);

  const std::unordered_map<std::string, DirectoryPtr>& get_directories() const;
  const std::unordered_map<std::string, FilePtr>& get_files() const;

  void add_file(const std::string& name, FilePtr file);
  void add_directory(const std::string& name, DirectoryPtr directory);

  int getattr(const char* path, struct stat* stbuf) override;
  int opendir(const char* path, struct fuse_file_info*) override;
  int readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset,
              struct fuse_file_info* fi) override;
  int releasedir(const char* path, struct fuse_file_info* fi) override;

};

#endif

/* EOF */
