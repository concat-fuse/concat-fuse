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

#include "control_file.hpp"

#include <stdint.h>
#include <sys/stat.h>
#include <fuse.h>

#include "glob_file_list.hpp"
#include "multi_file.hpp"
#include "simple_directory.hpp"
#include "simple_file_list.hpp"
#include "util.hpp"

namespace {

uint64_t make_fh(size_t v)
{
  return static_cast<uint64_t>(v);
}

size_t fh2idx(uint64_t handle)
{
  return static_cast<size_t>(handle - UINT64_C(1));
}

} // namespace

ControlFile::ControlFile(SimpleDirectory& directory, Mode mode) :
  m_directory(directory),
  m_mode(mode),
  m_tmpbuf()
{
}

ControlFile::~ControlFile()
{
}

int
ControlFile::getattr(const char* path, struct stat* stbuf)
{
  stbuf->st_mode = S_IFREG | 0644;
  stbuf->st_nlink = 2;
  stbuf->st_size = 0;
  return 0;
}

int
ControlFile::open(const char* path, struct fuse_file_info* fi)
{
  m_tmpbuf.push_back({});
  fi->fh = make_fh(m_tmpbuf.size());
  fi->direct_io = 1;

  log_debug("ControlFile::open {}", fi->fh);

  return 0;
}

int
ControlFile::write(const char* path, const char* buf, size_t len, off_t offset,
                   struct fuse_file_info* fi)
{
  m_tmpbuf[fh2idx(fi->fh)].append(buf, len);
  return static_cast<int>(len);
}

int
ControlFile::truncate(const char* path, off_t offsite)
{
  return 0;
}

int
ControlFile::release(const char* path, struct fuse_file_info* fi)
{
  const std::string& data = m_tmpbuf[fh2idx(fi->fh)];
  std::string sha1 = sha1sum(data);

  log_debug("RECEIVED: {}", sha1);

  auto it = m_directory.get_files().find(sha1);
  if (it != m_directory.get_files().end())
  {
    dynamic_cast<MultiFile&>(*it->second).refresh();
    return 0;
  }
  else
  {
    switch(m_mode)
    {
      case GLOB_MODE:
        m_directory.add_file(sha1,
                             make_unique<MultiFile>(make_unique<GlobFileList>(split(data, '\0'))));
        return 0;

      case LIST_MODE:
        m_directory.add_file(sha1,
                             make_unique<MultiFile>(make_unique<SimpleFileList>(split(data, '\0'))));
        return 0;

      default:
        return 0;
    }
  }
}

/* EOF */
