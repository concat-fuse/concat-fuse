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

#include <assert.h>
#include <ctype.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <glob.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef int (*orig_open_t)(const char* pathname, int flags);
typedef int (*orig_close_t)(int fd);
typedef int (*orig_fxstat_t)(int ver, int fd, struct stat *buf);
typedef off_t (*orig_lseek_t)(int fd, off_t offset, int whence);
typedef ssize_t (*orig_read_t)(int fd, void* buf, size_t count);

orig_open_t orig_open;
orig_close_t orig_close;
orig_fxstat_t orig_fxstat;
orig_lseek_t orig_lseek;
orig_read_t orig_read;

typedef FILE* (*orig_fopen_t)(const char* pathname, const char* mode);
typedef int (*orig_fflush_t)(FILE* fd);
typedef int (*orig_fclose_t)(FILE* fd);
typedef size_t (*orig_fread_t)(void *ptr, size_t size, size_t nmemb, FILE *stream);
typedef size_t (*orig_fwrite_t)(const void *ptr, size_t size, size_t nmemb, FILE *stream);

orig_fopen_t orig_fopen;
orig_fflush_t orig_fflush;
orig_fclose_t orig_fclose;
orig_fread_t orig_fread;
orig_fwrite_t orig_fwrite;

#define CONCAT_MAGIC_FD INT_MAX

#define log_debug(...) fprintf(stderr, "[DEBUG] " __VA_ARGS__)
//#define log_debug(...)

typedef struct
{
  const char* filename;
  size_t size;
  int fd;
} FileInfo;

FileInfo* file_info_lst;
int file_info_count;

size_t magicfile_pos;
size_t magicfile_size;

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
    log_debug("concat.so: no concat pattern given\n");
  }
  else
  {
    int ret = glob(pattern, 0, NULL, &glob_data);
    if (ret == GLOB_NOMATCH)
    {
      log_debug("concat.so: no matching files found for pattern: %s\n", pattern);
    }
    else
    {
      file_info_count = glob_data.gl_pathc;
      file_info_lst = malloc(sizeof(FileInfo) * file_info_count);

      magicfile_size = 0;
      for(int i = 0; i < glob_data.gl_pathc; ++i)
      {
        file_info_lst[i].filename = strdup(glob_data.gl_pathv[i]);
        file_info_lst[i].size = get_file_size(file_info_lst[i].filename);
        file_info_lst[i].fd = 0;

        log_debug("glob match: %s %zu\n",
                file_info_lst[i].filename,
                file_info_lst[i].size);

        magicfile_size += file_info_lst[i].size;
      }
    }
    globfree(&glob_data);
  }
}

__attribute__ ((constructor))
void init()
{
  log_debug("init()\n");

  orig_open = (orig_open_t)dlsym(RTLD_NEXT, "open");
  orig_lseek = (orig_lseek_t)dlsym(RTLD_NEXT, "lseek");
  orig_read = (orig_read_t)dlsym(RTLD_NEXT, "read");
  orig_close = (orig_close_t)dlsym(RTLD_NEXT, "close");
  orig_fxstat = (orig_fxstat_t)dlsym(RTLD_NEXT, "__fxstat");

  orig_fopen = (orig_fopen_t)dlsym(RTLD_NEXT, "fopen");
  orig_fclose = (orig_fclose_t)dlsym(RTLD_NEXT, "fclose");
  orig_fread = (orig_fread_t)dlsym(RTLD_NEXT, "fread");
  orig_fwrite = (orig_fwrite_t)dlsym(RTLD_NEXT, "fwrite");
  orig_fflush = (orig_fflush_t)dlsym(RTLD_NEXT, "fflush");

  collect_file_info();
}

int magicfile_find_file(size_t* offset)
{
  for(int i = 0; i < file_info_count; ++i)
  {
    if (*offset >= file_info_lst[i].size)
    {
      *offset -= file_info_lst[i].size;
    }
    else
    {
      return i;
    }
  }
  return -1;
}

void read_subfile(const char* filename, size_t offset, void* buf, size_t count)
{
  // FIXME: insert error handling here
  int fd = orig_open(filename, O_RDONLY);
  if (fd < 0)
  {
    perror(filename);
  }
  else
  {
    orig_lseek(fd, offset, SEEK_SET);
    orig_read(fd, buf, count);
    orig_close(fd);
  }
}

