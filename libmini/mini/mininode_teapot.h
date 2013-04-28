// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MININODE_TEAPOT_H
#define MININODE_TEAPOT_H

#include "mininode_geom.h"

//! a geometry node that represents the Utah teapot
class mininode_geometry_teapot: public mininode_geom
   {
   public:

   mininode_geometry_teapot();

   virtual void rendergeo(int wocolor=0,int wonrm=0,int wotex=0);
   virtual void getbbox(miniv3d &bboxmin,miniv3d &bboxmax) const;
   };

#endif
