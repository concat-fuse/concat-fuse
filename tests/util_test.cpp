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

#include "util.hpp"

TEST(UtilTest, is_hex)
{
  std::string validhex = "0123456789abcdefABCDEF";
  for(size_t i = 0; i < validhex.size(); ++i)
  {
    ASSERT_TRUE(is_hex(validhex[i]));
  }

  std::string invalidhex = "xyzXZY-+!@#";
  for(size_t i = 0; i < invalidhex.size(); ++i)
  {
    ASSERT_FALSE(is_hex(invalidhex[i]));
  }
}

TEST(UtilTest, hex2int)
{
  ASSERT_EQ(hex2int('0'), 0);
  ASSERT_EQ(hex2int('1'), 1);
  ASSERT_EQ(hex2int('2'), 2);
  ASSERT_EQ(hex2int('3'), 3);
  ASSERT_EQ(hex2int('a'), 10);
  ASSERT_EQ(hex2int('b'), 11);
  ASSERT_EQ(hex2int('c'), 12);
  ASSERT_EQ(hex2int('d'), 13);
  ASSERT_EQ(hex2int('e'), 14);
  ASSERT_EQ(hex2int('f'), 15);
  ASSERT_EQ(hex2int('A'), 10);
  ASSERT_EQ(hex2int('B'), 11);
  ASSERT_EQ(hex2int('C'), 12);
  ASSERT_EQ(hex2int('D'), 13);
  ASSERT_EQ(hex2int('E'), 14);
  ASSERT_EQ(hex2int('F'), 15);
}

TEST(UtilTest, url_quote)
{
  ASSERT_EQ(url_quote("hello/world"), "hello%2fworld");
  ASSERT_EQ(url_quote("hello world"), "hello%20world");
}

TEST(UtilTest, url_unquote)
{
  ASSERT_EQ(url_unquote("hello%2fworld"), "hello/world");
  ASSERT_EQ(url_unquote("hello%2Fworld"), "hello/world");
}

TEST(UtilTest, has_prefix)
{
  ASSERT_TRUE(has_prefix("foobar", "foo"));
  ASSERT_TRUE(has_prefix("foo", "foo"));
  ASSERT_FALSE(has_prefix("barfoo", "foo"));
  ASSERT_FALSE(has_prefix("foo", "foobar"));
}

TEST(UtilTest, sha1sum)
{
  ASSERT_EQ(sha1sum("", 0), "da39a3ee5e6b4b0d3255bfef95601890afd80709");
  ASSERT_EQ(sha1sum("Hello World", 11), "0a4d55a8d778e5022fab701977c5d840bbc486d0");
}

/* EOF */
