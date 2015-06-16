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

find_package(Threads REQUIRED)
find_library(RT rt)

find_package(PkgConfig REQUIRED)
pkg_search_module(FUSE REQUIRED fuse)

# older versions don't have a mhash.pc
pkg_search_module(MHASH mhash)
if(NOT MHASH_FOUND)
  find_library(MHASH_LIBRARIES mhash)
  message(STATUS "Found mhash: ${MHASH_LIBRARIES}")
  if(MHASH_LIBRARIES-NOTFOUND)
    message(FATAL_ERROR "required library mhash not found")
  endif()
endif()

pkg_search_module(MINIZIP REQUIRED minizip)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG} -DNDEBUG)

if(WARNINGS)
  set(WARNING_CXX_FLAGS ${WARNING_CXX_FLAGS}
    -pedantic
    -Wall
    -Wextra
    -Wno-c++0x-compat
    -Wnon-virtual-dtor
    -Weffc++
    -Wconversion
    -Wshadow
    -Wcast-qual
    -Winit-self # only works with >= -O1
    -Wno-unused-parameter
    -Wfloat-equal
    -Wunreachable-code)
endif()

if(WERROR)
  set(WARNING_CXX_FLAGS ${WARNING_CXX_FLAGS}
    -Werror)
endif()

file(GLOB CONCAT_FUSE_SOURCES_CXX RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  src/*.cpp)
list(REMOVE_ITEM CONCAT_FUSE_SOURCES_CXX src/main.cpp)

add_library(libconcat-fuse STATIC ${CONCAT_FUSE_SOURCES_CXX})
set_target_properties(libconcat-fuse PROPERTIES OUTPUT_NAME concat-fuse)
target_compile_definitions(libconcat-fuse PUBLIC -D__STDC_CONSTANT_MACROS)
target_compile_options(libconcat-fuse PUBLIC ${FUSE_CFLAGS_OTHER} ${WARNING_CXX_FLAGS})
target_link_libraries(libconcat-fuse PUBLIC ${MINIZIP_LIBRARIES})
target_include_directories(libconcat-fuse PUBLIC ${MINIZIP_INCLUDE_DIRS})

add_executable(concat-fuse src/main.cpp)
target_include_directories(concat-fuse SYSTEM PUBLIC ${FUSE_INCLUDE_DIRS} ${MHASH_INCLUDE_DIRS})
target_link_libraries(concat-fuse libconcat-fuse ${FUSE_LIBRARIES} ${MHASH_LIBRARIES})
target_compile_options(concat-fuse PUBLIC ${FUSE_CFLAGS_OTHER} ${WARNING_CXX_FLAGS})

if(BUILD_TESTS)
  # Note: include directories must use the absolute path as otherwise SYSTEM
  # bugs out and uses -I instead of -isystem
  add_library(gtest STATIC ${CMAKE_CURRENT_SOURCE_DIR}/external/googletest/src/gtest-all.cc)
  target_include_directories(gtest SYSTEM PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/external/googletest/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/external/googletest/)

  add_library(gtest_main STATIC ${CMAKE_CURRENT_SOURCE_DIR}/external/googletest/src/gtest_main.cc)
  target_include_directories(gtest_main SYSTEM PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/external/googletest/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/external/googletest/)

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
  RENAME cfconcat
  PERMISSIONS OWNER_EXECUTE OWNER_READ OWNER_WRITE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  DESTINATION "bin/")
install(FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/doc/cfconcat.1
  ${CMAKE_CURRENT_SOURCE_DIR}/doc/concat-fuse.1
  DESTINATION "share/man/man1/")

# EOF #
