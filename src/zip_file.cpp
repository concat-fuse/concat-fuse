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

#include "zip_file.hpp"

#include <algorithm>
#include <sys/stat.h>

#include "util.hpp"

ZipFile::ZipFile(const std::string& filename) :
  m_pos(0),
  m_size(0),
  m_mtime(),
  m_filename(filename),
  m_fp(),
  m_entries()
{
  // FIXME:
  // * options to filter zip file content and customize sorting should
  //   be provided to the user

  m_fp = unzOpen(m_filename.c_str());
  if (!m_fp)
  {
    throw std::runtime_error("failed to open " + m_filename);
  }

  // read .zip index
  unzGoToFirstFile(m_fp);
  do
  {
    unz_file_pos pos;
    unzGetFilePos(m_fp, &pos);

    unz_file_info file_info;
    char filename_c[4096];
    unzGetCurrentFileInfo(
      m_fp,
      &file_info,
      filename_c, sizeof(filename_c),
      nullptr, 0, // extrafield
      nullptr, 0 // comment
      );

    m_entries.push_back(ZipEntry{ pos, file_info.uncompressed_size, filename_c});
    m_size += file_info.uncompressed_size;
  }
  while(unzGoToNextFile(m_fp) == UNZ_OK);

  // entries in the .zip might not be ordered, so sort them
  std::sort(m_entries.begin(), m_entries.end(),
            [](ZipEntry const& lhs, ZipEntry const& rhs){
              return lhs.filename < rhs.filename;
            });

  log_debug("{}: size={} entries={}", m_filename, m_size, m_entries.size());
}

ZipFile::~ZipFile()
{
  unzClose(m_fp);
}

ssize_t
ZipFile::read(size_t pos, char* buf, size_t count)
{
  // FIXME:
  // * seeking should check the current position with unztell() and
  //   the current file and avoid redundant seeking
  // * unzCloseCurrentFile() is never called
  // * error checking
  // * 64bit functions should be used instead of 32bit ones

  // clip read request to available data
  if (pos + count > m_size)
  {
    count = m_size - pos;
  }

  size_t total_count = 0;
  while(count != 0)
  {
    size_t rel_offset = pos + total_count;
    ssize_t idx = find_file(rel_offset);
    if (idx < 0)
    {
      log_debug("zip entry not found: size={} pos={} count={}", m_size, pos, count);
      return -1;
    }

    unzGoToFilePos(m_fp, &m_entries[idx].pos);
    size_t data_left = m_entries[idx].uncompressed_size - rel_offset;

    unzOpenCurrentFile(m_fp);

    // "seek" to the offset via read() calls
    char tmpbuf[1024 * 16];
    while(rel_offset != 0)
    {
      if (rel_offset > sizeof(tmpbuf))
      {
        rel_offset -= unzReadCurrentFile(m_fp, tmpbuf, sizeof(tmpbuf));
      }
      else
      {
        rel_offset -= unzReadCurrentFile(m_fp, tmpbuf, static_cast<unsigned int>(rel_offset));
      }
    }

    // read the data
    ssize_t read_amount = std::min(data_left, count);
    ssize_t len = unzReadCurrentFile(m_fp, buf, static_cast<unsigned int>(read_amount));
    if (len == 0)
    {
      log_debug("{}: unexpected eof", m_filename);
      return -1;
    }
    else if (len < 0)
    {
      log_debug("{}: uncompression error", m_filename);
      return -1;
    }
    else if (len != read_amount)
    {
      log_debug("{}: unexpected short read", m_filename);
      return -1;
    }
    else
    {
      total_count += len;
      count -= len;
      buf += len;
    }
  }

  return total_count;
}

ssize_t
ZipFile::find_file(size_t& offset)
{
  for(size_t i = 0; i < m_entries.size(); ++i)
  {
    if (offset < m_entries[i].uncompressed_size)
    {
      return i;
    }
    else
    {
      offset -= m_entries[i].uncompressed_size;
    }
  }
  return -1;
}

size_t
ZipFile::get_size() const
{
  return m_size;
}

struct timespec
ZipFile::get_mtime() const
{
  return m_mtime;
}

int
ZipFile::getattr(const char* path, struct stat* stbuf)
{
  stbuf->st_mode = S_IFREG | 0444;
  stbuf->st_nlink = 2;
  stbuf->st_size = get_size();
  stbuf->st_mtim = get_mtime();
  return 0;
}

int
ZipFile::utimens(const char* path, const struct timespec tv[2])
{
  return 0;
}

int
ZipFile::open(const char* path, struct fuse_file_info* fi)
{
  return 0;
}

int
ZipFile::release(const char* path, struct fuse_file_info* fi)
{
  return 0;
}

int
ZipFile::read(const char* path, char* buf, size_t len, off_t offset,
              struct fuse_file_info* fi)
{
  return static_cast<int>(read(static_cast<size_t>(offset), buf, len));
}

/* EOF */
