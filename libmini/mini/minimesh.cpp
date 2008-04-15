// (c) by Stefan Roettger

#include "minimesh.h"

// default constructor
minimesh::minimesh(): minidyna<minihedron>() {}

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

      // get vertices of tetrahedron
      v1=get(i).vtx1;
      v2=get(i).vtx2;
      v3=get(i).vtx3;
      v4=get(i).vtx4;

      // for each embedded data value
      for (j=0; j<vals.getsize(); j++)
         {
         // get embedded data value
         val=get(i).vals.get(j);

         // calculate determinant of reference tetrahedron
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

   // check if already visited
   if (get(idx).visit) return;

   // mark as already visited
   ref(idx).visit=TRUE;

   // get vertices of tetrahedron
   v1=get(idx).vtx1;
   v2=get(idx).vtx2;
   v3=get(idx).vtx3;
   v4=get(idx).vtx4;

   // get face dependencies
   fd1=get(idx).dep123;
   fd2=get(idx).dep142;
   fd3=get(idx).dep243;
   fd4=get(idx).dep341;

   // calculate back faces
   bf1=bf2=bf3=bf4=FALSE;
   if (fd1!=0) bf1=minigeom_plane(v1,v2,v3,v4).isincl(eye);
   if (fd2!=0) bf2=minigeom_plane(v1,v4,v2,v3).isincl(eye);
   if (fd3!=0) bf3=minigeom_plane(v2,v4,v3,v1).isincl(eye);
   if (fd4!=0) bf4=minigeom_plane(v3,v4,v1,v2).isincl(eye);

   // descend to back faces
   if (fd1!=0) if (bf1) descend(fd1,eye);
   if (fd2!=0) if (bf2) descend(fd2,eye);
   if (fd3!=0) if (bf3) descend(fd3,eye);
   if (fd4!=0) if (bf4) descend(fd4,eye);

   // append actual tetrahedron to sorted mesh
   SORT.append(get(idx));

   // descend to front faces
   if (fd1!=0) if (!bf1) descend(fd1,eye);
   if (fd2!=0) if (!bf2) descend(fd2,eye);
   if (fd3!=0) if (!bf3) descend(fd3,eye);
   if (fd4!=0) if (!bf4) descend(fd4,eye);
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
   }

// destructor
minibsptree::~minibsptree() {}

