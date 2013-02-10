// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef NODES_H
#define NODES_H

#include <mini/mini_object.h>
#include <grid/grid_extent.h>

//! grid_extent evaluator node
class node_grid_extent: public mininode_geometry_evaluator, public grid_extent
   {
   public:

   //! default constructor
   node_grid_extent();

   //! copy constructor
   node_grid_extent(const grid_extent &ext,
                    double maxsize=100000.0);

   //! custom constructor
   node_grid_extent(const minicoord &leftbottom,const minicoord &rightbottom,const minicoord &lefttop,const minicoord &righttop,
                    double maxsize=100000.0);

   //! default destructor
   virtual ~node_grid_extent() {}

   //! set extent via 4 corner points
   void set(const minicoord &leftbottom,const minicoord &rightbottom,const minicoord &lefttop,const minicoord &righttop,
            double maxsize=100000.0);

   protected:

   void construct(double maxsize=100000.0);

   virtual miniv3d evaluate(double x,double y);
   };

#endif
