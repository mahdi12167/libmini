// (c) by Stefan Roettger

#include "minimesh.h"

// default constructor
minimesh::minimesh(): minidyna<minihedron>()
   {
   CONFIGURE_DEGENERATE_SIZE=0.0;
   CONFIGURE_DEGENERATE_ASPECT=0.0;
   }

// copy contructor
minimesh::minimesh(const minimesh &mesh): minidyna<minihedron>(mesh) {}

// copy contructor
minimesh::minimesh(const minidyna<minihedron> &mesh): minidyna<minihedron>(mesh) {}

// destructor
minimesh::~minimesh() {}

// append a tetrahdron
void minimesh::append(const minihedron &h)
   {
   if (h.check()) WARNMSG();
   minidyna<minihedron>::append(h);
   }

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

   if (poly.getnumhalfspace()<4) return(mesh);

   gon=polygonize(poly.getface(0));
   if (gon.getsize()==0) return(mesh);

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

   mesh.reject();
   mesh.connect();

   return(mesh);
   }

// reject degenerate tetrahedra
void minimesh::reject()
   {
   unsigned int i;

   miniv3d v1,v2,v3,v4;

   double d1,d2,d3,d4;
   double mind,maxd;

   // check all tetrahedra
   for (i=0; i<getsize(); i++)
      {
      // get vertices of the actual tetrahedron
      v1=get(i).vtx1;
      v2=get(i).vtx2;
      v3=get(i).vtx3;
      v4=get(i).vtx4;

      // calculate distances of corners to opposing face
      d1=minigeom_plane(v1,v2,v3,v4).getdistance(v4);
      d2=minigeom_plane(v1,v4,v2,v3).getdistance(v3);
      d3=minigeom_plane(v2,v4,v3,v1).getdistance(v1);
      d4=minigeom_plane(v3,v4,v1,v2).getdistance(v2);

      // calculate minimum and maximum distance
      mind=FMIN(FMIN(d1,d2),FMIN(d3,d4));
      maxd=FMAX(FMAX(d1,d2),FMAX(d3,d4));

      // check for degenerate size
      if (CONFIGURE_DEGENERATE_SIZE>0.0)
         if (mind<CONFIGURE_DEGENERATE_SIZE)
            {
            remove(i);
            i--;
            }

      // check for degenerate aspect
      if (CONFIGURE_DEGENERATE_ASPECT>0.0)
         if (mind/maxd<CONFIGURE_DEGENERATE_ASPECT)
            {
            remove(i);
            i--;
            }
      }
   }

// connect the faces of a tetrahedral mesh
void minimesh::connect()
   {
   unsigned int i;

   miniv3d v1,v2,v3,v4;

   // set dependencies for all tetrahedra
   for (i=0; i<getsize(); i++)
      {
      // get vertices of the actual tetrahedron
      v1=get(i).vtx1;
      v2=get(i).vtx2;
      v3=get(i).vtx3;
      v4=get(i).vtx4;

      // search for face dependencies
      ref(i).dep123=getdep(v1,v2,v3,v4,i);
      ref(i).dep142=getdep(v1,v4,v2,v3,i);
      ref(i).dep243=getdep(v2,v4,v3,v1,i);
      ref(i).dep341=getdep(v3,v4,v1,v2,i);
      }
   }

