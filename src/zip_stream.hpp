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

#ifndef HEADER_ZIP_DATA_HPP
#define HEADER_ZIP_DATA_HPP

#include <memory>
#include <string>
#include <vector>

#include <unzip.h>

#include "stream.hpp"

class ZipStream : public Stream
{
private:
  struct ZipEntry
  {
    unz_file_pos pos;
    size_t uncompressed_size;
    std::string filename;
  };

private:
  ZipStream(unzFile fp);

public:
  static std::unique_ptr<ZipStream> open(const std::string& filename);
  ~ZipStream();

  size_t get_size() const;

  ssize_t read(size_t pos, char* buf, size_t count);

private:
  ssize_t find_file(size_t& offset);
  void read_index();
  void sort_index();

private:
  unzFile m_fp;
  size_t m_pos;
  size_t m_size;
  std::string m_filename;
  std::vector<ZipEntry> m_entries;

private:
  ZipStream(const ZipStream&) = delete;
  ZipStream& operator=(const ZipStream&) = delete;
};

#endif

/* EOF */
