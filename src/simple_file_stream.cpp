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

#include "simple_file_stream.hpp"

#include <string.h>

SimpleFileStream::SimpleFileStream(const std::string& data) :
  m_data(data)
{
}

ssize_t
SimpleFileStream::read(size_t pos, char* buf, size_t count)
{
  if (pos < m_data.size())
  {
    size_t len = std::min(m_data.size() - pos, count);
    memcpy(buf, m_data.data() + pos, len);
    return static_cast<ssize_t>(len);
  }
  else
  {
    return 0;
  }
}

size_t
SimpleFileStream::get_size() const
{
  return m_data.size();
}

/* EOF */
