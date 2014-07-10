// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minibase.h"
#include "miniOGL.h"

// the global error handler for the mini library
void (*minibase::minierrorhandler)(const char *file,int line,int fatal,const char *msg)=0;

// set the global error handler for the mini library
void setminierrorhandler(void (*handler)(const char *file,int line,int fatal,const char *msg))
   {minibase::minierrorhandler=handler;}

// the mini namespace for 16bit elevation data
namespace mini {
#include "minicore.cpp"
}

// the Mini namespace for 32bit elevation data
#define MINIFLOAT
namespace Mini {
#include "minicore.cpp"
}
