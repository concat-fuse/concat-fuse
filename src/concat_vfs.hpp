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

#ifndef HEADER_CONCAT_VFS_HPP
#define HEADER_CONCAT_VFS_HPP

#include <fuse.h>
#include <map>
#include <memory>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <mutex>

#include "multi_file.hpp"

class ConcatVFS
{
private:
  std::mutex m_mutex;

  std::vector<std::string> m_from_file0_tmpbuf;
  std::map<std::string, std::unique_ptr<MultiFile> > m_from_file0_multi_files;

  std::vector<std::string> m_from_glob0_tmpbuf;
  std::map<std::string, std::unique_ptr<MultiFile> > m_from_glob0_multi_files;

public:
  ConcatVFS();

  int getattr(const char* path, struct stat* stbuf);
  int utimens(const char* path, const struct timespec tv[2]);

  int open(const char* path, struct fuse_file_info* fi);
  int read(const char* path, char* buf, size_t len, off_t offset,
           struct fuse_file_info*);
  int write(const char* path, const char* buf, size_t len, off_t offset,
            struct fuse_file_info* fi);
  int truncate(const char* path, off_t offsite);
  int flush(const char* path, struct fuse_file_info*);
  int release(const char* path, struct fuse_file_info* fi);

  int opendir(const char* path, struct fuse_file_info*);
  int readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset,
              struct fuse_file_info* fi);
  int releasedir(const char* path, struct fuse_file_info* fi);

private:
  ConcatVFS(const ConcatVFS&) = delete;
  ConcatVFS& operator=(const ConcatVFS&) = delete;
};

#endif

/* EOF */
