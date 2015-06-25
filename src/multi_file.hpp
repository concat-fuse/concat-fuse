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

#ifndef HEADER_MULTI_FILE_HPP
#define HEADER_MULTI_FILE_HPP

#include <memory>
#include <string>
#include <sys/types.h>
#include <vector>
#include <shared_mutex>

#include "file_list.hpp"
#include "file.hpp"

class MultiFileStream;

class MultiFile : public File
{
private:
  size_t m_size;
  struct timespec m_mtime;

  std::unique_ptr<FileList> m_file_list;
  std::vector<FileInfo> m_files;
  std::unique_ptr<MultiFileStream> m_stream;

  std::shared_timed_mutex m_mutex;

public:
  MultiFile(std::unique_ptr<FileList> file_list);
  ~MultiFile();

  void refresh();

  int getattr(const char* path, struct stat* stbuf) override;
  int utimens(const char* path, const struct timespec tv[2]) override;

  int open(const char* path, struct fuse_file_info* fi) override;
  int release(const char* path, struct fuse_file_info* fi) override;

  int read(const char* path, char* buf, size_t len, off_t offset,
           struct fuse_file_info* fi) override;

private:
  MultiFile(const MultiFile&) = delete;
  MultiFile& operator=(const MultiFile&) = delete;
};

#endif

/* EOF */
