// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIDEFS_H
#define MINIDEFS_H

#include <stddef.h>
#include <float.h>

#include <limits> // for nan

#ifndef NULL
#   define NULL (0)
#endif

#define BOOLINT char

#ifndef TRUE
#   define TRUE (1)
#endif
#ifndef FALSE
#   define FALSE (0)
#endif

#ifndef PI
#   define PI (3.14159265358979323846)
#endif
#ifndef RAD
#   define RAD (PI/180.0)
#endif

#ifndef MAXFLOAT
#   define MAXFLOAT (FLT_MAX)
#endif

#ifndef NAN
#   define NAN (std::numeric_limits<double>::quiet_NaN())
#endif

#endif
