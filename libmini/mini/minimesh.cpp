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
void minibsptree::insert(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &h,const minivals &vals)
   {insert(0,v1,v2,v3,vals,minigeom_plane(v1,v2,v3,h));}

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
   unsigned int i;

   if (!DONE)
      {
      intersect(0);
      DONE=TRUE;
      }

   mesh.setnull();

   for (i=0; i<TREE.getsize(); i++)
      if (TREE[i].left==0 && TREE[i].right==0)
         mesh.append(TREE[i].mesh);
   }

// extract to sorted tetrahedral mesh
void minibsptree::extract(const miniv3d &eye,minimesh &mesh)
   {
   if (!DONE)
      {
      intersect(0);
      DONE=TRUE;
      }

   COLLECT.setnull();
   collect(0,eye);

   mesh=COLLECT;
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

      // break polyhedral leaves into a set of connected tetrahedra
      if (TREE[idx].left==0 && TREE[idx].right==0)
         {
         TREE[idx].vals.setnull();
         tetrahedralize(TREE[idx].poly,TREE[idx].mesh);
         connect(TREE[idx].mesh);
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

         d1=(c-b).getlength();
         d2=(d-b).getlength();

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
         v1=gon[0];
         v2=gon[j+1];
         v3=gon[j+2];

         if (minigeom_plane(v1,v2,v3).getdistance(anchor)>minigeom_base::delta)
            mesh.append(minihedron(anchor,v1,v2,v3,poly.getvals()));
         }
      }
   }

// connect the faces of a tetrahedral mesh
void minibsptree::connect(minimesh &mesh)
   {
   unsigned int i;

   miniv3d v1,v2,v3,v4;
   miniv3d nrm;

   // set dependencies for all tetrahedra
   for (i=0; i<mesh.getsize(); i++)
      {
      // get vertices of tetrahedron
      v1=mesh[i].vtx1;
      v2=mesh[i].vtx1;
      v3=mesh[i].vtx1;
      v4=mesh[i].vtx1;

      // search for face dependencies
      mesh[i].dep123=getdep(v1,v2,v3,v4,mesh);
      mesh[i].dep142=getdep(v1,v4,v2,v3,mesh);
      mesh[i].dep243=getdep(v2,v4,v3,v1,mesh);
      mesh[i].dep341=getdep(v3,v4,v1,v2,mesh);
      }
   }

// search for a face dependency
unsigned int minibsptree::getdep(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &h,const minimesh &mesh)
   {
   unsigned int i;

   miniv3d m;
   miniv3d p1,p2,p3,p4;
   miniv3d m1,m2,m3,m4;

   // calculate face midpoint
   m=(v1+v2+v3)/3;

   // search all tetrahedra
   for (i=0; i<mesh.getsize(); i++)
      {
      // get vertices of tetrahedron
      p1=mesh[i].vtx1;
      p2=mesh[i].vtx1;
      p3=mesh[i].vtx1;
      p4=mesh[i].vtx1;

      // calculate face midpoints
      m1=(p1+p2+p3)/3;
      m2=(p1+p4+p2)/3;
      m3=(p2+p4+p3)/3;
      m4=(p3+p4+p1)/3;

      // check for face and orientation match
      if ((m1-m).getlength()<minigeom_base::delta) if (!minigeom_plane(p1,p2,p3,p4).isincl(h)) return(i);
      if ((m2-m).getlength()<minigeom_base::delta) if (!minigeom_plane(p1,p4,p2,p3).isincl(h)) return(i);
      if ((m3-m).getlength()<minigeom_base::delta) if (!minigeom_plane(p2,p4,p3,p1).isincl(h)) return(i);
      if ((m4-m).getlength()<minigeom_base::delta) if (!minigeom_plane(p3,p4,p1,p2).isincl(h)) return(i);
      }

   return(0);
   }

// sort a tetrahedral mesh with respect to the eye point
void minibsptree::sort(const unsigned int idx,const miniv3d &eye,minimesh &mesh)
   {
   unsigned int i;

   for (i=0; i<TREE[idx].mesh.getsize(); i++) TREE[idx].mesh[i].flag=FALSE;

   SORT.setnull();

   descend(idx,0,eye);

   mesh=SORT;
   }

// descend a tetrahedral mesh with respect to the eye point
void minibsptree::descend(const unsigned int idx,const unsigned int h,const miniv3d &eye)
   {
   miniv3d v1,v2,v3,v4;
   unsigned int fd1,fd2,fd3,fd4;
   BOOLINT bf1,bf2,bf3,bf4;

   // check if already visited
   if (TREE[idx].mesh[h].flag) return;

   // mark as already visited
   TREE[idx].mesh[h].flag=TRUE;

   // get vertices of tetrahedron
   v1=TREE[idx].mesh[h].vtx1;
   v2=TREE[idx].mesh[h].vtx2;
   v3=TREE[idx].mesh[h].vtx3;
   v4=TREE[idx].mesh[h].vtx4;

   // get face dependencies
   fd1=TREE[idx].mesh[h].dep123;
   fd2=TREE[idx].mesh[h].dep142;
   fd3=TREE[idx].mesh[h].dep243;
   fd4=TREE[idx].mesh[h].dep341;

   // calculate back faces
   bf1=bf2=bf3=bf4=FALSE;
   if (fd1!=0) bf1=minigeom_plane(v1,v2,v3,v4).isincl(eye);
   if (fd2!=0) bf2=minigeom_plane(v1,v4,v2,v3).isincl(eye);
   if (fd3!=0) bf3=minigeom_plane(v2,v4,v3,v1).isincl(eye);
   if (fd4!=0) bf4=minigeom_plane(v3,v4,v1,v2).isincl(eye);

   // descend to back faces
   if (fd1!=0) if (bf1) descend(idx,fd1,eye);
   if (fd2!=0) if (bf2) descend(idx,fd2,eye);
   if (fd3!=0) if (bf3) descend(idx,fd3,eye);
   if (fd4!=0) if (bf4) descend(idx,fd4,eye);

   // append actual tetrahedron to sorted mesh
   SORT.append(TREE[idx].mesh[h]);

   // descend to front faces
   if (fd1!=0) if (!bf1) descend(idx,fd1,eye);
   if (fd2!=0) if (!bf2) descend(idx,fd2,eye);
   if (fd3!=0) if (!bf3) descend(idx,fd3,eye);
   if (fd4!=0) if (!bf4) descend(idx,fd4,eye);
   }

// collect tetrahedra by descending the bsp tree with respect to the eye point
void minibsptree::collect(const unsigned int idx,const miniv3d &eye)
   {
   // check if bsp tree is empty
   if (!TREE.isnull())
      if (TREE[idx].left==0 && TREE[idx].right==0)
         {
         sort(idx,eye,TREE[idx].mesh); // sort leave node
         COLLECT.append(TREE[idx].mesh); // append leave node
         }
      else
         if (!TREE[idx].plane.isincl(eye)) // check which half space includes the eye point
            {
            // collect left half space
            if (TREE[idx].left!=0) collect(TREE[idx].left,eye);

            // collect right half space
            if (TREE[idx].right!=0) collect(TREE[idx].right,eye);
            }
         else
            {
            // collect right half space
            if (TREE[idx].right!=0) collect(TREE[idx].right,eye);

            // collect left half space
            if (TREE[idx].left!=0) collect(TREE[idx].left,eye);
            }
   }
