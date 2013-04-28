// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MININODE_GEOM_H
#define MININODE_GEOM_H

#include "miniOGL.h"

#include "mininodes.h"

//! a geometry node base class for special use cases
//!  use mininode_geometry to put geometry as a single triangle strip
//!  for all other use cases derive from this base class
class mininode_geom: public mininode_geometry
   {
   public:

   mininode_geom() : mininode_geometry() {}
   virtual ~mininode_geom() {}

   //! render geometric primitives
   //!  front facing triangles are specified counter-clock-wise
   //!  back facing triangles are culled
   //!  normals are specified to face outward
   //!  a head light shader is applied by default
   //!  inward facing normals are flipped when shaded
   virtual void rendergeo(int wocolor=0,int wonrm=0,int wotex=0) = 0;

   //! return bounding box of geometric primitives
   //!  for example the utah teapot has a bounding box of
   //!   bboxmin=miniv3d(-3,0,-2) to bboxmax=miniv3d(3.434,3.15,2)
   virtual void getbbox(miniv3d &bboxmin,miniv3d &bboxmax) const = 0;

   //! shoot a ray at the geometric primitives
   //!  by default the method returns the hit point on the bounding sphere
   //!  for exact hit points override the method and
   //!   provide the distance of the closest ray-triangle hit calculation
   //!   if there is no hit point provide MAXFLOAT as result
   virtual double shoot(const miniv3d &o,const miniv3d &d,double mindist=0.0) const
      {
      // approximate exact hit point with bounding sphere
      return(shootbsphere(o,d,mindist));
      }

   };

#endif
