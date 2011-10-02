// (c) by Stefan Roettger

#include "miniP.h"

// set the global error handler for the mini library
void setminierrorhandler(void (*handler)(const char *file,int line,int fatal))
   {minibase::minierrorhandler=handler;}

namespace mini {
#include "core.cpp"
}

#define MINIFLOAT
namespace Mini {
#include "core.cpp"
}
