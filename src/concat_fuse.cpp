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

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

#include "glob_file_list.hpp"
#include "multi_file.hpp"
#include "util.hpp"

using MultiFilePtr = std::unique_ptr<MultiFile>;
std::vector<MultiFilePtr> g_multi_files;

std::vector<std::string> g_from_file;
std::vector<std::string> g_from_file0;

int concat_getattr(const char* path, struct stat* stbuf)
{
  std::cout << "getattr(" << path << ")" << std::endl;

  memset(stbuf, 0, sizeof(*stbuf));

  if (strcmp(path, "/") == 0 ||
      strcmp(path, "/by-glob") == 0 ||
      strcmp(path, "/from-file") == 0 ||
      strcmp(path, "/from-file0") == 0)
  {
    stbuf->st_mode = S_IFDIR | 0755;
    stbuf->st_nlink = 2;
    return 0;
  }
  else if (strcmp(path, "/from-file/control") == 0 ||
           strcmp(path, "/from-file0/control") == 0)
  {
    stbuf->st_mode = S_IFREG | 0644;
    stbuf->st_nlink = 2;
    stbuf->st_size = 0;
    return 0;
  }
  else if (has_prefix(path, "/from-file/"))
  {
    //stbuf->st_mode = S_IFREG | 0444;
    //stbuf->st_nlink = 2;
    //stbuf->st_size = 0;
    return -ENOENT;
  }
  else if (has_prefix(path, "/from-file0/"))
  {
    //stbuf->st_mode = S_IFREG | 0444;
    //stbuf->st_nlink = 2;
    //stbuf->st_size = 0;
    return -ENOENT;
  }
  else if (has_prefix(path, "/by-glob/"))
  {
    std::string pattern = url_unquote(path + 9);

    std::cout << "PATERN: '" << pattern << "'" << std::endl;

    auto it = g_multi_files.end();
#if 0
      std::find_if(g_multi_files.begin(), g_multi_files.end(),
                           [pattern](const MultiFilePtr& multi_file) {
                             return multi_file->get_pattern() == pattern;
                           });
#endif

    if (it == g_multi_files.end())
    {
      std::vector<std::string> patterns{patterns};
      auto multi_file = std::make_unique<MultiFile>(std::make_unique<GlobFileList>(patterns));

      std::cout << "Making new multifile" << std::endl;

      stbuf->st_mode = S_IFREG | 0444;
      stbuf->st_nlink = 2;
      stbuf->st_size = multi_file->get_size();

      g_multi_files.push_back(std::move(multi_file));
    }
    else
    {
      stbuf->st_mode = S_IFREG | 0444;
      stbuf->st_nlink = 2;
      stbuf->st_size = (*it)->get_size();
    }

    return 0;
  }
  else
  {
    return -ENOENT;
  }
}

int concat_utimens(const char* path, const struct timespec tv[2])
{
  std::cout << "utimens(" << path << ")" << std::endl;
  return -ENOENT;
}

int concat_open(const char* path, struct fuse_file_info* fi)
{
  std::cout << "open(" << path << ") " << fi->fh << std::endl;

  if (strcmp(path, "/from-file/control") == 0)
  {
    g_from_file.push_back("");
    fi->fh = g_from_file.size();
    return 0;
  }
  else if (strcmp(path, "/from-file0/control") == 0)
  {
    g_from_file0.push_back("");
    fi->fh = g_from_file0.size();
    return 0;
  }
  else
  {
    return 0;
  }
}

int concat_release(const char* path, struct fuse_file_info* fi)
{
  std::cout << "release(" << path << ") " << fi->fh << std::endl;
  if (strcmp(path, "/from-file/control") == 0)
  {
    std::cout << "RECEIVED: " << g_from_file[fi->fh - 1] << std::endl;
    return 0;
  }
  else if (strcmp(path, "/from-file0/control") == 0)
  {
    std::cout << "RECEIVED: " << g_from_file[fi->fh - 1] << std::endl;
    return 0;
  }
  else
  {
    return 0;
  }
}

