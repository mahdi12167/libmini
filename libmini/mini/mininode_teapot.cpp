// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "miniOGL.h"

#include "teapot.h"
#include "mininode_teapot.h"

mininode_geometry_teapot::mininode_geometry_teapot()
   : mininode_geometry()
   {}

void mininode_geometry_teapot::rendergeo(int /*wocolor*/,int /*wonrm*/,int /*wotex*/)
   {
   // front facing triangles are specified counter-clock-wise
   // back facing triangles are culled
   // normals are specified to face outward
   // a head light shader is applied by default
   // inward facing normals are flipped when shaded

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
   // bounding box of geometry = (-3,0,-2) to (3.434,3.15,2)
   bboxmin=miniv3d(-3,0,-2);
   bboxmax=miniv3d(3.434,3.15,2);
   }

double mininode_geometry_teapot::shoot(const miniv3d &o,const miniv3d &d,double mindist) const
   {
   // approximate exact hit point with bounding sphere
   return(shootbsphere(o,d,mindist));
   }
