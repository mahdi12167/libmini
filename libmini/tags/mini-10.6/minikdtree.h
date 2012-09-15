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
      {
      root = NULL;
      bbox = FALSE;
      }

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

   // definition of kdtree plane
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
         if ((*node)->plane.point==point) return;

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

      if (!bbox)
         {
         bboxmin = bboxmax = point;
         bbox = TRUE;
         }
      else
         {
         if (point.x<bboxmin.x) bboxmin.x=point.x;
         else if (point.x>bboxmax.x) bboxmax.x=point.x;

         if (point.y<bboxmin.y) bboxmin.y=point.y;
         else if (point.y>bboxmax.y) bboxmax.y=point.y;

         if (point.z<bboxmin.z) bboxmin.z=point.z;
         else if (point.z>bboxmax.z) bboxmax.z=point.z;
         }
      }

   public:

   void insert(const Vector3D &point, const Item &item)
      {insert(point, item, &root);}

   //! get root node ref
   const Node *getRoot()
      {return(root);}

   //! get bounding box of inserted items
   BOOLINT getBBox(Vector3D &bmin, Vector3D &bmax)
      {
      if (bbox)
         {
         bmin = bboxmin;
         bmax = bboxmax;

         return(TRUE);
         }

      return(FALSE);
      }

   //! get barycenter of inserted items
   miniv3d getcenter()
      {
      miniv3d center;

      if (bbox) center = 0.5*(bboxmin+bboxmax);

      return(center);
      }

   //! get radius of inserted items
   double getradius()
      {
      double radius=0.0;

      if (bbox) radius = 0.5*(bboxmax-bboxmin).length();

      return(radius);
      }

   //! normalize point /wrt bbox
   Vector3D normalize(const Vector3D &p)
      {
      miniv3d v=p;

      if (bbox)
         {
         if (bboxmin.x<bboxmax.x) v.x = (v.x-bboxmin.x)/(bboxmax.x-bboxmin.x);
         if (bboxmin.y<bboxmax.y) v.y = (v.y-bboxmin.y)/(bboxmax.y-bboxmin.y);
         if (bboxmin.z<bboxmax.z) v.z = (v.z-bboxmin.z)/(bboxmax.z-bboxmin.z);
         }

      return(v);
      }

   //! denormalize point /wrt bbox
   Vector3D denormalize(const Vector3D &p)
      {
      miniv3d v=p;

      if (bbox)
         {
         v.x = v.x*(bboxmax.x-bboxmin.x)+bboxmin.x;
         v.y = v.y*(bboxmax.y-bboxmin.y)+bboxmin.y;
         v.z = v.z*(bboxmax.z-bboxmin.z)+bboxmin.z;
         }

      return(v);
      }

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

         // determine bbox of item point list
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

         // determine axis with largest extent
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

         // determine median along selected axis
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

         // create left item point list
         for (i=0; i<mid; i++)
            left.push(ips[i]);

         // create right item point list
         for (i=mid+1; i<s; i++)
            right.push(ips[i]);

         // insert median item point
         insert(ips[mid].point, ips[mid].item, &root, axis);
         ips.clear();

         // insert left item point list
         insert(left);
         left.clear();

         // insert right item point list
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

   protected:

   //! test search in kd-tree
   //!  in: 3D center point
   //!  in: search radius around center
   //!  out: tree contains points within the search radius?
   BOOLINT test(const Vector3D &point, double radius, const Node *node)
      {
      if (node!=NULL)
         {
         double distance = getDistance(node->plane.point, point);

         // actual node is within search radius?
         if (distance<radius)
            return(TRUE);

         bool left = isInLeftHalfSpace(point, node->plane);
         double intersection = getDistance(point, node->plane);

         if (left)
            {
            // if search radius intersects left half space traverse into it
            if (intersection <= radius)
               if (test(point, radius, node->leftSpace))
                  return(TRUE);

            // if search radius intersects right half space traverse into it
            if (intersection >= -radius)
               if (test(point, radius, node->rightSpace))
                  return(TRUE);
            }
         else
            {
            // if search radius intersects right half space traverse into it
            if (intersection >= -radius)
               if (test(point, radius, node->rightSpace))
                  return(TRUE);

            // if search radius intersects left half space traverse into it
            if (intersection <= radius)
               if (test(point, radius, node->leftSpace))
                  return(TRUE);
            }
         }

      return(FALSE);
      }

   public:

   BOOLINT test(const Vector3D &point, double radius)
      {return(search(point, radius, root));}

   protected:

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

   public:

   void remove()
      {
      remove(&root);
      bbox = FALSE;
      }

   //! count items in subtree
   unsigned int items(const Node *node)
      {
      if (node!=NULL)
         return(items(node->leftSpace) + items(node->rightSpace) + 1);

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

         double left = length(node->leftSpace);
         double right = length(node->rightSpace);

         return((leftnum*left + rightnum*right)/(leftnum + rightnum) + 1);
         }

      return(0);
      }

   double length()
      {return(length(root));}

   // get normal of plane
   //  the normal points into the right half space
   static Vector3D getNormal(const Plane &plane)
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

   // get normal of node
   static Vector3D getNormal(const Node *node)
      {return(getNormal(node->plane));}

   // get position of node
   static Vector3D getPosition(const Node *node)
      {return(node->plane.point);}

   // get item of node
   static Item getItem(const Node *node)
      {return(node->item);}

   // euclidean distance of two points
   static double getDistance(const Vector3D &point1, const Vector3D &point2)
      {
      Vector3D vec = point2-point1;

      return(sqrt(vec * vec));
      }

   // signed distance of a point to a plane
   //  negative values indicate position in left half space
   //  positive values indicate position in right half space
   static double getDistance(const Vector3D &point, const Plane &plane)
      {
      Vector3D vec = point-plane.point;
      Vector3D normal = getNormal(plane);

      return(vec * normal);
      }

   // determines whether or not a point is in the left half space of a plane
   static bool isInLeftHalfSpace(const Vector3D &point, const Plane &plane)
      {
      double distance = getDistance(point, plane);

      return(distance<0.0);
      }

   // determines whether or not a point is in the left half space of a node
   static bool isInLeftHalfSpace(const Vector3D &point, const Node *node)
      {return(isInLeftHalfSpace(point, node->plane));}

   protected:

   // reference to root node
   Node *root;

   // bounding box
   BOOLINT bbox;
   Vector3D bboxmin, bboxmax;
   };

#endif
