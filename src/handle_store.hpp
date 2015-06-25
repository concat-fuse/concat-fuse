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

#ifndef HEADER_HANDLE_STORE_HPP
#define HEADER_HANDLE_STORE_HPP

#include <stdint.h>
#include <unordered_map>

template<typename T>
class HandleStore
{
public:
  typedef typename std::unordered_map<uint64_t, T>::iterator iterator;
  typedef typename std::unordered_map<uint64_t, T>::const_iterator const_iterator;

private:
  std::unordered_map<uint64_t, T> m_items;
  uint64_t m_id;

public:
  HandleStore() :
    m_items(),
    m_id(1)
  {}

  ~HandleStore()
  {}

  uint64_t store(T&& v)
  {
    m_items[m_id] = std::move(v);
    return m_id++;
  }

  uint64_t store(T const& v)
  {
    m_items[m_id] = v;
    return m_id++;
  }

  T& get(uint64_t id)
  {
    return m_items[id];
  }

  T drop(uint64_t id)
  {
    auto it = m_items.find(id);
    T tmp(std::move(it->second));
    m_items.erase(it);
    return tmp;
  }

  iterator begin() { return m_items.begin(); }
  iterator end() { return m_items.end(); }
  const_iterator begin() const { return m_items.begin(); }
  const_iterator end() const { return m_items.end(); }
};

#endif

/* EOF */
