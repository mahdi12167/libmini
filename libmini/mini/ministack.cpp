// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "ministack.h"

std::vector<M4x4> scoped_push::stack_;

// calculate determinant of 4x4 matrix
double M4x4::det() const
   {
   return(mtx[0].w*mtx[1].z*mtx[2].y*mtx[3].x-
          mtx[0].z*mtx[1].w*mtx[2].y*mtx[3].x-
          mtx[0].w*mtx[1].y*mtx[2].z*mtx[3].x+
          mtx[0].y*mtx[1].w*mtx[2].z*mtx[3].x+
          mtx[0].z*mtx[1].y*mtx[2].w*mtx[3].x-
          mtx[0].y*mtx[1].z*mtx[2].w*mtx[3].x-
          mtx[0].w*mtx[1].z*mtx[2].x*mtx[3].y+
          mtx[0].z*mtx[1].w*mtx[2].x*mtx[3].y+
          mtx[0].w*mtx[1].x*mtx[2].z*mtx[3].y-
          mtx[0].x*mtx[1].w*mtx[2].z*mtx[3].y-
          mtx[0].z*mtx[1].x*mtx[2].w*mtx[3].y+
          mtx[0].x*mtx[1].z*mtx[2].w*mtx[3].y+
          mtx[0].w*mtx[1].y*mtx[2].x*mtx[3].z-
          mtx[0].y*mtx[1].w*mtx[2].x*mtx[3].z-
          mtx[0].w*mtx[1].x*mtx[2].y*mtx[3].z+
          mtx[0].x*mtx[1].w*mtx[2].y*mtx[3].z+
          mtx[0].y*mtx[1].x*mtx[2].w*mtx[3].z-
          mtx[0].x*mtx[1].y*mtx[2].w*mtx[3].z-
          mtx[0].z*mtx[1].y*mtx[2].x*mtx[3].w+
          mtx[0].y*mtx[1].z*mtx[2].x*mtx[3].w+
          mtx[0].z*mtx[1].x*mtx[2].y*mtx[3].w-
          mtx[0].x*mtx[1].z*mtx[2].y*mtx[3].w-
          mtx[0].y*mtx[1].x*mtx[2].z*mtx[3].w+
          mtx[0].x*mtx[1].y*mtx[2].z*mtx[3].w);
   }

