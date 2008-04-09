// (c) by Stefan Roettger

#include "minimesh.h"

// default constructor
minibsptree::minibsptree()
   {DONE=FALSE;}

// destructor
minibsptree::~minibsptree() {}

// clear bsp tree
void minibsptree::clear()
   {
   TREE.setnull();
   DONE=FALSE;
   }

// insert from tetrahedral mesh
void minibsptree::insert(const minimesh &mesh)
   {
   unsigned int i;

   minitet tet;

   if (DONE) ERRORMSG();

   // insert all tetraheda of the mesh into the bsp tree
   for (i=0; i<mesh.getsize(); i++)
      {
      tet=mesh[i];

      insert(tet.vtx1,tet.vtx2,tet.vtx3,tet.vtx4,minival_array());
      insert(tet.vtx1,tet.vtx4,tet.vtx2,tet.vtx3,minival_array());
      insert(tet.vtx2,tet.vtx4,tet.vtx3,tet.vtx1,minival_array());
      insert(tet.vtx3,tet.vtx4,tet.vtx1,tet.vtx2,tet.val);
      }
   }

// insert from triangular face
void minibsptree::insert(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &p,const minival_array &val)
   {
   miniv3d pnt,nrm;
   minigeom_plane plane;

   // set anchor point
   pnt=v1;

   // set anchor normal
   nrm=(v2-v1)/(v3-v1);
   nrm.normalize();

   // flip anchor normal to point inward
   if (nrm*(p-v1)<0.0) nrm=-nrm;

   // set anchor plane
   plane=minigeom_plane(pnt,nrm);

   // insert the plane into the bsp tree
   insert(0,v1,v2,v3,val,plane);
   }

// insert node
void minibsptree::insert(unsigned int idx,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const minival_array &val,const minigeom_plane &plane)
   {
   minibspt_node node;

   double d1,d2,d3;
   BOOLINT f1,f2,f3;

   // check if bsp tree is empty
   if (TREE.isnull())
      {
      node.plane=plane;
      node.left=node.right=0;

      node.val=val;

      TREE.append(node);
      }
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
         {
         TREE[idx].val.append(val);
         return;
         }

      // calculate side indicator for each triangle vertex
      f1=(d1>-minigeom_base::delta);
      f2=(d2>-minigeom_base::delta);
      f3=(d3>-minigeom_base::delta);

      // check if the triangle intrudes into the left half space
      if (f1 || f2 || f3)
         if (TREE[idx].left!=0) insert(TREE[idx].left,v1,v2,v3,val,plane); // insert recursively
         else
            {
            node.plane=plane;
            node.left=node.right=0;

            node.val=val;

            TREE.append(node); // insert as new child
            TREE[idx].left=TREE.getsize()-1; // link from parent
            }

      // check if the triangle intrudes into the right half space
      if (!f1 || !f2 || !f3)
         if (TREE[idx].right!=0) insert(TREE[idx].right,v1,v2,v3,val,plane); // insert recursively
         else
            {
            node.plane=plane;
            node.left=node.right=0;

            node.val=val;

            TREE.append(node); // insert as new child
            TREE[idx].right=TREE.getsize()-1; // link from parent
            }
      }
   }

// extract to tetrahedral mesh
void minibsptree::extract(minimesh &mesh)
   {
   if (!DONE)
      {
      intersect(0);
      DONE=TRUE;
      }

   mesh.setnull();
   }

// extract to polyhedral tree
void minibsptree::extract(minibspt &tree)
   {
   if (!DONE)
      {
      intersect(0);
      DONE=TRUE;
      }

   tree.setnull();
   }

// intersect bsp tree
void minibsptree::intersect(unsigned int idx)
   {
   minigeom_plane plane;

   // check if bsp tree is empty
   if (!TREE.isnull())
      {
      // append slots to polyhedron
      TREE[idx].poly.getval().append(TREE[idx].val);

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

      // break polyhedral leaves into tetrahedra
      if (TREE[idx].left==0 && TREE[idx].right==0)
         {
         TREE[idx].val.setnull();
         tetrahedrize(TREE[idx].poly,TREE[idx].mesh);
         }
      }
   }

// tetrahedrize a convex polyhedron
void minibsptree::tetrahedrize(minigeom_polyhedron &poly,minimesh &mesh)
   {
   poly.clear();
   mesh.setnull();
   }
