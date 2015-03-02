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
    "-Wfloat-equal",
    "-Wunreachable-code",
]


def options(opt):
    opt.load("compiler_cxx")
    opt.load('gnu_dirs')

    gr = opt.add_option_group('concat-fuse options')
    gr.add_option('--build-tests', action='store_true', default=False, help='Build tests')
    gr.add_option('--developer', action='store_true', default=False, help='Switch on extra debug info, warnings and verbosity')


def configure(conf):
    conf.load("compiler_cxx")
    conf.load('gnu_dirs')

    conf.check_cfg(package='fuse', args=['--cflags', '--libs'])
    if not conf.check_cfg(package='mhash', args=['--cflags', '--libs'], mandatory=False):
        conf.check(lib="mhash")

    conf.env.append_value('CXXFLAGS', ["-std=c++11", "-O0", "-g"])

    if conf.options.developer:
        conf.env.append_value('CXXFLAGS_WARNINGS', developer_cxxflags)
    else:
        conf.env.append_value('DEFINES_WARNINGS', ["NDEBUG"])

    conf.env.build_tests = conf.options.build_tests


def build(bld):
    bld.install_as('${PREFIX}/bin/vcat', ['vcat.py'], chmod=0755)
    bld.install_files('${MANDIR}/man1', ["doc/vcat.1", "doc/concat-fuse.1"])

    bld.stlib(target="concat_fuse",
              source=["src/concat_fuse.cpp",
                      "src/concat_vfs.cpp",
                      "src/control_file.cpp",
                      "src/directory.cpp",
                      "src/glob_file_list.cpp",
                      "src/multi_file.cpp",
                      "src/simple_directory.cpp",
                      "src/simple_file.cpp",
                      "src/simple_file_list.cpp",
                      "src/util.cpp"],
              use=["WARNINGS", "FUSE", "MHASH"])

    bld.program(target="concat-fuse",
                source=["src/main.cpp"],
                use=["WARNINGS", "concat_fuse", "FUSE", "MHASH"])

    if bld.env.build_tests:
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
                    use=(["gtest", "gtest_main", "concat_fuse", "MHASH"]),
                    install_path=None)


# EOF #
