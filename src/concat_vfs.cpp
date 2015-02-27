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
#include <map>
#include <stdint.h>
#include <string.h>

#include "simple_file_list.hpp"
#include "glob_file_list.hpp"
#include "multi_file.hpp"
#include "util.hpp"

ConcatVFS::ConcatVFS() :
  m_mutex(),
  m_from_file0_tmpbuf(),
  m_from_file0_multi_files(),
  m_from_glob0_tmpbuf(),
  m_from_glob0_multi_files()
{}

int
ConcatVFS::getattr(const char* path, struct stat* stbuf)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  log_debug("getattr({})", path);

  memset(stbuf, 0, sizeof(*stbuf));

  stbuf->st_uid = fuse_get_context()->uid;
  stbuf->st_gid = fuse_get_context()->gid;

  if (strcmp(path, "/") == 0 ||
      strcmp(path, "/from-file0") == 0 ||
      strcmp(path, "/from-glob0") == 0)
  {
    stbuf->st_mode = S_IFDIR | 0755;
    stbuf->st_nlink = 2;
    return 0;
  }
  else if (has_prefix(path, "/from-file0/"))
  {
    if (strcmp(path, "/from-file0/control") == 0)
    {
      stbuf->st_mode = S_IFREG | 0644;
      stbuf->st_nlink = 2;
      stbuf->st_size = 0;
      return 0;
    }
    else
    {
      std::string filename = path + strlen("/from-file0/");

      auto it = m_from_file0_multi_files.find(filename);
      if (it == m_from_file0_multi_files.end())
      {
        return -ENOENT;
      }
      else
      {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 2;
        stbuf->st_size = it->second->get_size();
        return 0;
      }
    }
  }
  else if (has_prefix(path, "/from-glob0/"))
  {
    if (strcmp(path, "/from-glob0/control") == 0)
    {
      stbuf->st_mode = S_IFREG | 0644;
      stbuf->st_nlink = 2;
      stbuf->st_size = 0;
      return 0;
    }
    else
    {
      std::string filename = path + strlen("/from-glob0/");

      auto it = m_from_glob0_multi_files.find(filename);
      if (it == m_from_glob0_multi_files.end())
      {
        return -ENOENT;
      }
      else
      {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 2;
        stbuf->st_size = it->second->get_size();
        return 0;
      }
    }
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
  log_debug("utimens({})", path);
  return -ENOENT;
}

int
ConcatVFS::open(const char* path, struct fuse_file_info* fi)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  log_debug("open({}, {})", path, fi->fh);

  if (has_prefix(path, "/from-file0/"))
  {
    if (strcmp(path, "/from-file0/control") == 0)
    {
      m_from_file0_tmpbuf.push_back("");
      fi->fh = m_from_file0_tmpbuf.size();
      return 0;
    }
    else
    {
      std::string filename = path + strlen("/from-file0/");
      auto it = m_from_file0_multi_files.find(filename);
      if (it == m_from_file0_multi_files.end())
      {
        return -ENOENT;
      }
      else
      {
        return 0;
      }
    }
  }
  else if (has_prefix(path, "/from-glob0/"))
  {
    if (strcmp(path, "/from-glob0/control") == 0)
    {
      m_from_glob0_tmpbuf.push_back("");
      fi->fh = m_from_glob0_tmpbuf.size();
      return 0;
    }
    else
    {
      std::string filename = path + strlen("/from-glob0/");
      auto it = m_from_glob0_multi_files.find(filename);
      if (it == m_from_glob0_multi_files.end())
      {
        return -ENOENT;
      }
      else
      {
        return 0;
      }
    }
  }
  else
  {
    return 0;
  }
}

