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

#include "simple_directory.hpp"

#include "concat_vfs.hpp"
#include "util.hpp"

SimpleDirectory::SimpleDirectory() :
  m_directories(),
  m_files(),
  m_on_change()
{
}

SimpleDirectory::~SimpleDirectory()
{
}

void
SimpleDirectory::set_on_change(const std::function<void ()>& on_change)
{
  m_on_change = on_change;
}

const std::unordered_map<std::string, DirectoryPtr>&
SimpleDirectory::get_directories() const
{
  return m_directories;
}

const std::unordered_map<std::string, FilePtr>&
SimpleDirectory::get_files() const
{
  return m_files;
}

void
SimpleDirectory::add_file(const std::string& name, FilePtr file)
{
  m_files[name] = std::move(file);
  if (m_on_change)
  {
    m_on_change();
  }
}

void
SimpleDirectory::add_directory(const std::string& name, DirectoryPtr directory)
{
  directory->set_parent(this);
  m_directories[name] = std::move(directory);
  if (m_on_change)
  {
    m_on_change();
  }
}

int
SimpleDirectory::getattr(const char* path, struct stat* stbuf)
{
  stbuf->st_mode = S_IFDIR | 0755;
  stbuf->st_nlink = 2;
  return 0;
}

int
SimpleDirectory::readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset,
                         struct fuse_file_info* fi)
{
  filler(buf, ".", nullptr, 0);
  filler(buf, "..", nullptr, 0);

  for(const auto& it : m_directories)
  {
    filler(buf, it.first.c_str(), nullptr, 0);
  }

  for(const auto& it : m_files)
  {
    filler(buf, it.first.c_str(), nullptr, 0);
  }

  return 0;
}

int
SimpleDirectory::opendir(const char* path, struct fuse_file_info*)
{
  return 0;
}

int
SimpleDirectory::releasedir(const char* path, struct fuse_file_info* fi)
{
  return 0;
}

/* EOF */
