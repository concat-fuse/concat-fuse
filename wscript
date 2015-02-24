# concat-fuse
# Copyright (C) 2015 Ingo Ruhnke <grumbel@gmx.de>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


from glob import glob


developer_cxxflags = [
    # "-ansi",
    "-pedantic",
    "-Wall",
    "-Wextra",
    "-Wno-c++0x-compat",
    "-Wnon-virtual-dtor",
    "-Weffc++",
    "-Wconversion",
    "-Werror",
    "-Wshadow",
    "-Wcast-qual",
    "-Winit-self", # only works with >= -O1
    "-Wno-unused-parameter",
    # "-Winline",
    # "-Wfloat-equal",
    # "-Wunreachable-code",
]


def options(opt):
    opt.load("compiler_cxx")


def configure(conf):
    conf.load("compiler_cxx")
    conf.check_cfg(package='fuse', args=['--cflags', '--libs'])

    conf.env.append_value('CXXFLAGS', ["-std=c++14", "-O2", "-g"])
    conf.env.append_value('CXXFLAGS_WARNINGS', developer_cxxflags)


def build(bld):
    bld.install_files('${PREFIX}/bin', ['vcat'])

    bld.stlib(target="concat_fuse",
              source=["src/concat_fuse.cpp",
                      "src/multi_file.cpp",
                      "src/util.cpp"],
              use=["WARNINGS", "FUSE"])

    bld.program(target="concat-fuse",
                source=["src/main.cpp"],
                use=["WARNINGS", "concat_fuse", "FUSE"])

    bld.program(target="concat-pattern",
                source=["src/concat_pattern.cpp"],
                use=["WARNINGS", "concat_fuse"])

    # build gtest
    bld.stlib(target="gtest",
              source=["external/gtest-1.7.0/src/gtest-all.cc"],
              includes=["external/gtest-1.7.0/include/",
                        "external/gtest-1.7.0/"])

    bld.stlib(target="gtest_main",
              source=["external/gtest-1.7.0/src/gtest_main.cc"],
              includes=["external/gtest-1.7.0/include/",
                        "external/gtest-1.7.0/"])
    
    bld.program(target="test_concat_fuse",
                source=glob("tests/*_test.cpp"),
                includes=["src/"],
                cxxflags=["-isystem", bld.path.find_dir("external/gtest-1.7.0/include/").abspath()],
                use=(["gtest", "gtest_main", "concat_fuse"]),
                install_path=None)


# EOF #
