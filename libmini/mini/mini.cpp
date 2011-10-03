// (c) by Stefan Roettger

#include "minibase.h"
#include "miniOGL.h"

// the global error handler for the mini library
void (*minibase::minierrorhandler)(const char *file,int line,int fatal)=NULL;

// set the global error handler for the mini library
void setminierrorhandler(void (*handler)(const char *file,int line,int fatal))
   {minibase::minierrorhandler=handler;}

// the mini namespace for 16bit data
namespace mini {
#include "minicore.cpp"
}

// the mini namespace for 32bit data
#define MINIFLOAT
namespace Mini {
#include "minicore.cpp"
}