int
ConcatVFS::read(const char* path, char* buf, size_t len, off_t offset,
                struct fuse_file_info*)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  log_debug("read({})", path);

  if (has_prefix(path, "/from-file0/"))
  {
    if (strcmp(path, "/from-file0/control") == 0)
    {
      return -EPERM;
    }
    else
    {
      std::string filename = path + strlen("/from-file0/");
      auto it = m_from_file0_multi_files.find(filename);
      if (it != m_from_file0_multi_files.end())
      {
        return static_cast<int>(it->second->read(static_cast<size_t>(offset), buf, len));
      }
      else
      {
        return 0;
      }
    }
  }
  else if (has_prefix(path, "/from-glob0/"))
  {
    if (strcmp(path, "/from-glob0/control") == 0)
    {
      return -EPERM;
    }
    else
    {
      std::string filename = path + strlen("/from-glob0/");
      auto it = m_from_glob0_multi_files.find(filename);
      if (it != m_from_glob0_multi_files.end())
      {
        return static_cast<int>(it->second->read(static_cast<size_t>(offset), buf, len));
      }
      else
      {
        return 0;
      }
    }
  }
  else
  {
    return -ENOENT;
  }
}

int ConcatVFS::write(const char* path, const char* buf, size_t len, off_t offset,
                     struct fuse_file_info* fi)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  log_debug("write({}) -> {}", path, fi->fh);

  if (strcmp(path, "/from-file0/control") == 0)
  {
    m_from_file0_tmpbuf[static_cast<size_t>(fi->fh - 1)].append(buf, len);
    return static_cast<int>(len);
  }
  else if (strcmp(path, "/from-glob0/control") == 0)
  {
    m_from_glob0_tmpbuf[static_cast<size_t>(fi->fh - 1)].append(buf, len);
    return static_cast<int>(len);
  }
  else
  {
    return -ENOENT;
  }
}

int ConcatVFS::flush(const char* path, struct fuse_file_info*)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  // called multiple times in a single write
  log_debug("flush({})", path);
  return 0;
}

int
ConcatVFS::release(const char* path, struct fuse_file_info* fi)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  // called once for file close
  log_debug("release({}) -> {}", path, fi->fh);

  if (strcmp(path, "/from-file0/control") == 0)
  {
    const std::string& data = m_from_file0_tmpbuf[static_cast<size_t>(fi->fh - 1)];
    std::string sha1 = sha1sum(data);

    log_debug("RECEIVED: {}", sha1);

    auto it = m_from_file0_multi_files.find(sha1);
    if (it == m_from_file0_multi_files.find(sha1))
    {
      m_from_file0_multi_files[sha1] = make_unique<MultiFile>(make_unique<SimpleFileList>(split(data, '\0')));
      return 0;
    }
    else
    {
      // do nothing, multifile is already there
      return 0;
    }
  }
  else if (strcmp(path, "/from-glob0/control") == 0)
  {
    const std::string& data = m_from_glob0_tmpbuf[static_cast<size_t>(fi->fh - 1)];
    std::string sha1 = sha1sum(data);

    log_debug("RECEIVED: {}", sha1);

    auto it = m_from_glob0_multi_files.find(sha1);
    if (it == m_from_glob0_multi_files.find(sha1))
    {
      m_from_glob0_multi_files[sha1] = make_unique<MultiFile>(make_unique<GlobFileList>(split(data, '\0')));
      return 0;
    }
    else
    {
      // do nothing, multifile is already there
      return 0;
    }
  }
  else
  {
    return 0;
  }
}

int
ConcatVFS::opendir(const char* path, struct fuse_file_info*)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  log_debug("opendir({})", path);
  return 0;
}

int
ConcatVFS::readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset,
                   struct fuse_file_info* fi)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  log_debug("readdir({})", path);

  if (strcmp(path, "/") == 0)
  {
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, "from-file0", NULL, 0);
    filler(buf, "from-glob0", NULL, 0);
    return 0;
  }
  else if (strcmp(path, "/from-file0") == 0)
  {
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, "control", NULL, 0);
    for(const auto& it : m_from_file0_multi_files)
    {
      filler(buf, it.first.c_str(), NULL, 0);
    }
    return 0;
  }
  else if (strcmp(path, "/from-glob0") == 0)
  {
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, "control", NULL, 0);
    for(const auto& it : m_from_glob0_multi_files)
    {
      filler(buf, it.first.c_str(), NULL, 0);
    }
    return 0;
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
  log_debug("releasedir({}, {})", path, fi->fh);
  return 0;
}

int
ConcatVFS::truncate(const char* path, off_t offset)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  // this is called before write and required
  log_debug("releasedir({}, {})", path, offset);
  return 0;
}

/* EOF */
