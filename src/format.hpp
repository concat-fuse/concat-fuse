#ifndef HEADER_PRINT_HPP
#define HEADER_PRINT_HPP

#include <ostream>
#include <iostream>
#include <stdlib.h>
#include <stdexcept>

inline void format_arg(std::ostream& os, size_t i)
{
  throw std::invalid_argument("not enough arguments to format");
}

template<typename Arg0, typename... Args>
void format_arg(std::ostream& os, size_t i, Arg0&& arg0, Args&&... args)
{
  if (i == 0)
  {
    os << std::forward<Arg0>(arg0);
  }
  else
  {
    format_arg(os, i - 1, std::forward<Args>(args)...);
  }
}

/** Simple Python'ish format function. {} gets replaced with arg, {{
    and }} escapes */
template<typename... Args>
void format(std::ostream& os, const char* fmt, Args&&... args)
{
  size_t arg_idx = 0;

  size_t start = 0;
  size_t i = 0;
  while(fmt[i] != '\0')
  {
    if (fmt[i] == '{')
    {
      if (fmt[i+1] == '{')
      {
        os.write(fmt + start, i + 1 - start);

        i += 2;
        start = i;
      }
      else if (fmt[i+1] == '}')
      {
        os.write(fmt + start, i - start);

        format_arg(os, arg_idx, std::forward<Args>(args)...);
        arg_idx += 1;

        i += 2;
        start = i;
      }
      else
      {
        throw std::invalid_argument("{ not followed by { or }");
      }
    }
    else if (fmt[i] == '}')
    {
      if (fmt[i+1] == '}')
      {
        os.write(fmt + start, i + 1 - start);
        i += 2;
        start = i;
      }
      else
      {
        throw std::invalid_argument("} not followed by }");
      }
    }
    else
    {
      i += 1;
    }
  }

  if (start != i)
  {
    os.write(fmt + start, i - start);
  }
}

#endif

/* EOF */
