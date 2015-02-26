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

#include <algorithm>
#include <fuse.h>
#include <iostream>
#include <map>
#include <memory>
#include <string.h>
#include <vector>

#include "concat_vfs.hpp"
#include "util.hpp"

int concat_getattr(const char* path, struct stat* stbuf)
{
  ConcatVFS* vfs = static_cast<ConcatVFS*>(fuse_get_context()->private_data);
  return vfs->getattr(path, stbuf);
}

int concat_utimens(const char* path, const struct timespec tv[2])
{
  ConcatVFS* vfs = static_cast<ConcatVFS*>(fuse_get_context()->private_data);
  return vfs->utimens(path, tv);
}

int concat_open(const char* path, struct fuse_file_info* fi)
{
  ConcatVFS* vfs = static_cast<ConcatVFS*>(fuse_get_context()->private_data);
  return vfs->open(path, fi);
}

int concat_read(const char* path, char* buf, size_t len, off_t offset,
                struct fuse_file_info* fi)
{
  ConcatVFS* vfs = static_cast<ConcatVFS*>(fuse_get_context()->private_data);
  return vfs->read(path, buf, len, offset, fi);
}

int concat_write(const char* path, const char* buf, size_t len, off_t offset,
                 struct fuse_file_info* fi)
{
  ConcatVFS* vfs = static_cast<ConcatVFS*>(fuse_get_context()->private_data);
  return vfs->write(path, buf, len, offset, fi);
}

int concat_flush(const char* path, struct fuse_file_info* fi)
{
  ConcatVFS* vfs = static_cast<ConcatVFS*>(fuse_get_context()->private_data);
  return vfs->flush(path, fi);
}

int concat_release(const char* path, struct fuse_file_info* fi)
{
  ConcatVFS* vfs = static_cast<ConcatVFS*>(fuse_get_context()->private_data);
  return vfs->release(path, fi);
}

int concat_opendir(const char* path, struct fuse_file_info* fi)
{
  ConcatVFS* vfs = static_cast<ConcatVFS*>(fuse_get_context()->private_data);
  return vfs->opendir(path, fi);
}

int concat_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset,
                   struct fuse_file_info* fi)
{
  ConcatVFS* vfs = static_cast<ConcatVFS*>(fuse_get_context()->private_data);
  return vfs->readdir(path, buf, filler, offset, fi);
}

int concat_releasedir(const char* path, struct fuse_file_info* fi)
{
  ConcatVFS* vfs = static_cast<ConcatVFS*>(fuse_get_context()->private_data);
  return vfs->releasedir(path, fi);
}

int concat_truncate(const char* path, off_t offsite)
{
  ConcatVFS* vfs = static_cast<ConcatVFS*>(fuse_get_context()->private_data);
  return vfs->truncate(path, offsite);
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

  auto vfs = make_unique<ConcatVFS>();
  return fuse_main(argc, argv, &ops, vfs.get());
}

/* EOF */
