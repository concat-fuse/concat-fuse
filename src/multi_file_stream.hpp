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

#ifndef HEADER_MULTI_FILE_STREAM_HPP
#define HEADER_MULTI_FILE_STREAM_HPP

#include <vector>

#include "file_list.hpp"
#include "stream.hpp"

class MultiFileStream final : public Stream
{
private:
  std::vector<FileInfo>& m_files;

public:
  MultiFileStream(std::vector<FileInfo>& file_list);

  ssize_t read(size_t pos, char* buf, size_t count) override;
  size_t get_size() const override;

private:
  int find_file(size_t* offset) const;
  void read_subfile(const std::string& filename, size_t offset, char* buf, size_t count) const;

private:
  MultiFileStream(const MultiFileStream&) = delete;
  MultiFileStream& operator=(const MultiFileStream&) = delete;
};

#endif

/* EOF */
