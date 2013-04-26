// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "miniOGL.h"

#include "teapot.h"
#include "mininode_teapot.h"

mininode_geometry_teapot::mininode_geometry_teapot()
   : mininode_geometry()
   {}

void mininode_geometry_teapot::render(int wocolor,int wonrm,int wotex)
   {
   vertexarray(teapotVertices);
   normalarray(teapotNormals);
   drawTeapotElements();
   vertexarray(NULL);
   normalarray(NULL);
   }

void mininode_geometry_teapot::getbbox(miniv3d &bboxmin,miniv3d &bboxmax) const
   {
   bboxmin=miniv3d(-3,0,-2);
   bboxmax=miniv3d(3.434,3.15,2);
   }

double mininode_geometry_teapot::shoot(const miniv3d &o,const miniv3d &d,double mindist) const
   {return(shootbsphere(o,d,mindist));}
