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

#ifndef HEADER_STREAM_HPP
#define HEADER_STREAM_HPP

#include <stddef.h>

class Stream
{
public:
  virtual ~Stream() {}
  virtual ssize_t read(size_t pos, char* buf, size_t count) = 0;
  virtual size_t get_size() const = 0;
};

#endif

/* EOF */
