# concat-fuse
# Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
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

set(TINYCMMC_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/external/tinycmmc/modules/")
find_package(tinycmmc CONFIG)
message(STATUS "tinycmmc module path: ${TINYCMMC_MODULE_PATH}")
list(APPEND CMAKE_MODULE_PATH ${TINYCMMC_MODULE_PATH})

include(MaximumWarnings)
include(ClangTidy)

find_package(Threads REQUIRED)
find_library(RT rt)

find_package(OpenSSL COMPONENTS Crypto REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(FUSE REQUIRED fuse IMPORTED_TARGET)
pkg_search_module(MINIZIP REQUIRED minizip IMPORTED_TARGET)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")

file(GLOB CONCAT_FUSE_SOURCES_CXX RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  src/*.cpp)
list(REMOVE_ITEM CONCAT_FUSE_SOURCES_CXX src/main.cpp)

add_library(libconcat-fuse STATIC ${CONCAT_FUSE_SOURCES_CXX})
set_target_properties(libconcat-fuse PROPERTIES OUTPUT_NAME concat-fuse)
target_compile_definitions(libconcat-fuse PUBLIC -D__STDC_CONSTANT_MACROS)
target_compile_options(libconcat-fuse PRIVATE  ${WARNINGS_CXX_FLAGS})
target_include_directories(libconcat-fuse PUBLIC src/)
target_link_libraries(libconcat-fuse PUBLIC
  PkgConfig::FUSE
  PkgConfig::MINIZIP
  OpenSSL::Crypto
  Threads::Threads)

add_executable(concat-fuse src/main.cpp)
target_link_libraries(concat-fuse libconcat-fuse)
target_compile_options(concat-fuse PRIVATE ${WARNINGS_CXX_FLAGS})

if(BUILD_TESTS)
  find_package(GTest REQUIRED)

  file(GLOB TEST_CONCAT_FUSE_SOURCES_CXX RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    tests/*_test.cpp)
  add_executable(test_concat_fuse ${TEST_CONCAT_FUSE_SOURCES_CXX})
  target_link_libraries(test_concat_fuse libconcat-fuse GTest::GTest GTest::Main)
  target_compile_options(test_concat_fuse PUBLIC ${WARNINGS_CXX_FLAGS})

  # add 'make test' target, use 'make test ARGS="-V"' or 'ctest -V' for verbose
  enable_testing()
  add_test(NAME test_concat_fuse
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND test_concat_fuse)
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