ssize_t magicfile_read(size_t pos, void* buf, size_t count)
{
  log_debug("magicfile_read(%zu, %p, %zu)\n", pos, buf, count);

  size_t total_count = 0;
  while(count != 0)
  {
    size_t offset = pos;
    int idx = magicfile_find_file(&offset);
    log_debug("found file: %zu %d %zu %zu\n", pos, idx, offset, count);
    if (idx < 0)
    {
      log_debug("EOF reached\n");
      return total_count;
    }
    else
    {
      size_t read_count = count;
      if (file_info_lst[idx].size - offset < count)
      {
        read_count = file_info_lst[idx].size - offset;
      }
      read_subfile(file_info_lst[idx].filename,
                   offset, buf, read_count);
      pos += read_count;
      buf += read_count;
      count -= read_count;
      total_count += read_count;
    }
  }
  log_debug("count = %zu\n", count);
  return total_count;
}

int open(const char* pathname, int flags, ...)
{
  if (strcmp(pathname, "CONCAT_MAGICFILE") == 0 ||
      strcmp(pathname, "/CONCAT_MAGICFILE") == 0)
  {
    magicfile_pos = 0;
    log_debug("magicopen(\"%s\", %d) -> %d\n", pathname, flags, 0);
    return CONCAT_MAGIC_FD;
  }
  else
  {
    int ret = orig_open(pathname, flags);
    log_debug("open(\"%s\", %d) -> %d\n", pathname, flags, ret);
    return ret;
  }
}

off_t lseek(int fd, off_t offset, int whence)
{
  log_debug("lseek(%d, %d, %d)\n", fd, (int)offset, whence);

  if (fd == CONCAT_MAGIC_FD)
  {
    switch(whence)
    {
      case SEEK_SET:
        magicfile_pos = offset;
        break;

      case SEEK_CUR:
        magicfile_pos += offset;
        break;

      case SEEK_END:
        magicfile_pos = magicfile_size + offset;
        break;
    }

    return magicfile_pos;
  }
  else
  {
    return orig_lseek(fd, offset, whence);
  }
}

ssize_t read(int fd, void* buf, size_t count)
{
  log_debug("read(%d, %p, %zu)\n", fd, buf, count);

  if (fd == CONCAT_MAGIC_FD)
  {
    ssize_t ret = magicfile_read(magicfile_pos, buf, count);
    if (ret < 0)
    {
      log_debug("error in read()");
      return ret;
    }
    else
    {
      magicfile_pos += ret;
      return ret;
    }
  }
  else
  {
    return orig_read(fd, buf, count);
  }
}

int __fxstat(int version, int fd, struct stat* buf)
{
  log_debug("fstat(%d, %p)\n", fd, buf);
  if (fd == CONCAT_MAGIC_FD)
  {
    memset(buf, 0, sizeof(*buf));
    buf->st_size = magicfile_size;
    return 0;
  }
  else
  {
    return orig_fxstat(version, fd, buf);
  }
}

int close(int fd)
{
  log_debug("close(%d)\n", fd);

  if (fd == CONCAT_MAGIC_FD)
  {
    magicfile_pos = 0;
    return 0;
  }
  else
  {
    return orig_close(fd);
  }
}

FILE* concat_magicfile_stream = NULL;

FILE* fopen(const char* pathname, const char* mode)
{
  if (strcmp(pathname, "CONCAT_MAGICFILE") == 0 ||
      strcmp(pathname, "/CONCAT_MAGICFILE") == 0)
  {
    assert(!concat_magicfile_stream);
    concat_magicfile_stream = calloc(1, sizeof(FILE));
    return concat_magicfile_stream;
  }
  else
  {
    log_debug("fopen(\"%s\", \"%s\")\n", pathname, mode);
    return orig_fopen(pathname, mode);
  }
}

int fflush(FILE* stream)
{
  if (stream == concat_magicfile_stream)
  {
    return 0;
  }
  else
  {
    log_debug("fflush(%p)\n", stream);
    return orig_fflush(stream);
  }
}

int fclose(FILE* stream)
{
  if (stream == concat_magicfile_stream)
  {
    return 0;
  }
  else
  {
    log_debug("fclose(%p)\n", stream);
    return orig_fclose(stream);
  }
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  if (stream == concat_magicfile_stream)
  {
    return 0;
  }
  else
  {
    log_debug("fread(%p, %zu, %zu, %p)\n", ptr, size, nmemb, stream);
    return orig_fread(ptr, size, nmemb, stream);
  }
}

/*
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  return orig_fwrite(ptr, size, nmemb, stream);
}
*/

/* EOF */