int concat_flush(const char* path, struct fuse_file_info*)
{
  std::cout << "flush(" << path << ")" << std::endl;
  return 0;
}

int concat_read(const char* path, char* buf, size_t len, off_t offset,
                struct fuse_file_info*)
{
  std::cout << "read(" << path << ")" << std::endl;

  if (has_prefix(path, "/by-glob/"))
  {
    std::string pattern = url_unquote(path + 9);

#if 0
    auto it = std::find_if(g_multi_files.begin(), g_multi_files.end(),
                           [pattern](const MultiFilePtr& multi_file) {
                             return multi_file->get_pattern() == pattern;
                           });

    if (it != g_multi_files.end())
    {
      return static_cast<int>((*it)->read(static_cast<size_t>(offset), buf, len));
    }
    else
    {
      return -ENOENT;
    }
#endif
    return -ENOENT;
  }
  else
  {
    return -ENOENT;
  }
}

int concat_write(const char* path, const char* buf, size_t len, off_t offset,
                 struct fuse_file_info* fi)
{
  std::cout << "write(" << path << "): " << fi->fh << std::endl;

  if (strcmp(path, "/from-file/control") == 0)
  {
    std::cout << "STUFF: " << g_from_file.size() << " " << std::endl;
    std::cout.write(buf, len);
    std::cout << std::endl;
    g_from_file[fi->fh - 1].append(buf, len);
    return static_cast<int>(len);
  }
  else if (strcmp(path, "/from-file0/control") == 0)
  {
    g_from_file0[fi->fh - 1].append(buf, len);
    return static_cast<int>(len);
  }
  else
  {
    return -ENOENT;
  }
}

int concat_opendir(const char* path, struct fuse_file_info*)
{
  std::cout << "opendir(" << path << ")" << std::endl;
  return 0;
}

int concat_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset,
                   struct fuse_file_info* fi)
{
  std::cout << "readdir(" << path << ")" << std::endl;

  if (strcmp(path, "/") == 0)
  {
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, "by-glob", NULL, 0);
    filler(buf, "from-file", NULL, 0);
    filler(buf, "from-file0", NULL, 0);
    return 0;
  }
  else if (strcmp(path, "/by-glob") == 0)
  {
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

#if 0
      // FIXME: broken
      for(const auto& multi_file : g_multi_files)
      {
        filler(buf, url_quote(multi_file->get_pattern()).c_str(), NULL, 0);
      }
#endif

    return 0;
  }
  else if (strcmp(path, "/from-file") == 0)
  {
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, "control", NULL, 0);
    return 0;
  }
  else if (strcmp(path, "/from-file0") == 0)
  {
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, "control", NULL, 0);
    return 0;
  }
  else
  {
    return -ENOENT;
  }
}

int concat_releasedir(const char* path, struct fuse_file_info* fi)
{
  std::cout << "releasedir(" << path << ")" << std::endl;
  return 0;
}

int concat_truncate(const char* path, off_t offsite)
{
  // this is called before write
  std::cout << "truncate(" << path << ", " << offsite << ")" << std::endl;
  return 0;
}

int concat_fallocate(const char* path, int, off_t, off_t, struct fuse_file_info*)
{
  std::cout << "fallocate(" << path << ")" << std::endl;
  return -ENOENT;
}

int concat_fuse_main(int argc, char** argv)
{
  struct fuse_operations ops;
  memset(&ops, 0, sizeof(ops));

  ops.getattr = concat_getattr;
  ops.utimens = concat_utimens;
  ops.open = concat_open;
  ops.flush = concat_flush;
  ops.release = concat_release;
  ops.read = concat_read;
  ops.write = concat_write;
  ops.opendir = concat_opendir;
  ops.readdir = concat_readdir;
  ops.releasedir = concat_releasedir;
  ops.truncate = concat_truncate;
  ops.fallocate = concat_fallocate;

  return fuse_main(argc, argv, &ops, nullptr);
}

/* EOF */
