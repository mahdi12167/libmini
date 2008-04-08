// (c) by Stefan Roettger

#include "minimesh.h"

// default constructor
minibspt::minibspt() {}

// destructor
minibspt::~minibspt() {}

// insert from tetrahedral mesh
void minibspt::insert(const minimesh &mesh)
   {
   unsigned int i;

   minitet tet;

   // insert all tetraheda of the mesh into the bsp tree
   for (i=0; i<mesh.getsize(); i++)
      {
      tet=mesh[i];

      insert(tet.vtx[0],tet.vtx[1],tet.vtx[2],tet.vtx[3],tet.val[0]);
      insert(tet.vtx[0],tet.vtx[3],tet.vtx[1],tet.vtx[2],tet.val[0]);
      insert(tet.vtx[1],tet.vtx[3],tet.vtx[2],tet.vtx[0],tet.val[0]);
      insert(tet.vtx[2],tet.vtx[3],tet.vtx[0],tet.vtx[1],tet.val[0]);
      }
   }

// insert from triangular face
void minibspt::insert(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &p,const minitet::minival &val)
   {
   miniv3d pnt,nrm;
   minibspt_struct node;

   // set anchor point
   pnt=v1;

   // set anchor normal
   nrm=(v2-v1)/(v3-v1);
   nrm.normalize();

   // flip anchor normal to point inward
   if (nrm*(p-v1)<0.0) nrm=-nrm;

   // set anchor plane
   node.plane=minigeom_halfspace(pnt,nrm);

   // set data slot and coordinates
   node.val=val;

   // set children to be undefined
   node.left=node.right=0;

   // insert the node into the bsp tree
   insert(0,v1,v2,v3,node);
   }

// insert node
void minibspt::insert(unsigned int idx,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const minibspt_struct &node)
   {
   double d1,d2,d3;
   BOOLINT f1,f2,f3;

   // check if bsp tree is empty
   if (TREE.getsize()==0) TREE.append(node);
   else
      {
      // determine distance of triangle vertices from anchor plane
      d1=TREE[idx].plane.getdistance(v1);
      d2=TREE[idx].plane.getdistance(v2);
      d3=TREE[idx].plane.getdistance(v3);

      // check if the anchor plane is already existing in the bsp tree
      if (d1>-minigeom_base::delta && d1<minigeom_base::delta &&
          d2>-minigeom_base::delta && d2<minigeom_base::delta &&
          d3>-minigeom_base::delta && d3<minigeom_base::delta)
         if (node.val.slot==TREE[idx].val.slot) return;

      // calculate side indicator for each triangle vertex
      f1=(d1>-minigeom_base::delta);
      f2=(d2>-minigeom_base::delta);
      f3=(d3>-minigeom_base::delta);

      // check if the triangle intrudes into the left half space
      if (f1 || f2 || f3)
         if (TREE[idx].left!=0) insert(TREE[idx].left,v1,v2,v3,node); // insert recursively
         else
            {
            TREE.append(node); // insert as new child
            TREE[idx].left=TREE.getsize()-1; // link from parent
            }

      // check if the triangle intrudes into the right half space
      if (!f1 || !f2 || !f3)
         if (TREE[idx].right!=0) insert(TREE[idx].right,v1,v2,v3,node); // insert recursively
         else
            {
            TREE.append(node); // insert as new child
            TREE[idx].right=TREE.getsize()-1; // link from parent
            }
      }
   }

// extract to tetrahedral mesh
void minibspt::extract(minimesh &mesh)
   {
   mesh.setsize(0);

   intersect(0);
   }

// intersect bsp tree
void minibspt::intersect(unsigned int idx)
   {
   minigeom_plane plane;

   // check if bsp tree is empty
   if (TREE.getsize()>0)
      {
      // intersect left half space
      if (TREE[idx].left!=0)
         {
         TREE[TREE[idx].left].poly=TREE[idx].poly;
         TREE[TREE[idx].left].poly.intersect(TREE[idx].plane);
         intersect(TREE[idx].left);
         }

      // intersect right half space
      if (TREE[idx].right!=0)
         {
         plane=TREE[idx].plane;
         plane.invert();

         TREE[TREE[idx].right].poly=TREE[idx].poly;
         TREE[TREE[idx].right].poly.intersect(plane);
         intersect(TREE[idx].right);
         }
      }
   }
