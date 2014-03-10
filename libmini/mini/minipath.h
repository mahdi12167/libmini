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

   BOOLINT load(ministring filename);
   void save(ministring filename);

   protected:

   BOOLINT read_curve_format(ministrings curve);
   BOOLINT read_trk_format(ministrings trk);
   };

#endif
