// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MININODE_CLOD_H
#define MININODE_CLOD_H

#include "mininodes.h"
#include "minipath.h"
#include "miniclod.h"

//! a geometry node that implements C-LOD for geo-referenced paths
class mininode_geometry_path_clod: public mininode_geometry, public miniclod
   {
   public:

   //! default constructor
   mininode_geometry_path_clod();

   //! destructor
   virtual ~mininode_geometry_path_clod();

   //! set path
   void set(const minipath &path);

   //! load path
   void load(ministring filename);

   //! create geometry from actual view point
   void create(double maxdev=1,double atdist=100, // maximum deviation at specific distance
               double maxwidth=1, // maximum width at specific distance
               double minv=0.0,double maxv=30.0,double sat=1.0,double val=1.0, // velocity color mapping
               double weight=1.0, // weight of color mapping
               int update=100); // vertices per update

   //! zscale
   static float zscale;

   protected:

   virtual void traverse_pre();

   virtual void updated(const minidyna<mini3D::joint_struct> &points);
   };

#endif
