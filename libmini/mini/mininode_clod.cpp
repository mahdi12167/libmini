// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "mininode_clod.h"

// zscale
float mininode_geometry_path_clod::zscale=0.975f;

// default constructor
mininode_geometry_path_clod::mininode_geometry_path_clod()
   : mininode_geometry(0,3,0),
     miniclod()
   {}

// destructor
mininode_geometry_path_clod::~mininode_geometry_path_clod()
   {}

// set path
void mininode_geometry_path_clod::set(const minipath &path)
   {miniclod::set(path);}

// load path
void mininode_geometry_path_clod::load(ministring filename)
   {miniclod::load(filename);}

// create geometry from actual view point
void mininode_geometry_path_clod::create(double maxdev,double atdist,
                                         double maxwidth,
                                         double minv,double maxv,double sat,double val,
                                         double weight,
                                         int update)
   {
   miniclod::create(mininode_culling::peek_view(),
                    maxdev,atdist,
                    maxwidth,
                    minv,maxv,sat,val,
                    weight,
                    update);
   }

// pre-traversal method
void mininode_geometry_path_clod::traverse_pre()
   {
   create_inc(mininode_culling::peek_view()); // update path incrementally
   mininode_geometry::traverse_pre(); // traverse base class
   }

// post-updated callback
void mininode_geometry_path_clod::updated(const minidyna<mini3D::joint_struct> &points)
   {
   *(mininode_geometry *)this=mininode_geometry_band(points);
   setZscale(zscale);
   }
