// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "mininode_path.h"

// default constructor
mininode_geometry_path::mininode_geometry_path()
   : mininode_geometry(0,3,0), minipath()
   {}

// constructor
mininode_geometry_path::mininode_geometry_path(ministring filename,double width)
   : mininode_geometry(0,3,0), minipath(filename)
   {recreate(width);}

// destructor
mininode_geometry_path::~mininode_geometry_path()
   {}

// recreate geometry from actual view point
void mininode_geometry_path::recreate(double width)
   {*(mininode_geometry *)this=mininode_geometry_band(*this,width);}
