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

#ifndef HEADER_MULTI_FILE_HPP
#define HEADER_MULTI_FILE_HPP

#include <string>
#include <vector>

struct FileInfo
{
  std::string filename;
  size_t size;
};

class MultiFile
{
private:
  std::string m_pattern;
  size_t m_pos;

  std::vector<FileInfo> m_files;
  size_t m_total_size;
  
public:
  MultiFile(const std::string& pattern);
  
  ssize_t read(size_t pos, char* buf, size_t count);

  std::string get_pattern() const { return m_pattern; }
  size_t get_size() const { return m_total_size; }
  
private:
  void collect_file_info();
  int find_file(size_t* offset);
  void read_subfile(const std::string& filename, size_t offset, char* buf, size_t count);
  
private:
  MultiFile(const MultiFile&) = delete;
  MultiFile& operator=(const MultiFile&) = delete;
};

#endif

/* EOF */
