// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MININODE_PATH_H
#define MININODE_PATH_H

#include "mininode_geom.h"
#include "minipath.h"

//! a geometry node that implements C-LOD for line strips
class mininode_geometry_path: public mininode_geom, minipath
   {
   public:

   mininode_geometry_path();
   virtual ~mininode_geometry_path();

   virtual void rendergeo(int wocolor=0,int wonrm=0,int wotex=0);
   virtual void getbbox(miniv3d &bboxmin,miniv3d &bboxmax) const;

   virtual double shoot(const miniv3d &o,const miniv3d &d,double mindist=0.0) const
      {
      // intersection with path is void
      return(-MAXFLOAT);
      }

   };

#endif
