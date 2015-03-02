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

#include "concat_vfs.hpp"

#include <inttypes.h>
#include <stdint.h>
#include <map>
#include <stdint.h>
#include <string.h>

#include "glob_file_list.hpp"
#include "multi_file.hpp"
#include "simple_directory.hpp"
#include "simple_file_list.hpp"
#include "util.hpp"

void
traverse_simple_directory(SimpleDirectory& directory,
                          std::unordered_map<std::string, Entry*>& m_entries,
                          const std::string& basedir,
                          const std::function<void ()>& on_change)
{
  directory.set_on_change(on_change);

  for(const auto& files : directory.get_files())
  {
    m_entries[path_join(basedir, files.first)] = files.second.get();
  }

  for(const auto& dir : directory.get_directories())
  {
    std::string path = path_join(basedir, dir.first);
    m_entries[path] = dir.second.get();
    traverse_simple_directory(static_cast<SimpleDirectory&>(*dir.second), m_entries, path, on_change);
  }
}

ConcatVFS::ConcatVFS() :
  m_mutex(),
  m_entries(),
  m_root()
{
  add_entry("/", m_root.get());
}

void
ConcatVFS::set_root(std::unique_ptr<SimpleDirectory>&& root)
{
  m_root = std::move(root);
  rebuild_entry_cache();
}

Entry*
ConcatVFS::lookup(const std::string& path)
{
  auto it = m_entries.find(path);
  if (it == m_entries.end())
  {
    return nullptr;
  }
  else
  {
    return it->second;
  }
}

void
ConcatVFS::add_entry(const std::string& path, Entry* entry)
{
  m_entries[path] = entry;
}

void
ConcatVFS::rebuild_entry_cache()
{
  log_debug("rebuild_entry_cache()");
  m_entries.clear();
  m_entries["/"] = m_root.get();
  traverse_simple_directory(*m_root, m_entries, "/", [this]{ rebuild_entry_cache(); });
}

SimpleDirectory&
ConcatVFS::get_root() const
{
  return *m_root;
}

int
ConcatVFS::getattr(const char* path, struct stat* stbuf)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  Entry* entry = lookup(path);
  if (entry)
  {
    memset(stbuf, 0, sizeof(*stbuf));
    stbuf->st_uid = fuse_get_context()->uid;
    stbuf->st_gid = fuse_get_context()->gid;
    return entry->getattr(path, stbuf);
  }
  else
  {
    return -ENOENT;
  }
}

int
ConcatVFS::utimens(const char* path, const struct timespec tv[2])
{
  std::lock_guard<std::mutex> lock(m_mutex);

  Entry* entry = lookup(path);
  if (entry)
  {
    return entry->utimens(path, tv);
  }
  else
  {
    return -ENOENT;
  }
}

int
ConcatVFS::open(const char* path, struct fuse_file_info* fi)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  Entry* entry = lookup(path);
  if (entry)
  {
    if (File* file = dynamic_cast<File*>(entry))
    {
      return file->open(path, fi);
    }
    else
    {
      return -EISDIR;
    }
  }
  else
  {
    return -ENOENT;
  }
}

int
ConcatVFS::read(const char* path, char* buf, size_t len, off_t offset,
                struct fuse_file_info* fi)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  Entry* entry = lookup(path);
  if (entry)
  {
    if (File* file = dynamic_cast<File*>(entry))
    {
      return file->read(path, buf, len, offset, fi);
    }
    else
    {
      return -EISDIR;
    }
  }
  else
  {
    return -ENOENT;
  }
}

int
ConcatVFS::write(const char* path, const char* buf, size_t len, off_t offset,
                 struct fuse_file_info* fi)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  Entry* entry = lookup(path);
  if (entry)
  {
    if (File* file = dynamic_cast<File*>(entry))
    {
      return file->write(path, buf, len, offset, fi);
    }
    else
    {
      return -EISDIR;
    }
  }
  else
  {
    return -ENOENT;
  }
}

int
ConcatVFS::flush(const char* path, struct fuse_file_info* fi)
{
  // called multiple times in a single write
  std::lock_guard<std::mutex> lock(m_mutex);

  Entry* entry = lookup(path);
  if (entry)
  {
    if (File* file = dynamic_cast<File*>(entry))
    {
      return file->flush(path, fi);
    }
    else
    {
      return -EISDIR;
    }
  }
  else
  {
    return -ENOENT;
  }
}

int
ConcatVFS::release(const char* path, struct fuse_file_info* fi)
{
  // called once for file close
  std::lock_guard<std::mutex> lock(m_mutex);

  Entry* entry = lookup(path);
  if (entry)
  {
    if (File* file = dynamic_cast<File*>(entry))
    {
      return file->release(path, fi);
    }
    else
    {
      return -EISDIR;
    }
  }
  else
  {
    return -ENOENT;
  }
}

int
ConcatVFS::opendir(const char* path, struct fuse_file_info* fi)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  Entry* entry = lookup(path);
  if (entry)
  {
    if (Directory* directory = dynamic_cast<Directory*>(entry))
    {
      return directory->opendir(path, fi);
    }
    else
    {
      return -ENOTDIR;
    }
  }
  else
  {
    return -ENOENT;
  }
}

int
ConcatVFS::readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset,
                   struct fuse_file_info* fi)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  Entry* entry = lookup(path);
  if (entry)
  {
    if (Directory* directory = dynamic_cast<Directory*>(entry))
    {
      return directory->readdir(path, buf, filler, offset, fi);
    }
    else
    {
      return -ENOTDIR;
    }
  }
  else
  {
    return -ENOENT;
  }
}

int
ConcatVFS::releasedir(const char* path, struct fuse_file_info* fi)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  Entry* entry = lookup(path);
  if (entry)
  {
    if (Directory* directory = dynamic_cast<Directory*>(entry))
    {
      return directory->releasedir(path, fi);
    }
    else
    {
      return -ENOTDIR;
    }
  }
  else
  {
    return -ENOENT;
  }
}

int
ConcatVFS::truncate(const char* path, off_t offset)
{
  // this is called before write and required
  std::lock_guard<std::mutex> lock(m_mutex);

  Entry* entry = lookup(path);
  if (entry)
  {
    if (File* file = dynamic_cast<File*>(entry))
    {
      return file->truncate(path, offset);
    }
    else
    {
      return -ENOTDIR;
    }
  }
  else
  {
    return -ENOENT;
  }
}

/* EOF */
