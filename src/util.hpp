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

#ifndef HEADER_UTIL_HPP
#define HEADER_UTIL_HPP

#include <string>
#include <vector>
#include <memory>

#include "format.hpp"

#ifdef NDEBUG
#  define log_debug(...)
#else
#  define log_debug(...) do{ format(std::cerr, "[DEBUG] " __FILE__ ": " __VA_ARGS__); std::endl(std::cerr); } while(0)
#endif

bool is_hex(char c);
int hex2int(char c);

std::string url_quote(const std::string& url);
std::string url_unquote(const std::string& url);

bool has_prefix(const char* text, const char* prefix);

std::string sha1sum(const std::string& data);
std::string sha1sum(const char* data, size_t len);

std::vector<std::string> split(const std::string& str, char c);

size_t get_file_size(const std::string& filename);

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

#endif

/* EOF */
