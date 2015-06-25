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

#include <gtest/gtest.h>

#include <vector>
#include <string>

#include "util.hpp"

TEST(UtilTest, is_hex)
{
  std::string validhex = "0123456789abcdefABCDEF";
  for(size_t i = 0; i < validhex.size(); ++i)
  {
    EXPECT_TRUE(is_hex(validhex[i]));
  }

  std::string invalidhex = "xyzXZY-+!@#";
  for(size_t i = 0; i < invalidhex.size(); ++i)
  {
    EXPECT_FALSE(is_hex(invalidhex[i]));
  }
}

TEST(UtilTest, hex2int)
{
  EXPECT_EQ(0, hex2int('0'));
  EXPECT_EQ(1, hex2int('1'));
  EXPECT_EQ(2, hex2int('2'));
  EXPECT_EQ(3, hex2int('3'));
  EXPECT_EQ(10, hex2int('a'));
  EXPECT_EQ(11, hex2int('b'));
  EXPECT_EQ(12, hex2int('c'));
  EXPECT_EQ(13, hex2int('d'));
  EXPECT_EQ(14, hex2int('e'));
  EXPECT_EQ(15, hex2int('f'));
  EXPECT_EQ(10, hex2int('A'));
  EXPECT_EQ(11, hex2int('B'));
  EXPECT_EQ(12, hex2int('C'));
  EXPECT_EQ(13, hex2int('D'));
  EXPECT_EQ(14, hex2int('E'));
  EXPECT_EQ(15, hex2int('F'));
}

TEST(UtilTest, url_quote)
{
  EXPECT_EQ("hello%2fworld", url_quote("hello/world"));
  EXPECT_EQ("hello%20world", url_quote("hello world"));
}

TEST(UtilTest, url_unquote)
{
  EXPECT_EQ("hello/world", url_unquote("hello%2fworld"));
  EXPECT_EQ("hello/world", url_unquote("hello%2Fworld"));
}

TEST(UtilTest, has_prefix)
{
  EXPECT_TRUE(has_prefix("foobar", "foo"));
  EXPECT_TRUE(has_prefix("foo", "foo"));
  EXPECT_FALSE(has_prefix("barfoo", "foo"));
  EXPECT_FALSE(has_prefix("foo", "foobar"));
}

TEST(UtilTest, sha1sum)
{
  EXPECT_EQ("da39a3ee5e6b4b0d3255bfef95601890afd80709", sha1sum("", 0));
  EXPECT_EQ("0a4d55a8d778e5022fab701977c5d840bbc486d0", sha1sum("Hello World", 11));
}

TEST(UtilTest, split)
{
  std::vector<std::string> multi_result{"Hello", "World", "Foobar"};
  std::vector<std::string> one_result{"One"};
  std::vector<std::string> not_empty_result{" "};
  std::vector<std::string> empty_result;

  EXPECT_NE(multi_result, split("Hello\nWorld", '\n'));
  EXPECT_EQ(multi_result, split("Hello\nWorld\nFoobar\n", '\n'));
  EXPECT_EQ(multi_result, split("Hello\nWorld\nFoobar", '\n'));
  EXPECT_EQ(multi_result, split(std::string("Hello\0World\0Foobar", 18), '\0'));
  EXPECT_EQ(empty_result, split("", '\n'));
  EXPECT_EQ(not_empty_result, split(" ", '\n'));
  EXPECT_EQ(one_result, split("One", '\n'));
  EXPECT_EQ(one_result, split("One\n", '\n'));
}

TEST(UtilTest, path_join)
{
  EXPECT_EQ("/name", path_join("/", "name"));
  EXPECT_EQ("/foo/bar", path_join("/foo", "bar"));
  EXPECT_EQ("/foo/bar", path_join("/foo/", "bar"));
}

/* EOF */
