// (c) by Stefan Roettger

#ifndef MINI_H
#define MINI_H

void setminierrorhandler(void (*handler)(const char *file,int line,int fatal)=0);

namespace mini {
#include "core.h"
}

#define MINIFLOAT
namespace Mini {
#include "core.h"
}

#endif
