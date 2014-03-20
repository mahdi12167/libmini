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
