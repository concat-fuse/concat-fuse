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

#ifndef HEADER_ENTRY_HPP
#define HEADER_ENTRY_HPP

class ConcatVFS;

class Entry
{
protected:
  ConcatVFS& m_vfs;

public:
  Entry(ConcatVFS& vfs) : m_vfs(vfs) {}
  virtual ~Entry() {}

  virtual int getattr(const char* path, struct stat* stbuf) = 0;
  virtual int utimens(const char* path, const struct timespec tv[2]) = 0;

private:
  Entry(const Entry&) = delete;
  Entry& operator=(const Entry&) = delete;
};

#endif

/* EOF */
