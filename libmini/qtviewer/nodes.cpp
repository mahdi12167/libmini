// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "nodes.h"

// node_grid_extent:

node_grid_extent::node_grid_extent()
   : mininode_geometry_evaluator(), grid_extent()
   {}

node_grid_extent::node_grid_extent(const grid_extent &ext)
   : mininode_geometry_evaluator(), grid_extent(ext.corner[0],ext.corner[1],ext.corner[2],ext.corner[3])
   {construct();}

node_grid_extent::node_grid_extent(const minicoord &leftbottom,const minicoord &rightbottom,const minicoord &lefttop,const minicoord &righttop)
   : mininode_geometry_evaluator(), grid_extent(leftbottom,rightbottom,lefttop,righttop)
   {construct();}

void node_grid_extent::set(const minicoord &leftbottom,const minicoord &rightbottom,const minicoord &lefttop,const minicoord &righttop,
                           double maxsize)
   {
   grid_extent:set(leftbottom,rightbottom,lefttop,righttop);
   construct(maxsize);
   }

void node_grid_extent::construct(double maxsize)
   {
   double sx=get_size_ds();
   double sy=get_size_dt();

   int nx=max(ceil(sx/maxsize),1);
   int ny=max(ceil(sy/maxsize),1);

   mininode_geometry_evaluator::construct(nx,ny);
   }

miniv3d node_grid_extent::evaluate(double x,double y)
   {
   minicoord c;

   c=get_coord(x,y);
   c.convert2ecef();

   return(c.vec);
   }
