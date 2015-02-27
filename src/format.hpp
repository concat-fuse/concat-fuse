#ifndef HEADER_PRINT_HPP
#define HEADER_PRINT_HPP

#include <ostream>
#include <iostream>
#include <stdlib.h>
#include <stdexcept>

/** Simple Python'ish format function. {} gets replaced with arg, {{
    and }} escapes */
template<typename... Args>
void format(std::ostream& os, const char* fmt)
{
  size_t i = 0;
  while(fmt[i] != '\0')
  {
    if (fmt[i] == '{')
    {
      if (fmt[i+1] == '{')
      {
        os.write(fmt, i + 1);
        format(os, fmt + i + 2);
        return;
      }
      else if (fmt[i+1] == '}')
      {
        throw std::invalid_argument("not enough arguments to format");
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
        os.write(fmt, i + 1);
        format(os, fmt + i + 2);
        return;
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

  os.write(fmt, i);
}

/** Simple Python'ish format function. {} gets replaced with arg, {{
    and }} escapes */
template<typename Arg0, typename... Args>
void format(std::ostream& os, const char* fmt, Arg0&& arg0, Args&&... args)
{
  size_t i = 0;
  while(fmt[i] != '\0')
  {
    if (fmt[i] == '{')
    {
      if (fmt[i+1] == '{')
      {
        os.write(fmt, i + 1);
        format(os, fmt + i + 2, arg0, args...);
        return;
      }
      else if (fmt[i+1] == '}')
      {
        os.write(fmt, i);
        os << arg0;
        format(os, fmt + i + 2, args...);
        return;
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
        os.write(fmt, i + 1);
        format(os, fmt + i + 2, arg0, args...);
        return;
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
  throw std::invalid_argument("to many arguments to format");
}

#endif

/* EOF */