// search for a face dependency
unsigned int minimesh::getdep(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &h,unsigned int omit) const
   {
   unsigned int i;

   minigeom_plane plane;

   miniv3d p1,p2,p3,p4;
   miniv3d m1,m2,m3,m4;

   unsigned int idx;
   double dist,d;

   // calculate matching plane
   plane=minigeom_plane(v1,v2,v3,h);

   idx=0;
   dist=-MAXFLOAT;

   // search all tetrahedra for a match
   for (i=0; i<getsize(); i++)
      if (i!=omit)
         {
         // get vertices of the actual tetrahedron
         p1=get(i).vtx1;
         p2=get(i).vtx2;
         p3=get(i).vtx3;
         p4=get(i).vtx4;

         // calculate face midpoints
         m1=(p1+p2+p3)/3.0;
         m2=(p1+p4+p2)/3.0;
         m3=(p2+p4+p3)/3.0;
         m4=(p3+p4+p1)/3.0;

         // check each face for a match:

         d=plane.getdistance(m1);

         if (d<minigeom_base::delta)
            if (d>dist)
               {
               idx=i;
               dist=d;
               }

         d=plane.getdistance(m2);

         if (d<minigeom_base::delta)
            if (d>dist)
               {
               idx=i;
               dist=d;
               }

         d=plane.getdistance(m3);

         if (d<minigeom_base::delta)
            if (d>dist)
               {
               idx=i;
               dist=d;
               }

         d=plane.getdistance(m4);

         if (d<minigeom_base::delta)
            if (d>dist)
               {
               idx=i;
               dist=d;
               }
         }

   return(idx);
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

      // get the vertices of the actual tetrahedron
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
double minimesh::getdet(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &p) const
   {
   miniv3d mtx[3];

   mtx[0]=miniv3d(v1-p);
   mtx[1]=miniv3d(v2-p);
   mtx[2]=miniv3d(v3-p);

   return(det_mtx(mtx));
   }

// get barycenter of mesh
miniv3d minimesh::barycenter() const
   {
   unsigned int i;

   miniv3d b;

   b=miniv3d(0.0);

   if (getsize()==0) return(b);

   for (i=0; i<getsize(); i++)
      b+=get(i).vtx1+get(i).vtx2+get(i).vtx3+get(i).vtx4;

   return(b/(4.0*getsize()));
   }

// scale mesh by factor
void minimesh::scale(const double scale)
   {
   unsigned int i;

   if (scale!=1.0)
      for (i=0; i<getsize(); i++) ref(i).scale(scale);
   }

// shrink tetrahedra by factor relative to barycenter
void minimesh::shrink(const double shrink)
   {
   unsigned int i;

   if (shrink!=1.0)
      for (i=0; i<getsize(); i++) ref(i).shrink(shrink);
   }

// shrink mesh by factor relative to barycenter
void minimesh::shrinkmesh(const double shrink)
   {
   miniv3d b;

   if (shrink!=1.0)
      {
      b=barycenter();

      offset(-b);
      scale(shrink);
      offset(b);
      }
   }

// add offset to mesh
void minimesh::offset(const miniv3d &offset)
   {
   unsigned int i;

   if (offset!=miniv3d(0.0))
      for (i=0; i<getsize(); i++) ref(i).offset(offset);
   }

// multiply mesh with matrix
void minimesh::multiply(const miniv4d matrix[3])
   {
   unsigned int i;

   if (matrix[0]!=miniv4d(1.0,0.0,0.0) ||
       matrix[1]!=miniv4d(0.0,1.0,0.0) ||
       matrix[2]!=miniv4d(0.0,0.0,1.0))
      for (i=0; i<getsize(); i++) ref(i).multiply(matrix);
   }

// get the maximum extent of the tetrahedra
double minimesh::getextent() const
   {
   unsigned int i;

   minihedron tet;

   double ext2,len2;

   ext2=0.0;

   for (i=0; i<getsize(); i++)
      {
      tet=get(i);

      len2=(tet.vtx1-tet.vtx2)*(tet.vtx1-tet.vtx2);
      if (len2>ext2) ext2=len2;

      len2=(tet.vtx1-tet.vtx3)*(tet.vtx1-tet.vtx3);
      if (len2>ext2) ext2=len2;

      len2=(tet.vtx1-tet.vtx4)*(tet.vtx1-tet.vtx4);
      if (len2>ext2) ext2=len2;

      len2=(tet.vtx2-tet.vtx3)*(tet.vtx2-tet.vtx3);
      if (len2>ext2) ext2=len2;

      len2=(tet.vtx2-tet.vtx4)*(tet.vtx2-tet.vtx4);
      if (len2>ext2) ext2=len2;

      len2=(tet.vtx3-tet.vtx4)*(tet.vtx3-tet.vtx4);
      if (len2>ext2) ext2=len2;
      }

   return(sqrt(ext2));
   }

// get the bounding box of the tetrahedral mesh
void minimesh::getbbox(miniv3d &bbmin,miniv3d &bbmax) const
   {
   unsigned int i;

   minihedron tet;

   bbmin=bbmax=get(0).vtx1;

   for (i=0; i<getsize(); i++)
      {
      tet=get(i);

      if (tet.vtx1.x<bbmin.x) bbmin.x=tet.vtx1.x;
      if (tet.vtx1.x>bbmax.x) bbmax.x=tet.vtx1.x;
      if (tet.vtx1.y<bbmin.y) bbmin.y=tet.vtx1.y;
      if (tet.vtx1.y>bbmax.y) bbmax.y=tet.vtx1.y;
      if (tet.vtx1.z<bbmin.z) bbmin.z=tet.vtx1.z;
      if (tet.vtx1.z>bbmax.z) bbmax.z=tet.vtx1.z;

      if (tet.vtx2.x<bbmin.x) bbmin.x=tet.vtx2.x;
      if (tet.vtx2.x>bbmax.x) bbmax.x=tet.vtx2.x;
      if (tet.vtx2.y<bbmin.y) bbmin.y=tet.vtx2.y;
      if (tet.vtx2.y>bbmax.y) bbmax.y=tet.vtx2.y;
      if (tet.vtx2.z<bbmin.z) bbmin.z=tet.vtx2.z;
      if (tet.vtx2.z>bbmax.z) bbmax.z=tet.vtx2.z;

      if (tet.vtx3.x<bbmin.x) bbmin.x=tet.vtx3.x;
      if (tet.vtx3.x>bbmax.x) bbmax.x=tet.vtx3.x;
      if (tet.vtx3.y<bbmin.y) bbmin.y=tet.vtx3.y;
      if (tet.vtx3.y>bbmax.y) bbmax.y=tet.vtx3.y;
      if (tet.vtx3.z<bbmin.z) bbmin.z=tet.vtx3.z;
      if (tet.vtx3.z>bbmax.z) bbmax.z=tet.vtx3.z;

      if (tet.vtx4.x<bbmin.x) bbmin.x=tet.vtx4.x;
      if (tet.vtx4.x>bbmax.x) bbmax.x=tet.vtx4.x;
      if (tet.vtx4.y<bbmin.y) bbmin.y=tet.vtx4.y;
      if (tet.vtx4.y>bbmax.y) bbmax.y=tet.vtx4.y;
      if (tet.vtx4.z<bbmin.z) bbmin.z=tet.vtx4.z;
      if (tet.vtx4.z>bbmax.z) bbmax.z=tet.vtx4.z;
      }
   }

// sort a tetrahedral mesh with respect to the eye point
minimesh minimesh::sort(const miniv3d &eye,BOOLINT intersect)
   {
   unsigned int i;

   for (i=0; i<getsize(); i++)
      {
      ref(i).intersect=intersect;
      ref(i).visit=FALSE;
      }

   // sort and append all tetrahedra to the output mesh
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
      // check if the actual tetrahedron has been already visited
      if (get(idx).visit) return;

      // mark as already visited
      ref(idx).visit=TRUE;

      // get the vertices of the tetrahedron
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

// configuring:

void minimesh::configure_degenerate_size(float size)
   {CONFIGURE_DEGENERATE_SIZE=size;}

void minimesh::configure_degenerate_aspect(float aspect)
   {CONFIGURE_DEGENERATE_ASPECT=aspect;}
