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

#include "util.hpp"
#include "multi_file.hpp"

using MultiFilePtr = std::unique_ptr<MultiFile>;
std::vector<MultiFilePtr> g_multi_files;

int concat_getattr(const char* path, struct stat* stbuf)
{
  std::cout << "getattr(" << path << ")" << std::endl;

  memset(stbuf, 0, sizeof(*stbuf));
  
  if (strcmp(path, "/") == 0)
  {
    stbuf->st_mode = S_IFDIR | 0755;
    stbuf->st_nlink = 2;
    return 0;
  }
  else if (strcmp(path, "/by-glob") == 0)
  {
    stbuf->st_mode = S_IFDIR | 0755;
    stbuf->st_nlink = 2;
    return 0;
  }
  else if (has_prefix(path, "/by-glob/"))
  {
    std::string pattern = url_unquote(path + 8);
    
    std::cout << "PATERN: '" << pattern << "'" << std::endl;

    auto it = std::find_if(g_multi_files.begin(), g_multi_files.end(),
                           [pattern](const MultiFilePtr& multi_file) {
                             return multi_file->get_pattern() == pattern;
                           });

    if (it == g_multi_files.end())
    {
      auto multi_file = std::make_unique<MultiFile>(pattern);

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

int concat_open(const char* path, struct fuse_file_info*)
{
  std::cout << "open(" << path << ")" << std::endl;
  return 0;
}

int concat_read(const char* path, char* buf, size_t len, off_t offset,
                struct fuse_file_info*)
{
  std::cout << "read(" << path << ")" << std::endl;

  if (has_prefix(path, "/by-glob/"))
  {
    std::string pattern = url_unquote(path + 8);

    auto it = std::find_if(g_multi_files.begin(), g_multi_files.end(),
                           [pattern](const MultiFilePtr& multi_file) {
                             return multi_file->get_pattern() == pattern;
                           });

    if (it != g_multi_files.end())
    {
      return static_cast<int>((*it)->read(offset, buf, len));
    }
    else
    {
      return -ENOENT;
    }
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
    return 0;
  }
  else if (strcmp(path, "/by-glob") == 0)
  {
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    for(const auto& multi_file : g_multi_files)
    {
      filler(buf, multi_file->get_pattern().c_str(), NULL, 0);
    }
    
    return 0;
  }
  else
  {
    return -ENOENT;
  }
}

int concat_releasedir(const char* path, struct fuse_file_info* )
{
  std::cout << "releasedir(" << path << ")" << std::endl;
  return 0;
}

int concat_release(const char* path, struct fuse_file_info*)
{
  std::cout << "release(" << path << ")" << std::endl;
  return 0;
}

int concat_fuse_main(int argc, char** argv)
{
  struct fuse_operations ops;
  memset(&ops, 0, sizeof(ops));

  ops.getattr = concat_getattr;
  ops.open = concat_open;
  ops.release = concat_release;  
  ops.read = concat_read;
  ops.opendir = concat_opendir;
  ops.readdir = concat_readdir;
  ops.releasedir = concat_releasedir;

  return fuse_main(argc, argv, &ops, nullptr);
}

/* EOF */
