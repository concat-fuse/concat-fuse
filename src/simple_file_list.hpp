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

#ifndef HEADER_SIMPLE_FILE_LIST_HPP
#define HEADER_SIMPLE_FILE_LIST_HPP

#include <memory>

#include "file_list.hpp"

class SimpleFileList : public FileList
{
private:
  std::vector<std::string> m_files;

public:
  static std::unique_ptr<SimpleFileList> from_file0(const std::string& data);
  static std::unique_ptr<SimpleFileList> from_file(const std::string& data);

public:
  SimpleFileList(const std::vector<std::string>& files) :
    m_files(files)
  {}

  std::vector<FileInfo> scan() const override;

private:
  SimpleFileList(const SimpleFileList&) = delete;
  SimpleFileList& operator=(const SimpleFileList&) = delete;
};

#endif

/* EOF */
