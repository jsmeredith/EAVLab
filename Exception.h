// Copyright 2012 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "STL.h"

#include <stdarg.h>

///\todo: this probably should go away and be replaced with eavlException
class Exception
{
  public:
    string message;
  public:
    Exception(const std::string &msg) : message(msg) { }
    Exception(const char *fmt, ...)
    {
        va_list argp;
        char msg[4096];
        va_start(argp, fmt);
        vsprintf(msg, fmt, argp);
        va_end(argp);
        message = msg;
    }
};


#endif
