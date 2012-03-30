// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef NODES_H
#define NODES_H

#include <mini/mininodes.h>
#include <grid/grid_extent.h>

//! grid_extent evaluator node
class node_grid_extent: public mininode_geometry_evaluator, public grid_extent
   {
   public:

   //! default constructor
   node_grid_extent();

   //! default destructor
   virtual ~node_grid_extent() {}

   protected:

   virtual miniv3d evaluate(double x,double y);
   };

#endif
