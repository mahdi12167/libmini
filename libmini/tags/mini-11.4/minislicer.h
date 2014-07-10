// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINISLICER_H
#define MINISLICER_H

#include "miniv3d.h"

namespace minislice {

// extract 1 triangle from a tetrahedron
//  v0 is the cutaway vertex
//  d is distance of the respective point to the cutting plane
inline void slice1tri(const miniv3d &v0,const double d0,
                      const miniv3d &v1,const double d1,
                      const miniv3d &v2,const double d2,
                      const miniv3d &v3,const double d3)
   {
   miniv3d p0,p1,p2;

   p0=(d1*v0+d0*v1)/(d0+d1);
   p1=(d2*v0+d0*v2)/(d0+d2);
   p2=(d3*v0+d0*v3)/(d0+d3);

   beginfans();
   texcoord(p0.x,p0.y,p0.z);
   fanvertex(p0.x,p0.y,p0.z);
   texcoord(p1.x,p1.y,p1.z);
   fanvertex(p1.x,p1.y,p1.z);
   texcoord(p2.x,p2.y,p2.z);
   fanvertex(p2.x,p2.y,p2.z);
   }

// extract 2 triangles (a quad) from a tetrahedron
//  v0 is the start point of the cutaway line segment
//  v1 is the end point of the cutaway line segment
//  d is distance of the respective point to the cutting plane
inline void slice2tri(const miniv3d &v0,const double d0,
                      const miniv3d &v1,const double d1,
                      const miniv3d &v2,const double d2,
                      const miniv3d &v3,const double d3)
   {
   miniv3d p0,p1,p2,p3;

   p0=(d2*v0+d0*v2)/(d0+d2);
   p1=(d2*v1+d1*v2)/(d1+d2);
   p2=(d3*v0+d0*v3)/(d0+d3);
   p3=(d3*v1+d1*v3)/(d1+d3);

   beginfans();
   texcoord(p0.x,p0.y,p0.z);
   fanvertex(p0.x,p0.y,p0.z);
   texcoord(p1.x,p1.y,p1.z);
   fanvertex(p1.x,p1.y,p1.z);
   texcoord(p3.x,p3.y,p3.z);
   fanvertex(p3.x,p3.y,p3.z);
   texcoord(p2.x,p2.y,p2.z);
   fanvertex(p2.x,p2.y,p2.z);
   }

// extract a slice from a tetrahedron
//  2 cases: slice geometry consists of either 1 or 2 triangles
inline void slicetet(const miniv3d &v0, // vertex v0
                     const miniv3d &v1, // vertex v1
                     const miniv3d &v2, // vertex v2
                     const miniv3d &v3, // vertex v3
                     const miniv3d &o,  // origin of cutting plane
                     const miniv3d &n)  // normal of cutting plane
   {
   double d0,d1,d2,d3;

   int ff;

   d0=(v0-o)*n;
   d1=(v1-o)*n;
   d2=(v2-o)*n;
   d3=(v3-o)*n;

   ff=0;

   if (d0<0.0) ff|=1;
   if (d1<0.0) ff|=2;
   if (d2<0.0) ff|=4;
   if (d3<0.0) ff|=8;

   switch (ff)
      {
      // 1 triangle
      case 1: case 14: slice1tri(v0,fabs(d0),
                                 v1,fabs(d1),
                                 v2,fabs(d2),
                                 v3,fabs(d3)); break;
      case 2: case 13: slice1tri(v1,fabs(d1),
                                 v0,fabs(d0),
                                 v2,fabs(d2),
                                 v3,fabs(d3)); break;
      case 4: case 11: slice1tri(v2,fabs(d2),
                                 v0,fabs(d0),
                                 v1,fabs(d1),
                                 v3,fabs(d3)); break;
      case 8: case 7:  slice1tri(v3,fabs(d3),
                                 v0,fabs(d0),
                                 v1,fabs(d1),
                                 v2,fabs(d2)); break;

      // 2 triangles
      case 3:  slice2tri(v0,fabs(d0),
                         v1,fabs(d1),
                         v2,fabs(d2),
                         v3,fabs(d3)); break;
      case 5:  slice2tri(v0,fabs(d0),
                         v2,fabs(d2),
                         v1,fabs(d1),
                         v3,fabs(d3)); break;
      case 6:  slice2tri(v1,fabs(d1),
                         v2,fabs(d2),
                         v0,fabs(d0),
                         v3,fabs(d3)); break;
      case 9:  slice2tri(v0,fabs(d0),
                         v3,fabs(d3),
                         v1,fabs(d1),
                         v2,fabs(d2)); break;
      case 10: slice2tri(v1,fabs(d1),
                         v3,fabs(d3),
                         v0,fabs(d0),
                         v2,fabs(d2)); break;
      case 12: slice2tri(v2,fabs(d2),
                         v3,fabs(d3),
                         v0,fabs(d0),
                         v1,fabs(d1)); break;
      }
   }

// extract a slice from a hexahedron
inline void slicehex(const miniv3d &v0, // vertex v0
                     const miniv3d &v1, // vertex v1
                     const miniv3d &v2, // vertex v2
                     const miniv3d &v3, // vertex v3
                     const miniv3d &v4, // vertex v4
                     const miniv3d &v5, // vertex v5
                     const miniv3d &v6, // vertex v6
                     const miniv3d &v7, // vertex v7
                     const miniv3d &o,  // origin of cutting plane
                     const miniv3d &n)  // normal of cutting plane
   {
   // center tet
   slicetet(v0,v3,v5,v6,o,n);

   // corner tets
   slicetet(v0,v5,v3,v1,o,n);
   slicetet(v3,v6,v0,v2,o,n);
   slicetet(v0,v6,v5,v4,o,n);
   slicetet(v3,v5,v6,v7,o,n);
   }

// extract a slice stack from a hexahedron
inline void stack(const miniv3d &v0, // vertex v0
                  const miniv3d &v1, // vertex v1
                  const miniv3d &v2, // vertex v2
                  const miniv3d &v3, // vertex v3
                  const miniv3d &v4, // vertex v4
                  const miniv3d &v5, // vertex v5
                  const miniv3d &v6, // vertex v6
                  const miniv3d &v7, // vertex v7
                  const miniv3d &o,  // origin of cutting plane
                  const miniv3d &n,  // normal of cutting plane
                  float maxd,  // maximum plane distance
                  float slab,  // slab thickness
                  float mu_a,  // global absorption
                  float mu_e,  // global emission
                  int twidth,  // texture width
                  int theight, // texture height
                  int tdepth)  // texture depth
   {
   double maxsize=max(max(twidth,theight),tdepth);

   double ax=(twidth-1)/(maxsize-1);
   double ay=(theight-1)/(maxsize-1);
   double az=(tdepth-1)/(maxsize-1);

   mtxtex();
   mtxid();
   mtxtranslate(0.5/twidth,0.5/theight,0.5/tdepth);
   mtxscale((twidth-1.0)/twidth,(theight-1.0)/theight,(tdepth-1.0)/tdepth);
   mtxtranslate(0.5,0.5,0.5);
   mtxscale(1.0/ax,1.0/ay,1.0/az);
   mtxmodel();

   float emiss=1-exp(-mu_a*slab);
   float alpha=mu_e*slab;

   color(emiss,emiss,emiss,alpha);

   for (double d=maxd; d>0.0; d-=slab)
      slicehex(v0,v1,v2,v3,
               v4,v5,v6,v7,
               o+d*n,n);
   }

}

#endif
