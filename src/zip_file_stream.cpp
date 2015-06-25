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

#include "zip_file_stream.hpp"

#include <algorithm>

#include "util.hpp"

std::unique_ptr<ZipFileStream>
ZipFileStream::open(const std::string& filename)
{
  unzFile fp = unzOpen(filename.c_str());
  if (!fp)
  {
    throw std::runtime_error("failed to open " + filename);
  }
  else
  {
    auto result = std::unique_ptr<ZipFileStream>(new ZipFileStream(fp));

    result->read_index();

    // entries in the .zip might not be ordered, so sort them
    result->sort_index();

    return result;
  }
}

void
ZipFileStream::read_index()
{
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

  log_debug("{}: size={} entries={}", m_filename, m_size, m_entries.size());
}

void
ZipFileStream::sort_index()
{
  std::sort(m_entries.begin(), m_entries.end(),
            [](ZipEntry const& lhs, ZipEntry const& rhs){
              return lhs.filename < rhs.filename;
            });
}

ZipFileStream::ZipFileStream(unzFile fp) :
  m_fp(fp),
  m_size(),
  m_filename(),
  m_entries(),
  m_mutex()
{
}

ZipFileStream::~ZipFileStream()
{
  unzClose(m_fp);
}

size_t
ZipFileStream::get_size() const
{
  return m_size;
}

ssize_t
ZipFileStream::read(size_t pos, char* buf, size_t count)
{
  std::lock_guard<std::mutex> lock(m_mutex);

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
    ssize_t idx_or_error = find_file(rel_offset);
    if (idx_or_error < 0)
    {
      log_debug("zip entry not found: size={} pos={} count={}", m_size, pos, count);
      return -1;
    }

    size_t idx = static_cast<size_t>(idx_or_error);

    unzGoToFilePos(m_fp, &m_entries[idx].pos);
    size_t data_left = m_entries[idx].uncompressed_size - rel_offset;

    unzOpenCurrentFile(m_fp);

    // "seek" to the offset via read() calls
    char tmpbuf[1024 * 16];
    while(rel_offset != 0)
    {
      unsigned read_length = static_cast<unsigned>((rel_offset > sizeof(tmpbuf)) ? sizeof(tmpbuf) : rel_offset);
      int len_or_error = unzReadCurrentFile(m_fp, tmpbuf, read_length);
      if (len_or_error == 0)
      {
        log_debug("{}: unexpected eof", m_filename);
        return -1;
      }
      else if (len_or_error < 0)
      {
        log_debug("{}: uncompression error", m_filename);
        return -1;
      }
      else
      {
        rel_offset -= static_cast<size_t>(len_or_error);
      }
    }

    // read the data
    ssize_t read_amount = static_cast<ssize_t>(std::min(data_left, count));
    ssize_t len_or_error = unzReadCurrentFile(m_fp, buf, static_cast<unsigned int>(read_amount));
    if (len_or_error == 0)
    {
      log_debug("{}: unexpected eof", m_filename);
      return -1;
    }
    else if (len_or_error < 0)
    {
      log_debug("{}: uncompression error", m_filename);
      return -1;
    }
    else if (len_or_error != read_amount)
    {
      log_debug("{}: unexpected short read", m_filename);
      return -1;
    }
    else
    {
      size_t len = static_cast<size_t>(len_or_error);
      total_count += len;
      count -= len;
      buf += len;
    }
  }

  return static_cast<ssize_t>(total_count);
}

ssize_t
ZipFileStream::find_file(size_t& offset) const
{
  for(size_t i = 0; i < m_entries.size(); ++i)
  {
    if (offset < m_entries[i].uncompressed_size)
    {
      return static_cast<ssize_t>(i);
    }
    else
    {
      offset -= m_entries[i].uncompressed_size;
    }
  }
  return -1;
}

/* EOF */
