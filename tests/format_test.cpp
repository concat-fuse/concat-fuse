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

#include <gtest/gtest.h>

#include <string>

#include "format.hpp"

namespace {

template<typename... Args>
std::string format_str(Args&&... args)
{
  std::ostringstream os;
  format(os, args...);
  return os.str();
}

} // namespace

TEST(FormatTest, format)
{
  ASSERT_EQ(format_str("{}", 5), "5");
  ASSERT_EQ(format_str("{} {} {}", 5, 6, 7), "5 6 7");
  ASSERT_EQ(format_str("{{ }}"), "{ }");
  ASSERT_EQ(format_str("{{ {} }}", 9123), "{ 9123 }");
  ASSERT_EQ(format_str("{{"), "{");
  ASSERT_EQ(format_str("}}"), "}");

  ASSERT_THROW(format_str("{"), std::invalid_argument);
  ASSERT_THROW(format_str("}"), std::invalid_argument);
  ASSERT_THROW(format_str("{foobar}"), std::invalid_argument);
}

/* EOF */

