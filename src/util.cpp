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

#include "util.hpp"

#include <array>
#include <fcntl.h>
#include <iomanip>
#include <mhash.h>
#include <sstream>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

bool is_hex(char c)
{
  return (('0' <= c && c <= '9') ||
          ('a' <= c && c <= 'f') ||
          ('A' <= c && c <= 'F'));
}

int hex2int(char c)
{
  if ('0' <= c && c <= '9')
  {
    return c - '0';
  }
  else if ('a' <= c && c <= 'f')
  {
    return c - 'a' + 10;
  }
  else if ('A' <= c && c <= 'F')
  {
    return c - 'A' + 10;
  }
  else
  {
    return 0;
  }
}

std::string url_quote(const std::string& url)
{
  std::ostringstream str;
  str.fill('0');
  str << std::hex;

  for(std::string::size_type i = 0; i < url.size(); ++i)
  {
    if (isalpha(url[i]))
    {
      str << url[i];
    }
    else
    {
      str << "%" << static_cast<int>(url[i]);
    }
  }

  return str.str();
}

std::string url_unquote(const std::string& url)
{
  std::ostringstream str;

  for(std::string::size_type i = 0; i < url.size(); ++i)
  {
    if (url[i] == '%')
    {
      if (i+2 < url.size() && is_hex(url[i+1]) && is_hex(url[i+2]))
      {
        char c = static_cast<char>((hex2int(url[i+1]) << 4) | hex2int(url[i+2]));
        str << c;
        i += 2;
      }
      else
      {
        // failed to decode escape, so leave it untouched
        str << url[i];
      }
    }
    else
    {
      str << url[i];
    }
  }

  return str.str();
}

bool has_prefix(const char* text, const char* prefix)
{
  size_t prefix_len = strlen(prefix);
  size_t text_len = strlen(text);
  if (text_len < prefix_len)
  {
    return false;
  }
  else
  {
    return strncmp(prefix, text, prefix_len) == 0;
  }
}

std::string sha1sum(const std::string& data)
{
  return sha1sum(data.data(), data.size());
}

std::string sha1sum(const char* data, size_t len)
{
  MHASH td = mhash_init(MHASH_SHA1);
  if (td == MHASH_FAILED)
  {
    return std::string();
  }
  else
  {
    mhash(td, data, static_cast<mutils_word32>(len));
    std::array<uint8_t, 20> digest;
    mhash_deinit(td, digest.data());

    std::ostringstream out;
    for (mutils_word32 i = 0; i < digest.size(); ++i)
      out << std::setfill('0') << std::setw(2) << std::hex << int(digest[i]);
    return out.str();
  }
}

std::vector<std::string> split(const std::string& str, char c)
{
  std::vector<std::string> result;

  size_t start = 0;
  size_t i = 0;
  for(i = 0; i < str.size(); ++i)
  {
    if (str[i] == c)
    {
      result.push_back(str.substr(start, i - start));
      start = i + 1;
    }
  }

  if (start != str.size())
  {
    result.push_back(str.substr(start, str.size() - start));
  }

  return result;
}

std::string path_join(const std::string& base, const std::string& name)
{
  if (base.empty())
  {
    log_debug("Shouldn't happen");
    return name;
  }
  else if (base[base.size() - 1] == '/')
  {
    return base + name;
  }
  else
  {
    return base + "/" + name;
  }
}

size_t get_file_size(const std::string& filename)
{
  struct stat buf;
  int ret = stat(filename.c_str(), &buf);
  if (ret < 0)
  {
    log_debug("{}: stat() failed: ", filename, strerror(errno));
    return 0;
  }
  else
  {
    return static_cast<size_t>(buf.st_size);
  }
}

/* EOF */
