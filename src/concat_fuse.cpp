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
#include <map>
#include <memory>
#include <string.h>
#include <vector>

#include "concat_vfs.hpp"
#include "control_file.hpp"
#include "simple_directory.hpp"
#include "simple_file.hpp"
#include "util.hpp"

int concat_getattr(const char* path, struct stat* stbuf)
{
  // filter noise out, as gettattr("/") is called each second for some
  // reason.
  if (strcmp(path, "/") != 0)
  {
    log_debug("getattr(\"{}\", {})", path, stbuf);
  }

  ConcatVFS* vfs = static_cast<ConcatVFS*>(fuse_get_context()->private_data);
  return vfs->getattr(path, stbuf);
}

int concat_utimens(const char* path, const struct timespec tv[2])
{
  log_debug("utimens(\"{}\", ...)", path);

  ConcatVFS* vfs = static_cast<ConcatVFS*>(fuse_get_context()->private_data);
  return vfs->utimens(path, tv);
}

int concat_open(const char* path, struct fuse_file_info* fi)
{
  log_debug("open({}, {})", path, fi);

  ConcatVFS* vfs = static_cast<ConcatVFS*>(fuse_get_context()->private_data);
  return vfs->open(path, fi);
}

int concat_release(const char* path, struct fuse_file_info* fi)
{
  log_debug("release({}) -> {}", path, fi->fh);

  ConcatVFS* vfs = static_cast<ConcatVFS*>(fuse_get_context()->private_data);
  return vfs->release(path, fi);
}

int concat_read(const char* path, char* buf, size_t len, off_t offset,
                struct fuse_file_info* fi)
{
  log_debug("read({}, {}, {}, {}, {})", path, static_cast<void*>(buf), len, offset, fi);

  ConcatVFS* vfs = static_cast<ConcatVFS*>(fuse_get_context()->private_data);
  return vfs->read(path, buf, len, offset, fi);
}

int concat_write(const char* path, const char* buf, size_t len, off_t offset,
                 struct fuse_file_info* fi)
{
  log_debug("write({}) -> {}", path, fi->fh);

  ConcatVFS* vfs = static_cast<ConcatVFS*>(fuse_get_context()->private_data);
  return vfs->write(path, buf, len, offset, fi);
}

int concat_flush(const char* path, struct fuse_file_info* fi)
{
  log_debug("flush({})", path);

  ConcatVFS* vfs = static_cast<ConcatVFS*>(fuse_get_context()->private_data);
  return vfs->flush(path, fi);
}

int concat_opendir(const char* path, struct fuse_file_info* fi)
{
  log_debug("opendir({})", path);

  ConcatVFS* vfs = static_cast<ConcatVFS*>(fuse_get_context()->private_data);
  return vfs->opendir(path, fi);
}

int concat_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset,
                   struct fuse_file_info* fi)
{
  log_debug("readdir({})", path);

  ConcatVFS* vfs = static_cast<ConcatVFS*>(fuse_get_context()->private_data);
  return vfs->readdir(path, buf, filler, offset, fi);
}

int concat_releasedir(const char* path, struct fuse_file_info* fi)
{
  log_debug("releasedir({}, {})", path, fi->fh);

  ConcatVFS* vfs = static_cast<ConcatVFS*>(fuse_get_context()->private_data);
  return vfs->releasedir(path, fi);
}

int concat_truncate(const char* path, off_t offset)
{
  log_debug("truncate({}, {})", path, offset);

  ConcatVFS* vfs = static_cast<ConcatVFS*>(fuse_get_context()->private_data);
  return vfs->truncate(path, offset);
}

fuse_operations
init_concat_fuse_ops()
{
  fuse_operations ops;

  memset(&ops, 0, sizeof(ops));

  ops.getattr = concat_getattr;
  ops.utimens = concat_utimens;

  ops.open = concat_open;
  ops.release = concat_release;

  ops.read = concat_read;
  ops.write = concat_write;
  ops.flush = concat_flush;
  ops.truncate = concat_truncate;

  ops.opendir = concat_opendir;
  ops.releasedir = concat_releasedir;
  ops.readdir = concat_readdir;

  return ops;
}

std::unique_ptr<SimpleDirectory>
init_concat_fuse_root()
{
  auto root = make_unique<SimpleDirectory>();

  root->add_file("VERSION", make_unique<SimpleFile>("2\n"));

  auto from_file_dir = make_unique<SimpleDirectory>();
  from_file_dir->add_file("control", make_unique<ControlFile>(*from_file_dir, ControlFile::LIST_MODE));
  root->add_directory("from-file0", std::move(from_file_dir));

  auto from_glob_dir = make_unique<SimpleDirectory>();
  from_glob_dir->add_file("control", make_unique<ControlFile>(*from_glob_dir, ControlFile::GLOB_MODE));
  root->add_directory("from-glob0", std::move(from_glob_dir));

  auto from_zip_dir = make_unique<SimpleDirectory>();
  from_zip_dir->add_file("control", make_unique<ControlFile>(*from_zip_dir, ControlFile::ZIP_MODE));
  root->add_directory("from-zip", std::move(from_zip_dir));

  return root;
}

int concat_fuse_main(int argc, char** argv)
{
  auto ops = init_concat_fuse_ops();
  auto root = init_concat_fuse_root();
  auto vfs = make_unique<ConcatVFS>();

  vfs->set_root(std::move(root));

  return fuse_main(argc, argv, &ops, vfs.get());
}

/* EOF */
