// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "mininode_teapot.h"

#ifndef NOOGL
#include "teapot.h"
#endif

mininode_geometry_teapot::mininode_geometry_teapot()
   : mininode_geom()
   {}

void mininode_geometry_teapot::rendergeo(int /*wocolor*/,int /*wonrm*/,int /*wotex*/)
   {
#ifndef NOOGL

   glDisable(GL_CULL_FACE);

   glVertexPointer(3,GL_FLOAT,0,teapotVertices);
   glEnableClientState(GL_VERTEX_ARRAY);
   glNormalPointer(GL_FLOAT,0,teapotNormals);
   if (wonrm==0) glEnableClientState(GL_NORMAL_ARRAY);

   drawTeapotElements();

   glDisableClientState(GL_VERTEX_ARRAY);
   if (wonrm==0) glDisableClientState(GL_NORMAL_ARRAY);

   glEnable(GL_CULL_FACE);

#endif
   }

void mininode_geometry_teapot::getbbox(miniv3d &bboxmin,miniv3d &bboxmax) const
   {
   // return bounding box of geometry = (-3,0,-2) to (3.434,3.15,2)
   bboxmin=miniv3d(-3,0,-2);
   bboxmax=miniv3d(3.434,3.15,2);
   }
