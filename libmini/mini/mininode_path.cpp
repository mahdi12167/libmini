// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "mininode_path.h"

// zscale
float mininode_geometry_path::zscale=0.975f;

// default constructor
mininode_geometry_path::mininode_geometry_path()
   : mininode_geometry(0,3,0)
   {}

// constructor
mininode_geometry_path::mininode_geometry_path(ministring filename,double width)
   : mininode_geometry(0,3,0)
   {
   path_.load(filename);
   recreate(width);
   }

// destructor
mininode_geometry_path::~mininode_geometry_path()
   {}

// recreate geometry with a particular band width
void mininode_geometry_path::recreate(double width)
   {
   *(mininode_geometry *)this=mininode_geometry_band_path(path_,width);
   setZscale(zscale);
   }
