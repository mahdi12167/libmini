// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "mininode_path.h"

mininode_geometry_path::mininode_geometry_path()
   : mininode_geom(), minipath()
   {}

void mininode_geometry_path::rendergeo(int /*wocolor*/,int /*wonrm*/,int /*wotex*/)
   {
#ifndef NOOGL

   CODEERROR();

#endif
   }

void mininode_geometry_path::getbbox(miniv3d &bboxmin,miniv3d &bboxmax) const
   {
   bboxmin=miniv3d(0,0,0);
   bboxmax=miniv3d(0,0,0);
   }
