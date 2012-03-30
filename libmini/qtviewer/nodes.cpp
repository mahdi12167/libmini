// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "nodes.h"

// node_grid_extent:

node_grid_extent::node_grid_extent()
   : mininode_geometry_evaluator(), grid_extent()
   {construct();}

miniv3d node_grid_extent::evaluate(double x,double y)
   {
   return(miniv3d(x,y,0));
   }
