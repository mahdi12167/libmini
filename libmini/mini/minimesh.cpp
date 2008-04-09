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

   minihedron h;

   if (DONE) ERRORMSG();

   // insert all tetraheda of the mesh into the bsp tree
   for (i=0; i<mesh.getsize(); i++)
      {
      h=mesh[i];

      insert(h.vtx1,h.vtx2,h.vtx3,h.vtx4,minivals());
      insert(h.vtx1,h.vtx4,h.vtx2,h.vtx3,minivals());
      insert(h.vtx2,h.vtx4,h.vtx3,h.vtx1,minivals());
      insert(h.vtx3,h.vtx4,h.vtx1,h.vtx2,h.vals);
      }
   }

// insert from triangular face
void minibsptree::insert(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &p,const minivals &vals)
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
   insert(0,v1,v2,v3,vals,plane);
   }

// insert node
void minibsptree::insert(unsigned int idx,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const minivals &vals,const minigeom_plane &plane)
   {
   minibsptree_node node;

   double d1,d2,d3;
   BOOLINT f1,f2,f3;

   // check if bsp tree is empty
   if (TREE.isnull())
      {
      node.plane=plane;
      node.left=node.right=0;

      node.vals=vals;

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
         TREE[idx].vals.append(vals);
         return;
         }

      // calculate side indicator for each triangle vertex
      f1=(d1>-minigeom_base::delta);
      f2=(d2>-minigeom_base::delta);
      f3=(d3>-minigeom_base::delta);

      // check if the triangle intrudes into the left half space
      if (f1 || f2 || f3)
         if (TREE[idx].left!=0) insert(TREE[idx].left,v1,v2,v3,vals,plane); // insert recursively
         else
            {
            node.plane=plane;
            node.left=node.right=0;

            node.vals=vals;

            TREE.append(node); // insert as new child
            TREE[idx].left=TREE.getsize()-1; // link from parent
            }

      // check if the triangle intrudes into the right half space
      if (!f1 || !f2 || !f3)
         if (TREE[idx].right!=0) insert(TREE[idx].right,v1,v2,v3,vals,plane); // insert recursively
         else
            {
            node.plane=plane;
            node.left=node.right=0;

            node.vals=vals;

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

   mesh.setnull(); //!!
   }

// extract to sorted tetrahedral mesh
void minibsptree::extract(minimesh &mesh,const miniv3d &eye)
   {
   if (!DONE)
      {
      intersect(0);
      DONE=TRUE;
      }

   mesh.setnull(); //!!
   }

// intersect bsp tree
void minibsptree::intersect(unsigned int idx)
   {
   minigeom_plane plane;

   // check if bsp tree is empty
   if (!TREE.isnull())
      {
      // append slots to polyhedron
      TREE[idx].poly.getvals().append(TREE[idx].vals);

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
         TREE[idx].vals.setnull();
         tetrahedralize(TREE[idx].poly,TREE[idx].mesh);
         TREE[idx].poly.clear();
         }
      }
   }

// polygonize a set of line segments
void polygonize(const minidyna<minigeom_segment> &segments,minigon &gon)
   {
   unsigned int i,j;

   miniv3d a,b,c,d;

   unsigned int idx;
   double dist,d1,d2;

   minigeom_segment tmp;

   gon.setnull();

   if (segments.getsize()<3) return;

   for (i=0; i<segments.getsize(); i++)
      {
      a=segments[i].getpoint(segments[i].getminlambda());
      b=segments[i].getpoint(segments[i].getmaxlambda());

      gon.append(a);

      idx=i;
      dist=MAXFLOAT;

      for (j=i+1; j<segments.getsize(); j++)
         {
         c=segments[j].getpoint(segments[j].getminlambda());
         d=segments[j].getpoint(segments[j].getmaxlambda());

         d1=(c-b).getLength();
         d2=(d-b).getLength();

         if (d1<dist)
            {
            idx=j;
            dist=d1;
            }

         if (d2<dist)
            {
            idx=j;
            dist=d2;

            segments[j].swap();
            }
         }

      if (i+2<segments.getsize())
         {
         tmp=segments[i+1];
         segments[i+1]=segments[idx];
         segments[idx]=tmp;
         }
      }
   }

// tetrahedralize a convex polyhedron
void minibsptree::tetrahedralize(const minigeom_polyhedron &poly,minimesh &mesh)
   {
   unsigned int i,j;

   minidyna<minigeom_segment> segments;
   minigon gon;

   miniv3d anchor,v1,v2,v3;

   mesh.setnull();

   segments=poly.getface(0);
   anchor=segments[0].getpoint(segments[0].getminlambda());

   for (i=1; i<poly.getnumhalfspace(); i++)
      {
      segments=poly.getface(i);
      polygonize(segments,gon);

      for (j=0; j+2<gon.getsize(); j++)
         {
         v1=gon[j];
         v2=gon[j+1];
         v3=gon[j+2];

         if (minigeom_plane(v1,v2,v3).getdistance(anchor)>minigeom_base::delta)
            mesh.append(minihedron(anchor,v1,v2,v3,poly.getvals()));
         }
      }
   }