// invert 4x4 matrix
M4x4 M4x4::invert() const
   {
   M4x4 m;
   double d;

   // calculate determinant
   d=det();

   // check determinant
   assert(d!=0.0);

   // calculate inverse
   d=1.0/d;
   m[0].x=d*(mtx[1].z*mtx[2].w*mtx[3].y-mtx[1].w*mtx[2].z*mtx[3].y+mtx[1].w*mtx[2].y*mtx[3].z-mtx[1].y*mtx[2].w*mtx[3].z-mtx[1].z*mtx[2].y*mtx[3].w+mtx[1].y*mtx[2].z*mtx[3].w);
   m[0].y=d*(mtx[0].w*mtx[2].z*mtx[3].y-mtx[0].z*mtx[2].w*mtx[3].y-mtx[0].w*mtx[2].y*mtx[3].z+mtx[0].y*mtx[2].w*mtx[3].z+mtx[0].z*mtx[2].y*mtx[3].w-mtx[0].y*mtx[2].z*mtx[3].w);
   m[0].z=d*(mtx[0].z*mtx[1].w*mtx[3].y-mtx[0].w*mtx[1].z*mtx[3].y+mtx[0].w*mtx[1].y*mtx[3].z-mtx[0].y*mtx[1].w*mtx[3].z-mtx[0].z*mtx[1].y*mtx[3].w+mtx[0].y*mtx[1].z*mtx[3].w);
   m[0].w=d*(mtx[0].w*mtx[1].z*mtx[2].y-mtx[0].z*mtx[1].w*mtx[2].y-mtx[0].w*mtx[1].y*mtx[2].z+mtx[0].y*mtx[1].w*mtx[2].z+mtx[0].z*mtx[1].y*mtx[2].w-mtx[0].y*mtx[1].z*mtx[2].w);
   m[1].x=d*(mtx[1].w*mtx[2].z*mtx[3].x-mtx[1].z*mtx[2].w*mtx[3].x-mtx[1].w*mtx[2].x*mtx[3].z+mtx[1].x*mtx[2].w*mtx[3].z+mtx[1].z*mtx[2].x*mtx[3].w-mtx[1].x*mtx[2].z*mtx[3].w);
   m[1].y=d*(mtx[0].z*mtx[2].w*mtx[3].x-mtx[0].w*mtx[2].z*mtx[3].x+mtx[0].w*mtx[2].x*mtx[3].z-mtx[0].x*mtx[2].w*mtx[3].z-mtx[0].z*mtx[2].x*mtx[3].w+mtx[0].x*mtx[2].z*mtx[3].w);
   m[1].z=d*(mtx[0].w*mtx[1].z*mtx[3].x-mtx[0].z*mtx[1].w*mtx[3].x-mtx[0].w*mtx[1].x*mtx[3].z+mtx[0].x*mtx[1].w*mtx[3].z+mtx[0].z*mtx[1].x*mtx[3].w-mtx[0].x*mtx[1].z*mtx[3].w);
   m[1].w=d*(mtx[0].z*mtx[1].w*mtx[2].x-mtx[0].w*mtx[1].z*mtx[2].x+mtx[0].w*mtx[1].x*mtx[2].z-mtx[0].x*mtx[1].w*mtx[2].z-mtx[0].z*mtx[1].x*mtx[2].w+mtx[0].x*mtx[1].z*mtx[2].w);
   m[2].x=d*(mtx[1].y*mtx[2].w*mtx[3].x-mtx[1].w*mtx[2].y*mtx[3].x+mtx[1].w*mtx[2].x*mtx[3].y-mtx[1].x*mtx[2].w*mtx[3].y-mtx[1].y*mtx[2].x*mtx[3].w+mtx[1].x*mtx[2].y*mtx[3].w);
   m[2].y=d*(mtx[0].w*mtx[2].y*mtx[3].x-mtx[0].y*mtx[2].w*mtx[3].x-mtx[0].w*mtx[2].x*mtx[3].y+mtx[0].x*mtx[2].w*mtx[3].y+mtx[0].y*mtx[2].x*mtx[3].w-mtx[0].x*mtx[2].y*mtx[3].w);
   m[2].z=d*(mtx[0].y*mtx[1].w*mtx[3].x-mtx[0].w*mtx[1].y*mtx[3].x+mtx[0].w*mtx[1].x*mtx[3].y-mtx[0].x*mtx[1].w*mtx[3].y-mtx[0].y*mtx[1].x*mtx[3].w+mtx[0].x*mtx[1].y*mtx[3].w);
   m[2].w=d*(mtx[0].w*mtx[1].y*mtx[2].x-mtx[0].y*mtx[1].w*mtx[2].x-mtx[0].w*mtx[1].x*mtx[2].y+mtx[0].x*mtx[1].w*mtx[2].y+mtx[0].y*mtx[1].x*mtx[2].w-mtx[0].x*mtx[1].y*mtx[2].w);
   m[3].x=d*(mtx[1].z*mtx[2].y*mtx[3].x-mtx[1].y*mtx[2].z*mtx[3].x-mtx[1].z*mtx[2].x*mtx[3].y+mtx[1].x*mtx[2].z*mtx[3].y+mtx[1].y*mtx[2].x*mtx[3].z-mtx[1].x*mtx[2].y*mtx[3].z);
   m[3].y=d*(mtx[0].y*mtx[2].z*mtx[3].x-mtx[0].z*mtx[2].y*mtx[3].x+mtx[0].z*mtx[2].x*mtx[3].y-mtx[0].x*mtx[2].z*mtx[3].y-mtx[0].y*mtx[2].x*mtx[3].z+mtx[0].x*mtx[2].y*mtx[3].z);
   m[3].z=d*(mtx[0].z*mtx[1].y*mtx[3].x-mtx[0].y*mtx[1].z*mtx[3].x-mtx[0].z*mtx[1].x*mtx[3].y+mtx[0].x*mtx[1].z*mtx[3].y+mtx[0].y*mtx[1].x*mtx[3].z-mtx[0].x*mtx[1].y*mtx[3].z);
   m[3].w=d*(mtx[0].y*mtx[1].z*mtx[2].x-mtx[0].z*mtx[1].y*mtx[2].x+mtx[0].z*mtx[1].x*mtx[2].y-mtx[0].x*mtx[1].z*mtx[2].y-mtx[0].y*mtx[1].x*mtx[2].z+mtx[0].x*mtx[1].y*mtx[2].z);

   return(m);
   }
