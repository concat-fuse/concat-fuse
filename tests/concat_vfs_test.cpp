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

#include <vector>
#include <string>
#include <memory>

#include "simple_directory.hpp"
#include "simple_file.hpp"
#include "concat_vfs.hpp"
#include "util.hpp"

TEST(ConcatVFSTest, entry_cache)
{
  auto vfs = make_unique<ConcatVFS>();
  SimpleDirectory& root = vfs->get_root();
  {
    auto from_file_dir = make_unique<SimpleDirectory>();
    from_file_dir->add_file("control", make_unique<SimpleFile>("SimpleFile1"));
    root.add_directory("from-file0", std::move(from_file_dir));
  }
  {
    auto dir = make_unique<SimpleDirectory>();
    dir->add_file("control", make_unique<SimpleFile>("SimpleFile2"));
    root.add_directory("from-glob0", std::move(dir));
  }
  vfs->rebuild_entry_cache();

  EXPECT_TRUE(vfs->lookup("/") != nullptr);
  EXPECT_TRUE(vfs->lookup("/from-glob0") != nullptr);
  EXPECT_TRUE(vfs->lookup("/from-file0") != nullptr);
  EXPECT_TRUE(vfs->lookup("/from-glob0/control") != nullptr);
  EXPECT_TRUE(vfs->lookup("/from-file0/control") != nullptr);
  EXPECT_TRUE(vfs->lookup("/doesnotexist/control") == nullptr);
  EXPECT_TRUE(vfs->lookup("/from-glob0/") == nullptr);
}

/* EOF */
