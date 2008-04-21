// (c) by Stefan Roettger

#include "minimath.h"

#include "minimesh.h"

// default constructor
minimesh::minimesh(): minidyna<minihedron>() {}

// copy contructor
minimesh::minimesh(const minimesh &mesh): minidyna<minihedron>(mesh) {}

// copy contructor
minimesh::minimesh(const minidyna<minihedron> &mesh): minidyna<minihedron>(mesh) {}

// destructor
minimesh::~minimesh() {}

// append a tetrahdron
void minimesh::append(const minihedron &h)
   {minidyna<minihedron>::append(h);}

// append a tetrahedral mesh
void minimesh::append(const minimesh &m)
   {minidyna<minihedron>::append(m);}

// polygonize a set of line segments
minigon minimesh::polygonize(minigeom_segments segments) const
   {
   unsigned int i,j;

   miniv3d a,b,c,d;

   unsigned int idx;
   double dist,d1,d2;

   minigeom_segment tmp;

   minigon gon;

   if (segments.getsize()<3) return(gon);

   for (i=0; i<segments.getsize(); i++)
      {
      a=segments[i].getminpoint();
      b=segments[i].getmaxpoint();

      gon.append(a);

      idx=i;
      dist=MAXFLOAT;

      for (j=i+1; j<segments.getsize(); j++)
         {
         c=segments[j].getminpoint();
         d=segments[j].getmaxpoint();

         d1=(c-b).getlength2();
         d2=(d-b).getlength2();

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

   return(gon);
   }

// tetrahedralize a convex polyhedron
minimesh minimesh::tetrahedralize(const minigeom_polyhedron &poly) const
   {
   unsigned int i,j;

   minigon gon;

   miniv3d anchor,v1,v2,v3;

   minimesh mesh;

   if (poly.getnumhalfspace()==0) return(mesh);

   gon=polygonize(poly.getface(0));
   anchor=gon[0];

   for (i=1; i<poly.getnumhalfspace(); i++)
      {
      gon=polygonize(poly.getface(i));

      for (j=0; j+2<gon.getsize(); j++)
         {
         v1=gon[0];
         v2=gon[j+1];
         v3=gon[j+2];

         if (FABS(minigeom_plane(v1,v2,v3).getdistance(anchor))>minigeom_base::delta)
            mesh.append(minihedron(anchor,v1,v2,v3,minivals()));
         }
      }

   mesh.connect();

   return(mesh);
   }

// connect the faces of a tetrahedral mesh
void minimesh::connect()
   {
   unsigned int i;

   miniv3d v1,v2,v3,v4;

   // set dependencies for all tetrahedra
   for (i=0; i<getsize(); i++)
      {
      // get vertices of tetrahedron
      v1=get(i).vtx1;
      v2=get(i).vtx2;
      v3=get(i).vtx3;
      v4=get(i).vtx4;

      // search for face dependencies
      ref(i).dep123=getdep(v1,v2,v3,v4);
      ref(i).dep142=getdep(v1,v4,v2,v3);
      ref(i).dep243=getdep(v2,v4,v3,v1);
      ref(i).dep341=getdep(v3,v4,v1,v2);
      }
   }

// search for a face dependency
unsigned int minimesh::getdep(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &h) const
   {
   unsigned int i;

   miniv3d m,m1,m2,m3,m4;
   miniv3d p1,p2,p3,p4;

   // calculate face midpoint
   m=(v1+v2+v3)/3;

   // search all tetrahedra for face match
   for (i=0; i<getsize(); i++)
      {
      // get vertices of tetrahedron
      p1=get(i).vtx1;
      p2=get(i).vtx2;
      p3=get(i).vtx3;
      p4=get(i).vtx4;

      // calculate face midpoints
      m1=(p1+p2+p3)/3.0;
      m2=(p1+p4+p2)/3.0;
      m3=(p2+p4+p3)/3.0;
      m4=(p3+p4+p1)/3.0;

      // check for face and orientation match
      if ((m1-m).getlength2()<fsqr(minigeom_base::delta)) if (!minigeom_plane(p1,p2,p3,p4).isincl(h)) return(i);
      if ((m2-m).getlength2()<fsqr(minigeom_base::delta)) if (!minigeom_plane(p1,p4,p2,p3).isincl(h)) return(i);
      if ((m3-m).getlength2()<fsqr(minigeom_base::delta)) if (!minigeom_plane(p2,p4,p3,p1).isincl(h)) return(i);
      if ((m4-m).getlength2()<fsqr(minigeom_base::delta)) if (!minigeom_plane(p3,p4,p1,p2).isincl(h)) return(i);
      }

   return(0);
   }

// append a polyhedron
void minimesh::append(const minigeom_polyhedron &poly)
   {append(tetrahedralize(poly));}

// set embedded data values
void minimesh::setvals(const minivals &vals)
   {
   unsigned int i,j;

   miniv3d v1,v2,v3,v4;
   double b0,b1,b2,b3,b4;

   minival val;

   // apply the embedded data values to each tetrahedron
   for (i=0; i<getsize(); i++)
      {
      // set the embedded data values
      ref(i).vals=vals;

      // get the vertices of tetrahedron
      v1=get(i).vtx1;
      v2=get(i).vtx2;
      v3=get(i).vtx3;
      v4=get(i).vtx4;

      // for each embedded data value
      for (j=0; j<vals.getsize(); j++)
         {
         // get the embedded data value
         val=get(i).vals.get(j);

         // calculate the determinant of reference tetrahedron
         b0=getdet(val.ref1,val.ref2,val.ref3,val.ref4);

         // calculate barycentric coordinates #1
         b4=getdet(val.ref1,val.ref2,val.ref3,v1);
         b3=getdet(val.ref1,val.ref4,val.ref2,v1);
         b1=getdet(val.ref2,val.ref4,val.ref3,v1);
         b2=getdet(val.ref3,val.ref4,val.ref1,v1);

         // reconstruct data coordinate #1
         ref(i).vals.ref(j).crd1=(b1*val.crd1+b2*val.crd2+b3*val.crd3+b4*val.crd4)/b0;

         // calculate barycentric coordinates #2
         b4=getdet(val.ref1,val.ref2,val.ref3,v2);
         b3=getdet(val.ref1,val.ref4,val.ref2,v2);
         b1=getdet(val.ref2,val.ref4,val.ref3,v2);
         b2=getdet(val.ref3,val.ref4,val.ref1,v2);

         // reconstruct data coordinate #2
         ref(i).vals.ref(j).crd2=(b1*val.crd1+b2*val.crd2+b3*val.crd3+b4*val.crd4)/b0;

         // calculate barycentric coordinates #3
         b4=getdet(val.ref1,val.ref2,val.ref3,v3);
         b3=getdet(val.ref1,val.ref4,val.ref2,v3);
         b1=getdet(val.ref2,val.ref4,val.ref3,v3);
         b2=getdet(val.ref3,val.ref4,val.ref1,v3);

         // reconstruct data coordinate #3
         ref(i).vals.ref(j).crd3=(b1*val.crd1+b2*val.crd2+b3*val.crd3+b4*val.crd4)/b0;

         // calculate barycentric coordinates #4
         b4=getdet(val.ref1,val.ref2,val.ref3,v4);
         b3=getdet(val.ref1,val.ref4,val.ref2,v4);
         b1=getdet(val.ref2,val.ref4,val.ref3,v4);
         b2=getdet(val.ref3,val.ref4,val.ref1,v4);

         // reconstruct data coordinate #4
         ref(i).vals.ref(j).crd4=(b1*val.crd1+b2*val.crd2+b3*val.crd3+b4*val.crd4)/b0;
         }
      }
   }

// get determinant
double minimesh::getdet(const miniv3d &p,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3) const
   {
   miniv3d mtx[3];

   mtx[0]=miniv3d(v1-p);
   mtx[1]=miniv3d(v2-p);
   mtx[2]=miniv3d(v3-p);

   return(det_mtx(mtx));
   }

// sort a tetrahedral mesh with respect to the eye point
minimesh minimesh::sort(const miniv3d &eye)
   {
   unsigned int i;

   for (i=0; i<getsize(); i++) ref(i).visit=FALSE;

   // sort and append each tetrahedron to the output mesh
   SORT.setnull();
   descend(0,eye);

   return(SORT);
   }

// descend a tetrahedral mesh with respect to the eye point
void minimesh::descend(const unsigned int idx,const miniv3d &eye)
   {
   miniv3d v1,v2,v3,v4;
   unsigned int fd1,fd2,fd3,fd4;
   BOOLINT bf1,bf2,bf3,bf4;

   // check if the tetrahedral mesh is empty
   if (!isnull())
      {
      // check if the tetrahedron has been already visited
      if (get(idx).visit) return;

      // mark as already visited
      ref(idx).visit=TRUE;

      // get the vertices of tetrahedron
      v1=get(idx).vtx1;
      v2=get(idx).vtx2;
      v3=get(idx).vtx3;
      v4=get(idx).vtx4;

      // get the face dependencies
      fd1=get(idx).dep123;
      fd2=get(idx).dep142;
      fd3=get(idx).dep243;
      fd4=get(idx).dep341;

      // calculate the back faces
      bf1=bf2=bf3=bf4=FALSE;
      if (fd1!=0) bf1=minigeom_plane(v1,v2,v3,v4).isincl(eye);
      if (fd2!=0) bf2=minigeom_plane(v1,v4,v2,v3).isincl(eye);
      if (fd3!=0) bf3=minigeom_plane(v2,v4,v3,v1).isincl(eye);
      if (fd4!=0) bf4=minigeom_plane(v3,v4,v1,v2).isincl(eye);

      // descend to the dependencies of the back faces
      if (fd1!=0) if (bf1) descend(fd1,eye);
      if (fd2!=0) if (bf2) descend(fd2,eye);
      if (fd3!=0) if (bf3) descend(fd3,eye);
      if (fd4!=0) if (bf4) descend(fd4,eye);

      // append the actual tetrahedron to the sorted mesh
      SORT.append(get(idx));

      // descend to the dependencies of the front faces
      if (fd1!=0) if (!bf1) descend(fd1,eye);
      if (fd2!=0) if (!bf2) descend(fd2,eye);
      if (fd3!=0) if (!bf3) descend(fd3,eye);
      if (fd4!=0) if (!bf4) descend(fd4,eye);
      }
   }

// get volume
double minimesh::getvolume() const
   {
   unsigned int i;

   double vol;

   vol=0.0;

   for (i=0; i<getsize(); i++) vol+=get(i).getvolume();

   return(vol);
   }

// default constructor
minibsptree::minibsptree()
   {
   DONE=FALSE;
   GOTEYE=FALSE;
   VOLDONE=FALSE;

   PHASE=0;
   STEP=0;
   }

// destructor
minibsptree::~minibsptree() {}

// clear bsp tree
void minibsptree::clear()
   {
   MESH.setnull();
   TREE.setnull();

   DONE=FALSE;
   GOTEYE=FALSE;
   VOLDONE=FALSE;

   PHASE=0;
   STEP=0;
   }

// insert from tetrahedral mesh
void minibsptree::insert(const minimesh &mesh)
   {if (!DONE && PHASE==0) MESH.append(mesh);}

// preprocess input mesh one step at a time
BOOLINT minibsptree::preprocess()
   {
   unsigned int idx;

   if (!DONE)
      switch (PHASE)
         {
         case 0:
            // phase #0: calculate the swizzle constant
            if (MESH.getsize()>0)
               for (SWIZZLE=PRIME; gcd(MESH.getsize(),SWIZZLE)!=1; SWIZZLE+=2);
            else DONE=TRUE;

            PHASE++;

            break;
         case 1:
            // swizzle the actual position
            idx=(SWIZZLE*STEP)%MESH.getsize();

            // phase #1: insert each tetrahedron of the input mesh into the bsp tree
            insert1(idx);

            if (++STEP>=MESH.getsize())
               {
               STEP=0;
               PHASE++;
               }

            break;
         case 2:
            // phase #2: insert each tetrahedron of the input mesh into the bsp tree
            insert2(STEP);

            if (++STEP>=MESH.getsize())
               {
               STEP=0;
               PHASE++;
               }

            break;
         case 3:
            // phase #3: intersect each node of the bsp tree
            intersect(STEP);

            if (++STEP>=TREE.getsize())
               {
               STEP=0;
               PHASE++;
               }

            break;
         case 4:
            // phase #4: clean up
            MESH.setnull();

            DONE=TRUE;

            break;
         }

   return(DONE);
   }

// get status of preprocessing
BOOLINT minibsptree::getstatus()
   {return(DONE);}

// insert tetrahedron (phase #1)
void minibsptree::insert1(unsigned int idx)
   {
   minihedron h;

   // get the tetrahedron
   h=MESH[idx];

   // insert the tetrahedral faces as a dividing plane to the bsp tree
   insert(0,h.vtx1,h.vtx2,h.vtx3,h.vtx4);
   insert(0,h.vtx1,h.vtx4,h.vtx2,h.vtx3);
   insert(0,h.vtx2,h.vtx4,h.vtx3,h.vtx1);
   insert(0,h.vtx3,h.vtx4,h.vtx1,h.vtx2);
   }

// insert tetrahedron (phase #2)
void minibsptree::insert2(unsigned int idx)
   {
   minihedron h;

   // get the tetrahedron
   h=MESH[idx];

   // assign the data coordinates of the tetrahedron to the bsp tree
   setvals(0,h.vtx1,h.vtx2,h.vtx3,h.vtx4,h.vals);
   }

// insert tetrahedral face as a dividing plane
void minibsptree::insert(unsigned int idx,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &h)
   {
   double d1,d2,d3;

   // check if the bsp tree is empty
   if (TREE.isnull()) append(minigeom_plane(v1,v2,v3,h));
   else
      {
      // determine the distance of the tetrahedral face vertices from the actual dividing plane
      d1=TREE[idx].plane.getdistance(v1);
      d2=TREE[idx].plane.getdistance(v2);
      d3=TREE[idx].plane.getdistance(v3);

      // check if the dividing plane is already existing
      if (FABS(d1)>minigeom_base::delta || FABS(d2)>minigeom_base::delta || FABS(d3)>minigeom_base::delta)
         {
         // check if the tetrahedral face intrudes into the left half space
         if (d1>minigeom_base::delta || d2>minigeom_base::delta || d3>minigeom_base::delta)
            if (TREE[idx].left!=0) insert(TREE[idx].left,v1,v2,v3,h); // insert recursively
            else
               {
               append(minigeom_plane(v1,v2,v3,h)); // append node to bsp tree
               TREE[idx].left=TREE.getsize()-1; // link left node from parent
               }

         // check if the tetrahedral face intrudes into the right half space
         if (d1<-minigeom_base::delta || d2<-minigeom_base::delta || d3<-minigeom_base::delta)
            if (TREE[idx].right!=0) insert(TREE[idx].right,v1,v2,v3,h); // insert recursively
            else
               {
               append(minigeom_plane(v1,v2,v3,h)); // append node to bsp tree
               TREE[idx].right=TREE.getsize()-1; // link right node from parent
               }
         }
      }
   }

// assign data coordinates to tetrahedral nodes
void minibsptree::setvals(unsigned int idx,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &v4,const minivals &vals)
   {
   double d1,d2,d3,d4;

   // check if the bsp tree is empty
   if (!TREE.isnull())
      {
      // determine the distance of the tetrahedral vertices from the actual dividing plane
      d1=TREE[idx].plane.getdistance(v1);
      d2=TREE[idx].plane.getdistance(v2);
      d3=TREE[idx].plane.getdistance(v3);
      d4=TREE[idx].plane.getdistance(v4);

      // check if the tetrahedon intrudes into the left half space
      if (d1>minigeom_base::delta || d2>minigeom_base::delta || d3>minigeom_base::delta || d4>minigeom_base::delta)
         if (TREE[idx].left!=0) setvals(TREE[idx].left,v1,v2,v3,v4,vals); // insert recursively
         else TREE[idx].leftvals.append(vals); // append data coordinates

      // check if the tetrahedon intrudes into the right half space
      if (d1<-minigeom_base::delta || d2<-minigeom_base::delta || d3<-minigeom_base::delta || d4<-minigeom_base::delta)
         if (TREE[idx].right!=0) setvals(TREE[idx].right,v1,v2,v3,v4,vals); // insert recursively
         else TREE[idx].rightvals.append(vals); // append data coordinates
      }
   }

// append node to bsp tree
void minibsptree::append(const minigeom_plane &plane)
   {
   minibsptree_node node;

   node.plane=plane;
   node.left=node.right=0;

   TREE.append(node);
   }

// extract tetrahedral mesh
minimesh minibsptree::extract()
   {
   unsigned int i;

   minimesh mesh;

   // preprocess the input mesh
   while (!preprocess());

   // append each tetrahedralized node to the output mesh
   for (i=0; i<TREE.getsize(); i++)
      {
      if (TREE[i].left==0) mesh.append(TREE[i].leftmesh);
      if (TREE[i].right==0) mesh.append(TREE[i].rightmesh);
      }

   return(mesh);
   }

// extract sorted tetrahedral mesh
minimesh minibsptree::extract(const miniv3d &eye,const double radius)
   {
   // preprocess the input mesh
   while (!preprocess());

   if (GOTEYE)
      if (eye==EYE && radius==RADIUS) return(COLLECT); // return the previously collected mesh

   EYE=eye;
   RADIUS=radius;
   GOTEYE=TRUE;

   // sort and append each tetrahedralized node to the output mesh
   COLLECT.setnull();
   collect(0);

   return(COLLECT);
   }

// intersect one node of the bsp tree
void minibsptree::intersect(unsigned int idx)
   {
   minigeom_plane plane;
   minigeom_polyhedron poly;

   // intersect the left half space
   if (TREE[idx].left!=0)
      {
      // add the actual half space to the left child's polyhedron
      TREE[TREE[idx].left].poly=TREE[idx].poly;
      TREE[TREE[idx].left].poly.intersect(TREE[idx].plane);
      }

   // intersect the right half space
   if (TREE[idx].right!=0)
      {
      // invert the actual half space
      plane=TREE[idx].plane;
      plane.invert();

      // add the actual inverted half space to the right child's polyhedron
      TREE[TREE[idx].right].poly=TREE[idx].poly;
      TREE[TREE[idx].right].poly.intersect(plane);
      }

   // break the left half space into a set of connected tetrahedra
   if (TREE[idx].left==0 && TREE[idx].leftvals.getsize()>0)
      {
      // add the actual half space to the node's polyhedron
      poly=TREE[idx].poly;
      poly.intersect(TREE[idx].plane);

      // calculate the connected tetrahedra
      TREE[idx].leftmesh.append(poly);

      // propagate the embedded data coordinates
      TREE[idx].leftmesh.setvals(TREE[idx].leftvals);
      TREE[idx].leftvals.setnull();
      }

   // break the right half space into a set of connected tetrahedra
   if (TREE[idx].right==0 && TREE[idx].rightvals.getsize()>0)
      {
      // invert the actual half space
      plane=TREE[idx].plane;
      plane.invert();

      // add the actual inverted half space to the node's polyhedron
      poly=TREE[idx].poly;
      poly.intersect(plane);

      // calculate the connected tetrahedra
      TREE[idx].rightmesh.append(poly);

      // propagate the embedded data coordinates
      TREE[idx].rightmesh.setvals(TREE[idx].rightvals);
      TREE[idx].rightvals.setnull();
      }

   // clear the actual polyhedron
   TREE[idx].poly.clear();
   }

// collect tetrahedra by descending the bsp tree
// also sort tetrahedra with respect to the eye point
// also cull tetrahedra with respect to the visibility radius
void minibsptree::collect(const unsigned int idx)
   {
   double dist;

   // check if the bsp tree is empty
   if (!TREE.isnull())
      {
      // calculate the distance of the eye point to the dividing plane
      dist=TREE[idx].plane.getdistance(EYE);

      // check which half space includes the eye point
      if (dist<0.0)
         {
         // collect the left half space
         if (TREE[idx].left==0) COLLECT.append(TREE[idx].leftmesh.sort(EYE)); // sort left mesh
         else if (dist+RADIUS>0.0) collect(TREE[idx].left);

         // collect the right half space
         if (TREE[idx].right==0) COLLECT.append(TREE[idx].rightmesh.sort(EYE)); // sort right mesh
         else collect(TREE[idx].right);
         }
      else
         {
         // collect the right half space
         if (TREE[idx].right==0) COLLECT.append(TREE[idx].rightmesh.sort(EYE)); // sort right mesh
         else collect(TREE[idx].right);

         // collect the left half space
         if (TREE[idx].left==0) COLLECT.append(TREE[idx].leftmesh.sort(EYE)); // sort left mesh
         else if (dist+RADIUS>0.0) collect(TREE[idx].left);
         }
      }
   }

// get volume of tetrahedralized polyhedra
double minibsptree::getvolume()
   {
   unsigned int i;

   // preprocess the input mesh
   while (!preprocess());

   if (VOLDONE) return(VOL);

   VOL=0.0;

   // get volume of each tetrahedralized node of the output mesh
   for (i=0; i<TREE.getsize(); i++)
      {
      if (TREE[i].left==0) VOL+=TREE[i].leftmesh.getvolume();
      if (TREE[i].right==0) VOL+=TREE[i].rightmesh.getvolume();
      }

   VOLDONE=TRUE;

   return(VOL);
   }
