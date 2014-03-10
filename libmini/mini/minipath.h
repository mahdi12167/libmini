// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIPATH_H
#define MINIPATH_H

#include "minicurve.h"

class minipath : public minicurve
   {
   public:

   minipath(double start=0.0,double stop=1.0)
      : minicurve(start,stop)
      {}

   BOOLINT load(const char *filename);
   BOOLINT save(const char *filename);
   };

#endif
