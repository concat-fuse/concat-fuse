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
#include <stdio.h>
#include <dlfcn.h>
#include <ctype.h>

typedef int (*orig_open_t)(const char* pathname, int flags);
typedef int (*orig_close_t)(int fd);
typedef off_t (*orig_lseek_t)(int fd, off_t offset, int whence);
typedef ssize_t (*orig_read_t)(int fd, void* buf, size_t count);

int open(const char* pathname, int flags)
{
  printf("== open()\n");
  
  orig_open_t orig_open;
  orig_open = (orig_open_t)dlsym(RTLD_NEXT, "open");
  
  return orig_open(pathname, flags);
}

off_t lseek(int fd, off_t offset, int whence)
{
  printf("== lseek()\n");
  
  orig_lseek_t orig_lseek;
  orig_lseek = (orig_lseek_t)dlsym(RTLD_NEXT, "lseek");
  return orig_lseek(fd, offset, whence);
}

ssize_t read(int fd, void* buf, size_t count)
{
  printf("== read()\n");
   
  orig_read_t orig_read;
  orig_read = (orig_read_t)dlsym(RTLD_NEXT, "read");
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
  printf("== close()\n");

  orig_close_t orig_close;
  orig_close = (orig_close_t)dlsym(RTLD_NEXT, "close");
  
  return orig_close(fd);
}

/* EOF */
