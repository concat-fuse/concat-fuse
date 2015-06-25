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

#ifndef HEADER_CONTROL_FILE_HPP
#define HEADER_CONTROL_FILE_HPP

#include <vector>

#include "file.hpp"
#include "handle_store.hpp"

class SimpleDirectory;

class ControlFile : public File
{
public:
  enum  Mode { GLOB_MODE, LIST_MODE, ZIP_MODE };

private:
  SimpleDirectory& m_directory;
  Mode m_mode;
  HandleStore<std::string> m_tmpbuf;

public:
  ControlFile(SimpleDirectory& directory, Mode mode);
  ~ControlFile();

  int getattr(const char* path, struct stat* stbuf) override;

  int open(const char* path, struct fuse_file_info* fi) override;
  int write(const char* path, const char* buf, size_t len, off_t offset,
                    struct fuse_file_info* fi) override;
  int truncate(const char* path, off_t offsite) override;
  int release(const char* path, struct fuse_file_info* fi) override;

private:
  ControlFile(const ControlFile&) = delete;
  ControlFile& operator=(const ControlFile&) = delete;
};

#endif

/* EOF */
