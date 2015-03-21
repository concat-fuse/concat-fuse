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


cmake_minimum_required(VERSION 3.0)
project(concat-fuse)

include(FindPkgConfig)

find_package(Threads)
pkg_search_module(FUSE REQUIRED fuse)
pkg_search_module(MHASH REQUIRED mhash)
find_library(RT rt)

list(APPEND CMAKE_CXX_FLAGS -std=c++11)

if(DEVELOPER)
  set(WARNING_CXX_FLAGS
    -pedantic
    -Wall
    -Wextra
    -Wno-c++0x-compat
    -Wnon-virtual-dtor
    -Weffc++
    -Wconversion
    -Werror
    -Wshadow
    -Wcast-qual
    -Winit-self # only works with >= -O1
    -Wno-unused-parameter
    -Wfloat-equal
    -Wunreachable-code)
else(DEVELOPER)
  set(WARNING_CXX_FLAGS -DNDEBUG)
endif(DEVELOPER)

file(GLOB CONCAT_FUSE_SOURCES_CXX RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  src/concat_fuse.cpp
  src/concat_vfs.cpp
  src/control_file.cpp
  src/directory.cpp
  src/glob_file_list.cpp
  src/multi_file.cpp
  src/simple_directory.cpp
  src/simple_file.cpp
  src/simple_file_list.cpp
  src/util.cpp)

add_library(libconcat-fuse STATIC ${CONCAT_FUSE_SOURCES_CXX})
set_target_properties(libconcat-fuse PROPERTIES OUTPUT_NAME concat-fuse)
target_compile_definitions(libconcat-fuse PUBLIC -D__STDC_CONSTANT_MACROS)
target_compile_options(libconcat-fuse PUBLIC ${FUSE_CFLAGS_OTHER} ${WARNING_CXX_FLAGS})

add_executable(concat-fuse src/main.cpp)
target_include_directories(concat-fuse SYSTEM PUBLIC ${FUSE_INCLUDE_DIRS} ${MHASH_INCLUDE_DIRS})
target_link_libraries(concat-fuse libconcat-fuse ${FUSE_LIBRARIES} ${MHASH_LIBRARIES})
target_compile_options(concat-fuse PUBLIC ${FUSE_CFLAGS_OTHER} ${WARNING_CXX_FLAGS})

if(BUILD_TESTS)
  add_library(gtest STATIC ${CMAKE_CURRENT_SOURCE_DIR}/external/gtest-1.7.0/src/gtest-all.cc)
  target_include_directories(gtest SYSTEM PRIVATE
    external/gtest-1.7.0/include/
    external/gtest-1.7.0/)

  add_library(gtest_main STATIC ${CMAKE_CURRENT_SOURCE_DIR}/external/gtest-1.7.0/src/gtest_main.cc)
  target_include_directories(gtest_main SYSTEM PRIVATE
    external/gtest-1.7.0/include/
    external/gtest-1.7.0/)

  file(GLOB TEST_CONCAT_FUSE_SOURCES_CXX RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    tests/*_test.cpp)
  add_executable(test_concat_fuse ${TEST_CONCAT_FUSE_SOURCES_CXX})
  target_include_directories(test_concat_fuse PUBLIC src/ test/)
  target_link_libraries(test_concat_fuse libconcat-fuse gtest gtest_main)
  target_compile_options(test_concat_fuse PUBLIC ${FUSE_CFLAGS_OTHER} ${WARNING_CXX_FLAGS})
  target_include_directories(test_concat_fuse SYSTEM PUBLIC ${FUSE_INCLUDE_DIRS} ${MHASH_INCLUDE_DIRS})
  target_link_libraries(test_concat_fuse libconcat-fuse ${FUSE_LIBRARIES} ${MHASH_LIBRARIES})
  target_link_libraries(test_concat_fuse ${CMAKE_THREAD_LIBS_INIT})
endif(BUILD_TESTS)

install(TARGETS concat-fuse
  RUNTIME DESTINATION bin)
install(FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/cfconcat.py
  PERMISSIONS OWNER_EXECUTE OWNER_READ OWNER_WRITE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  DESTINATION "bin/")
install(FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/doc/cfconcat.1
  ${CMAKE_CURRENT_SOURCE_DIR}/doc/concat-fuse.1
  DESTINATION "share/man/1/")

# EOF #