// clear bsp tree
void minibsptree::clear()
   {
   TREE.setnull();

   DONE=FALSE;
   GOTEYE=FALSE;
   VOLDONE=FALSE;
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

// insert triangular face into bsp tree
void minibsptree::insert(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &h,const minivals &vals)
   {insert(0,v1,v2,v3,vals,minigeom_plane(v1,v2,v3,h));}

// insert dividing plane into bsp tree
void minibsptree::insert(unsigned int idx,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const minivals &vals,const minigeom_plane &plane)
   {
   double d1,d2,d3;

   // check if bsp tree is empty
   if (TREE.isnull()) append(vals,plane);
   else
      {
      // determine distance of triangle vertices from dividing plane
      d1=TREE[idx].plane.getdistance(v1);
      d2=TREE[idx].plane.getdistance(v2);
      d3=TREE[idx].plane.getdistance(v3);

      // check if the dividing plane is coplanar
      if (FABS(d1)<=minigeom_base::delta && FABS(d2)<=minigeom_base::delta && FABS(d3)<=minigeom_base::delta)
         if (!vals.isnull())
            // check plane orientation
            if (TREE[idx].plane.isequal(plane))
               if (TREE[idx].left!=0) insert(TREE[idx].left,v1,v2,v3,vals,plane); // insert recursively
               else TREE[idx].left=append(vals,plane); // link left node
            else
               if (TREE[idx].right!=0) insert(TREE[idx].right,v1,v2,v3,vals,plane); // insert recursively
               else TREE[idx].right=append(vals,plane); // link right node
         else return;
      else
         {
         // check if the triangle intrudes into the left half space
         if (d1>minigeom_base::delta || d2>minigeom_base::delta || d3>minigeom_base::delta)
            if (TREE[idx].left!=0) insert(TREE[idx].left,v1,v2,v3,vals,plane); // insert recursively
            else TREE[idx].left=append(vals,plane); // link left node

         // check if the triangle intrudes into the right half space
         if (d1<-minigeom_base::delta || d2<-minigeom_base::delta || d3<-minigeom_base::delta)
            if (TREE[idx].right!=0) insert(TREE[idx].right,v1,v2,v3,vals,plane); // insert recursively
            else TREE[idx].right=append(vals,plane); // link right node
         }
      }
   }

// append node to bsp tree
unsigned int minibsptree::append(const minivals &vals,const minigeom_plane &plane)
   {
   minibsptree_node node;

   node.plane=plane;
   node.left=node.right=0;

   node.vals=vals;

   TREE.append(node);

   return(TREE.getsize()-1);
   }

// extract tetrahedral mesh
minimesh minibsptree::extract()
   {
   unsigned int i;

   minimesh mesh;

   if (!DONE)
      {
      intersect(0);
      DONE=TRUE;
      }

   for (i=0; i<TREE.getsize(); i++)
      if (TREE[i].left==0 && TREE[i].right==0)
         mesh.append(TREE[i].mesh);

   return(mesh);
   }

// extract sorted tetrahedral mesh
minimesh minibsptree::extract(const miniv3d &eye,const double radius)
   {
   if (!DONE)
      {
      intersect(0);
      DONE=TRUE;
      }

   if (GOTEYE)
      if (eye==EYE && radius==RADIUS) return(COLLECT); // return the previously collected mesh

   EYE=eye;
   RADIUS=radius;
   GOTEYE=TRUE;

   COLLECT.setnull();
   collect(0);

   return(COLLECT);
   }

// intersect bsp tree
void minibsptree::intersect(unsigned int idx)
   {
   minigeom_plane plane;

   // check if bsp tree is empty
   if (!TREE.isnull())
      {
      // intersect left half space
      if (TREE[idx].left!=0)
         {
         // propagate embedded data values
         TREE[TREE[idx].left].vals.append(TREE[idx].vals);

         // add half space to polyhedron
         TREE[TREE[idx].left].poly=TREE[idx].poly;
         TREE[TREE[idx].left].poly.intersect(TREE[idx].plane);

         // descend recursively
         intersect(TREE[idx].left);
         }

      // intersect right half space
      if (TREE[idx].right!=0)
         {
         // invert half space
         plane=TREE[idx].plane;
         plane.invert();

         // add inverted half space to polyhedron
         TREE[TREE[idx].right].poly=TREE[idx].poly;
         TREE[TREE[idx].right].poly.intersect(plane);

         // descend recursively
         intersect(TREE[idx].right);
         }

      // break polyhedral leaves into a set of connected tetrahedra
      if (TREE[idx].left==0 && TREE[idx].right==0)
         {
         // add half space to polyhedron
         TREE[idx].poly.intersect(TREE[idx].plane);

         // calculate connected tetrahedra
         TREE[idx].mesh.append(TREE[idx].poly);

         // calculate embedded data coordinates
         TREE[idx].mesh.setvals(TREE[idx].vals);
         }

      // shrink visited nodes
      TREE[idx].poly.clear();
      TREE[idx].vals.setnull();
      }
   }

// collect tetrahedra by descending the bsp tree
// also sort tetrahedra with respect to the eye point
// also cull tetrahedra with respect to the visibility radius
void minibsptree::collect(const unsigned int idx)
   {
   double dist;

   // check if bsp tree is empty
   if (!TREE.isnull())
      // check for leaf node
      if (TREE[idx].left==0 && TREE[idx].right==0)
         // append sorted leaf node
         COLLECT.append(TREE[idx].mesh.sort(EYE));
      else
         {
         // calculate distance of eye point to dividing plane
         dist=TREE[idx].plane.getdistance(EYE);

         // check which half space includes the eye point
         if (dist<0.0)
            {
            // collect left half space
            if (TREE[idx].left!=0)
               if (dist+RADIUS>0.0) collect(TREE[idx].left);

            // collect right half space
            if (TREE[idx].right!=0) collect(TREE[idx].right);
            }
         else
            {
            // collect right half space
            if (TREE[idx].right!=0)
               if (dist-RADIUS<0.0) collect(TREE[idx].right);

            // collect left half space
            if (TREE[idx].left!=0) collect(TREE[idx].left);
            }
         }
   }

// get volume of tetrahedralized polyhedra
double minibsptree::getvolume()
   {
   unsigned int i;

   if (!DONE)
      {
      intersect(0);
      DONE=TRUE;
      }

   if (VOLDONE) return(VOL);

   VOL=0.0;

   for (i=0; i<TREE.getsize(); i++)
      if (TREE[i].left==0 && TREE[i].right==0)
         VOL+=TREE[i].mesh.getvolume();

   VOLDONE=TRUE;

   return(VOL);
   }
