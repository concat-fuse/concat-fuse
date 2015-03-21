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

#include <array>
#include <vector>
#include <string>
#include <memory>
#include <string.h>
#include <fuse.h>
#include <iostream>

#include "simple_file_list.hpp"
#include "multi_file.hpp"
#include "util.hpp"

TEST(MultiFileTest, test)
{
  auto file_list = make_unique<SimpleFileList>(std::vector<std::string>{ "tests/test1.dat", "tests/test2.dat" });
  auto multi_file = make_unique<MultiFile>(std::move(file_list));

  struct fuse_file_info fi;
  memset(&fi, 0, sizeof(fi));
  std::array<char, 16> buf;
  std::array<char, 16> expected_result{'F', 'o', 'o', 'b', 'a', 'r', '\n', 'd', 'e', 'a', 'd', 'b', 'e', 'e', 'f', '\n'};
  int ret = multi_file->read("dummy_path", buf.data(), buf.size(), 0, &fi);

  // size of the test1.dat and test2.dat combined
  EXPECT_EQ(16, ret);
  EXPECT_EQ(expected_result, buf);
}

/* EOF */
