// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIKDTREE_H
#define MINIKDTREE_H

#include "minibase.h"
#include "miniv3d.h"

//! templated kd-tree
template <class Item, class Vector3D = miniv3d>
class minikdtree
   {
   public:

   // definition of plane orientations
   enum {
      x_axis = 0,
      y_axis = 1,
      z_axis = 2,
   };

   // definition of plane
   typedef struct {
      Vector3D point;
      char orientation;
   } Plane;

   // node definition
   typedef struct NodeStruct {
      struct NodeStruct *leftSpace, *rightSpace;
      Plane plane;
      Item item;
   } Node;

   //! default constructor
   minikdtree() {root=NULL;}

   //! destructor
   ~minikdtree()
      {remove();}

   //! insert the an item at a particular point into the kd-tree
   void insert(const Vector3D &point, const Item &item, Node **node)
      {
      int level = 0;

      while (*node!=NULL)
         {
         if (pointInLeftHalfSpace(point, *node))
            node = &((*node)->leftSpace);
         else
            node = &((*node)->rightSpace);

         level++;
         }

      *node = new Node;

      (*node)->item = item;
      (*node)->leftSpace = (*node)->rightSpace = NULL;

      (*node)->plane.point = point;
      (*node)->plane.orientation = level%3;
      }

   void insert(const Vector3D &point, const Item &item)
      {insert(point, item, &root);}

   //! remove items in subtree
   void remove(Node **node)
      {
      if (*node!=NULL)
         {
         remove(&((*node)->leftSpace));
         remove(&((*node)->rightSpace));

         delete *node;
         *node = NULL;
         }
      }

   void remove()
      {remove(&root);}

   //! count items in subtree
   unsigned int items(const Node *node)
      {
      if (node!=NULL)
         return(items(node->leftSpace) + items(node->rightSpace)) + 1;

      return(0);
      }

   unsigned int items()
      {return(items(root));}

   //! count maximum branch length in subtree
   unsigned int depth(const Node *node)
      {
      if (node!=NULL)
         {
         unsigned int left = depth(node->leftSpace);
         unsigned int right = depth(node->rightSpace);

         return(max(left, right) + 1);
         }

      return(0);
      }

   unsigned int depth()
      {return(depth(root));}

   protected:

   // euclidean distance of two points
   double getDistance(const Vector3D &point1, const Vector3D &point2)
      {
      Vector3D diff = point2-point1;

      return(sqrt(diff * diff));
      }

   // minimum distance of a point to a plane
   //  the plane is given by a point on the plane and
   //  a normal that is orthogonal to the plane
   //  as defined by the Hessian Normal Form
   double getDistance(const Vector3D &point1, const Vector3D &point2, const Vector3D &normal)
      {
      Vector3D diff = point2-point1;

      return(diff * normal);
      }

   // determines whether or not a point is in the left half space of a kd-tree node
   BOOLINT pointInLeftHalfSpace(const Vector3D &point, const Node *node)
      {
      Vector3D normal(0,0,0);

      switch (node->plane.orientation)
         {
         case x_axis: normal = Vector3D(1,0,0); break;
         case y_axis: normal = Vector3D(0,1,0); break;
         case z_axis: normal = Vector3D(0,0,1); break;
         }

      double distance = getDistance(point, node->plane.point, normal);

      return(distance <= 0.0);
      }

   Node *root;
   };

#endif
