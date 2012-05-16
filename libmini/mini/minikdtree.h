// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIKDTREE_H
#define MINIKDTREE_H

#include "minibase.h"
#include "miniv3d.h"
#include "minidyna.h"
#include "minisort.h"

//! templated kd-tree
template <class Item, class Vector3D = miniv3d>
class minikdtree
   {
   public:

   //! default constructor
   minikdtree()
      {root = NULL;}

   //! destructor
   ~minikdtree()
      {remove();}

   // definition of plane orientations
   enum {
      x_axis = 0,
      y_axis = 1,
      z_axis = 2,
      no_axis = -1
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

   // item point definition
   class ItemPoint {
      public:
      ItemPoint() {}
      ItemPoint(Item d, Vector3D p) {item=d; point=p;}
      Item item;
      Vector3D point;
   };

   // item point list definition
   typedef minidyna<ItemPoint> ItemPoints;

   protected:

   //! insert an item at a particular point into the kd-tree
   void insert(const Vector3D &point, const Item &item, Node **node, int axis = no_axis)
      {
      unsigned int level = 0;

      while (*node!=NULL)
         {
         if (isInLeftHalfSpace(point, (*node)->plane))
            node = &((*node)->leftSpace);
         else
            node = &((*node)->rightSpace);

         level++;
         }

      if (axis == no_axis)
         axis = level%3;

      *node = new Node;

      (*node)->item = item;
      (*node)->leftSpace = (*node)->rightSpace = NULL;

      (*node)->plane.point = point;
      (*node)->plane.orientation = axis;
      }

   public:

   const Node *getRoot()
      {return(root);}

   void insert(const Vector3D &point, const Item &item)
      {insert(point, item, &root);}

   protected:

   static BOOLINT xsortfunc(const ItemPoint &a, const ItemPoint &b) {return(a.point.x<b.point.x);}
   static BOOLINT ysortfunc(const ItemPoint &a, const ItemPoint &b) {return(a.point.y<b.point.y);}
   static BOOLINT zsortfunc(const ItemPoint &a, const ItemPoint &b) {return(a.point.z<b.point.z);}

   public:

   //! insert a list of item points into the kd-tree
   void insert(const ItemPoints &itempoints)
      {
      unsigned int s;

      s = itempoints.getsize();

      if (s>0)
         {
         unsigned int i;

         ItemPoints ips = itempoints;

         Vector3D pmin, pmax;

         pmin = pmax = ips[0].point;

         for (i=1; i<s; i++)
            {
            Vector3D p = ips[i].point;

            if (p.x<pmin.x) pmin.x = p.x;
            else if (p.x>pmax.x) pmax.x = p.x;

            if (p.y<pmin.y) pmin.y = p.y;
            else if (p.y>pmax.y) pmax.y = p.y;

            if (p.z<pmin.z) pmin.z = p.z;
            else if (p.z>pmax.z) pmax.z = p.z;
            }

         Vector3D pd = pmax-pmin;
         unsigned int axis;

         if (pd.x>pd.y)
            if (pd.x>pd.z)
               axis = x_axis;
            else
               axis = z_axis;
         else
            if (pd.y>pd.z)
               axis = y_axis;
            else
               axis = z_axis;

         unsigned int mid = (s-1)/2;

         switch (axis)
            {
            case x_axis:
               shellsort<ItemPoint>(ips, xsortfunc);
               while (mid>0 && ips[mid-1].point.x==ips[mid].point.x) mid--;
               break;
            case y_axis:
               shellsort<ItemPoint>(ips, ysortfunc);
               while (mid>0 && ips[mid-1].point.y==ips[mid].point.y) mid--;
               break;
            case z_axis:
               shellsort<ItemPoint>(ips, zsortfunc);
               while (mid>0 && ips[mid-1].point.z==ips[mid].point.z) mid--;
               break;
            }

         ItemPoints left, right;

         for (i=0; i<mid; i++)
            left.push(ips[i]);

         for (i=mid+1; i<s; i++)
            right.push(ips[i]);

         insert(ips[mid].point, ips[mid].item, &root, axis);
         ips.clear();

         insert(left);
         left.clear();

         insert(right);
         right.clear();
         }
      }

   protected:

   //! nearest neighbor search in kd-tree
   //!  in: 3D point
   //!  in: reference to starting node (the root node)
   //!  out: reference of node containing nearest point
   const Node *search(const Vector3D &point, const Node *node)
      {
      const Node *result = NULL;

      if (node!=NULL)
         {
         result = node;

         bool left = isInLeftHalfSpace(point, node->plane);

         // traverse into corresponding half space
         const Node *result2 = search(point, left? node->leftSpace : node->rightSpace);

         // update result if closer
         if (result2!=NULL)
            {
            double distance1 = getDistance(point, result->plane.point);
            double distance2 = getDistance(point, result2->plane.point);

            if (distance1>distance2)
               result = result2;
            }

         double distance1 = getDistance(point, result->plane.point);
         double distance2 = dabs(getDistance(point, node->plane));

         // check if other half space can yield a closer result
         if (distance1>distance2)
            {
            // traverse into other half space
            const Node *result2 = search(point, left? node->rightSpace : node->leftSpace);

            // update result if closer
            if (result2!=NULL)
               {
               double distance1 = getDistance(point, result->plane.point);
               double distance2 = getDistance(point, result2->plane.point);

               if (distance1>distance2)
                  result = result2;
               }
            }
         }

      return(result);
      }

   public:

   const Node *search(const Vector3D &point)
      {return(search(point, root));}

   protected:

   //! distance search in kd-tree
   //!  in: 3D center point
   //!  in: search radius around center
   //!  out: reference list of nodes within the search radius
   minidyna<const Node *> search(const Vector3D &point, double radius, const Node *node)
      {
      minidyna<const Node *> result;

      if (node!=NULL)
         {
         double distance = getDistance(node->plane.point, point);

         // append node if within search radius
         if (distance<radius)
            result.append(node);

         double intersection = getDistance(point, node->plane);

         // if search radius intersects left half space traverse into it
         if (intersection <= radius)
            result.append(search(point, radius, node->leftSpace));

         // if search radius intersects right half space traverse into it
         if (intersection >= -radius)
            result.append(search(point, radius, node->rightSpace));
         }

      return(result);
      }

   public:

   minidyna<const Node *> search(const Vector3D &point, double radius)
      {return(search(point, radius, root));}

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

   //! count maximum branch depth in subtree
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

   //! calculate average branch length in subtree
   double length(const Node *node)
      {
      if (node!=NULL)
         {
         unsigned int leftnum = items(node->leftSpace);
         unsigned int rightnum = items(node->rightSpace);

         if (leftnum==0 && rightnum==0) return(1);

         double left = depth(node->leftSpace);
         double right = depth(node->rightSpace);

         return((leftnum*left + rightnum*right)/(leftnum + rightnum) + 1);
         }

      return(0);
      }

   unsigned int length()
      {return(length(root));}

   protected:

   // get normal of plane
   //  the normal points into the right half space
   Vector3D getNormal(const Plane &plane)
      {
      Vector3D normal(0,0,0);

      switch (plane.orientation)
         {
         case x_axis: normal = Vector3D(1,0,0); break;
         case y_axis: normal = Vector3D(0,1,0); break;
         case z_axis: normal = Vector3D(0,0,1); break;
         }

      return(normal);
      }

   // euclidean distance of two points
   double getDistance(const Vector3D &point1, const Vector3D &point2)
      {
      Vector3D vec = point2-point1;

      return(sqrt(vec * vec));
      }

   // signed distance of a point to a plane
   //  negative values indicate position in left half space
   //  positive values indicate position in right half space
   double getDistance(const Vector3D &point, const Plane &plane)
      {
      Vector3D vec = point-plane.point;
      Vector3D normal = getNormal(plane);

      return(vec * normal);
      }

   // determines whether or not a point is in the left half space of a plane
   bool isInLeftHalfSpace(const Vector3D &point, const Plane &plane)
      {
      double distance = getDistance(point, plane);

      return(distance<0.0);
      }

   // reference to root node
   Node *root;
   };

#endif
