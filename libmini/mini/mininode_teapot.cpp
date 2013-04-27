// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "miniOGL.h"

#include "teapot.h"
#include "mininode_teapot.h"

mininode_geometry_teapot::mininode_geometry_teapot()
   : mininode_geometry()
   {}

void mininode_geometry_teapot::rendergeo(int /*wocolor*/,int /*wonrm*/,int /*wotex*/)
   {
   // normals are specified to face outward
   // front facing triangles are specified counter-clock-wise
   drawTeapotElements();
   }

void mininode_geometry_teapot::getbbox(miniv3d &bboxmin,miniv3d &bboxmax) const
   {
   bboxmin=miniv3d(-3,0,-2);
   bboxmax=miniv3d(3.434,3.15,2);
   }

double mininode_geometry_teapot::shoot(const miniv3d &o,const miniv3d &d,double mindist) const
   {return(shootbsphere(o,d,mindist));}
