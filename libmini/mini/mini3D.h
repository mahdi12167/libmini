// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINI3D_H
#define MINI3D_H

#include "miniv3f.h"
#include "miniv3d.h"

//! 3D software rendering pipeline
class mini3D
   {
   public:

   struct point_struct_line
      {
      miniv3d pos;
      miniv3d col;
      };

   struct point_struct_band
      {
      miniv3d pos;
      miniv3d nrm;
      miniv3d col;
      double wdt;
      };

   struct point_struct_sphere
      {
      miniv3d pos;
      miniv3d col;
      double r;
      };

   //! default constructor
   mini3D();

   //! destructor
   virtual ~mini3D();
   };

#endif
