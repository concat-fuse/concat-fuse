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

#include "simple_directory.hpp"

#include "concat_vfs.hpp"
#include "util.hpp"

SimpleDirectory::SimpleDirectory(ConcatVFS& vfs, const std::string& basedir) :
  Directory(vfs, basedir),
  m_directories(),
  m_files()
{
}

SimpleDirectory::~SimpleDirectory()
{
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
  m_vfs.add_entry(path_join(m_basedir, name), file.get());
  m_files[name] = std::move(file);
}

void
SimpleDirectory::add_directory(const std::string& name, DirectoryPtr directory)
{
  m_vfs.add_entry(path_join(m_basedir, name), directory.get());
  m_directories[name] = std::move(directory);
}

int
SimpleDirectory::getattr(const char* path, struct stat* stbuf)
{
  stbuf->st_mode = S_IFDIR | 0755;
  stbuf->st_nlink = 2;
  return 0;
}

int
SimpleDirectory::utimens(const char* path, const struct timespec tv[2])
{
  return 0;
}

int
SimpleDirectory::opendir(const char* path, struct fuse_file_info* fi)
{
  return 0;
}

int
SimpleDirectory::readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset,
                         struct fuse_file_info* fi)
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

int
SimpleDirectory::releasedir(const char* path, struct fuse_file_info* fi)
{
  return 0;
}

/* EOF */
