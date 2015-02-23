// Copyright (c) 2015 Ingo Ruhnke <grumbel@gmail.com>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgement in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#define _GNU_SOURCE

#include <ctype.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef int (*orig_open_t)(const char* pathname, int flags);
typedef int (*orig_close_t)(int fd);
typedef off_t (*orig_lseek_t)(int fd, off_t offset, int whence);
typedef ssize_t (*orig_read_t)(int fd, void* buf, size_t count);

orig_open_t orig_open;
orig_lseek_t orig_lseek;
orig_read_t orig_read;
orig_close_t orig_close;

#define CONCAT_MAGIC_FD 0xdeadbeef

typedef struct
{
  const char* filename;
  size_t size;
  int fd;
} FileInfo;

FileInfo* file_info_lst;
int file_info_count;

size_t get_file_size(const char* filename)
{
  int fd = orig_open(filename, O_RDONLY);
  if (fd < 0)
  {
    perror(filename);
    return 0;
  }
  else
  {
    off_t ret = orig_lseek(fd, 0, SEEK_END);
    orig_close(fd);
    return ret;
  }
}

void collect_file_info()
{
  glob_t glob_data;
  char* pattern = getenv("CONCAT_PATTERN");
  if (!pattern)
  {
    fprintf(stderr, "concat.so: no concat pattern given\n");
  }
  else
  {
    int ret = glob(pattern, 0, NULL, &glob_data);
    if (ret == GLOB_NOMATCH)
    {
      fprintf(stderr, "concat.so: no matching files found for pattern: %s\n", pattern);
    }
    else
    {
      file_info_count = glob_data.gl_pathc;
      file_info_lst = malloc(sizeof(FileInfo) * file_info_count);

      for(int i = 0; i < glob_data.gl_pathc; ++i)
      {
        file_info_lst[i].filename = strdup(glob_data.gl_pathv[i]);
        file_info_lst[i].size = get_file_size(file_info_lst[i].filename);
        file_info_lst[i].fd = 0;

        fprintf(stderr, "glob match: %s %zu\n",
                file_info_lst[i].filename,
                file_info_lst[i].size);
      }
    }
    globfree(&glob_data);
  }
}

__attribute__ ((constructor))
void init()
{
  fprintf(stderr, "== init()\n");

  orig_open = (orig_open_t)dlsym(RTLD_NEXT, "open");
  orig_lseek = (orig_lseek_t)dlsym(RTLD_NEXT, "lseek");
  orig_read = (orig_read_t)dlsym(RTLD_NEXT, "read");
  orig_close = (orig_close_t)dlsym(RTLD_NEXT, "close");

  collect_file_info();
}

int open(const char* pathname, int flags, ...)
{
  int ret = orig_open(pathname, flags);
  fprintf(stderr, "== open(\"%s\", %d) -> %d\n", pathname, flags, ret);
  return ret;
}

off_t lseek(int fd, off_t offset, int whence)
{
  fprintf(stderr, "== lseek(%d, %d, %d)\n", fd, (int)offset, whence);

  return orig_lseek(fd, offset, whence);
}

ssize_t read(int fd, void* buf, size_t count)
{
  fprintf(stderr, "== read(%d, %p, %zu)\n", fd, buf, count);

  ssize_t ret = orig_read(fd, buf, count);
  if (ret > 0)
  {
    char* cbuf = buf;
    for(ssize_t i = 0; i < ret; ++i)
    {
      if (isalpha(cbuf[i]))
        {
          cbuf[i] = cbuf[i]+1;
        }
    }
  }
  return ret;
}

int close(int fd)
{
  fprintf(stderr, "== close(%d)\n", fd);
  return orig_close(fd);
}

/* EOF */
