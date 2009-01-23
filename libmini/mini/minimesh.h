// (c) by Stefan Roettger

#ifndef MINIMESH_H
#define MINIMESH_H

#include "ostream"

#include "minibase.h"

#include "miniv3d.h"
#include "minimath.h"

#include "minidyna.h"
#include "minigeom.h"

typedef minidyna<miniv3d,10> minigon;

class minival
   {
   public:

   //! default constructor
   minival() {}

   //! destructor
   ~minival() {}

   minival(const unsigned int s,const unsigned int b,
           const miniv3d &c1,const miniv3d &c2,const miniv3d &c3,const miniv3d &c4)
      {
      slot=s;
      brickid=b;

      crd1=c1;
      crd2=c2;
      crd3=c3;
      crd4=c4;

      ref1=c1;
      ref2=c2;
      ref3=c3;
      ref4=c4;
      }

   minival(const unsigned int s,const unsigned int b,
           const miniv3d &c1,const miniv3d &c2,const miniv3d &c3,const miniv3d &c4,
           const miniv3d &r1,const miniv3d &r2,const miniv3d &r3,const miniv3d &r4)
      {
      slot=s;
      brickid=b;

      crd1=c1;
      crd2=c2;
      crd3=c3;
      crd4=c4;

      ref1=r1;
      ref2=r2;
      ref3=r3;
      ref4=r4;
      }

   unsigned int slot; // data slot
   unsigned int brickid; // data brick id
   miniv3d crd1,crd2,crd3,crd4; // data coordinates

   protected:

   miniv3d ref1,ref2,ref3,ref4; // reference coordinates

   friend class minimesh;
   };

//! stream output
inline std::ostream& operator << (std::ostream &out,const minival &v)
   {return(out << "minival( slot=" << v.slot << ", brickid=" << v.brickid << ", " << v.crd1 << ", " << v.crd2 << ", " << v.crd3 << ", " << v.crd4 << " )");}

typedef minidyna<minival,3> minivals;

class minihedron
   {
   public:

   //! default constructor
   minihedron()
      {
      dep123=dep142=dep243=dep341=0;

      intersect=FALSE;
      visit=FALSE;
      }

   //! constructor
   minihedron(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &v4,const minival &c)
      {
      vtx1=v1;
      vtx2=v2;
      vtx3=v3;
      vtx4=v4;

      vals.set(c);

      dep123=dep142=dep243=dep341=0;

      intersect=FALSE;
      visit=FALSE;
      }

   //! constructor
   minihedron(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &v4,const minivals &a)
      {
      vtx1=v1;
      vtx2=v2;
      vtx3=v3;
      vtx4=v4;

      vals=a;

      dep123=dep142=dep243=dep341=0;

      intersect=FALSE;
      visit=FALSE;
      }

   //! constructor
   minihedron(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &v4,const unsigned int s,const unsigned int b)
      {
      vtx1=v1;
      vtx2=v2;
      vtx3=v3;
      vtx4=v4;

      vals.set(minival(s,b,v1,v2,v3,v4));

      dep123=dep142=dep243=dep341=0;

      intersect=FALSE;
      visit=FALSE;
      }

   //! destructor
   ~minihedron() {}

   miniv3d barycenter() const
      {return(0.25*(vtx1+vtx2+vtx3+vtx4));}

   //! scale vertices by factor
   void scale(const double scale)
      {
      vtx1*=scale;
      vtx2*=scale;
      vtx3*=scale;
      vtx4*=scale;
      }

   //! shrink vertices by factor relative to barycenter
   void shrink(const double shrink)
      {
      miniv3d b;

      b=barycenter();

      vtx1=(vtx1-b)*shrink+b;
      vtx2=(vtx2-b)*shrink+b;
      vtx3=(vtx3-b)*shrink+b;
      vtx4=(vtx4-b)*shrink+b;
      }

   //! add offset to vertices
   void offset(const miniv3d &offset)
      {
      vtx1+=offset;
      vtx2+=offset;
      vtx3+=offset;
      vtx4+=offset;
      }

   //! multiply vertices with matrix
   void multiply(const miniv4d matrix[3])
      {
      miniv4d v1(vtx1,1.0),v2(vtx2,1.0),v3(vtx3,1.0),v4(vtx4,1.0);

      vtx1=miniv3d(matrix[0]*v1,matrix[1]*v1,matrix[2]*v1);
      vtx2=miniv3d(matrix[0]*v2,matrix[1]*v2,matrix[2]*v2);
      vtx3=miniv3d(matrix[0]*v3,matrix[1]*v3,matrix[2]*v3);
      vtx4=miniv3d(matrix[0]*v4,matrix[1]*v4,matrix[2]*v4);
      }

   //! get volume
   double getvolume() const
      {
      miniv3d mtx[3];

      mtx[0]=miniv3d(vtx1-vtx4);
      mtx[1]=miniv3d(vtx2-vtx4);
      mtx[2]=miniv3d(vtx3-vtx4);

      return(FABS(det_mtx(mtx))/2.0);
      }

   //! check for degeneracy
   BOOLINT check() const
      {return(getvolume()==0.0);}

   miniv3d vtx1,vtx2,vtx3,vtx4; // corner vertices
   minivals vals; // embedded data values

   BOOLINT intersect; // intersection flag

   protected:

   unsigned int dep123,dep142,dep243,dep341; // face dependencies

   BOOLINT visit; // sorting flag

   friend class minimesh;
   };

//! stream output
inline std::ostream& operator << (std::ostream &out,const minihedron &h)
   {return(out << "minihedron( " << h.vtx1 << ", " << h.vtx2 << ", " << h.vtx3 << ", " << h.vtx4 << ", " << h.vals << " )");}

class minimesh: public minidyna<minihedron>
   {
   public:

   //! default constructor
   minimesh();

   //! copy contructor
   minimesh(const minimesh &mesh);

   //! copy contructor
   minimesh(const minidyna<minihedron> &mesh);

   //! destructor
   ~minimesh();

   //! append a tetrahdron
   void append(const minihedron &h);

   //! append a tetrahedral mesh
   void append(const minimesh &m);

   //! append a polyhedron
   void append(const minigeom_polyhedron &poly);

   //! set embedded data values
   void setvals(const minivals &vals);

   //! get barycenter of mesh
   miniv3d barycenter() const;

   //! scale mesh by factor
   void scale(const double scale);

   //! shrink tetrahedra by factor relative to barycenter
   void shrink(const double shrink);

   //! shrink mesh by factor relative to barycenter
   void shrinkmesh(const double shrink);

   //! add offset to mesh
   void offset(const miniv3d &offset);

   //! multiply mesh with matrix
   void multiply(const miniv4d matrix[3]);

   //! get the maximum extent of the tetrahedra
   double getextent() const;

   //! get the bounding box of the tetrahedral mesh
   void getbbox(miniv3d &bbmin,miniv3d &bbmax) const;

   //! sort a tetrahedral mesh with respect to the eye point
   minimesh sort(const miniv3d &eye,BOOLINT intersect);

   //! get volume
   double getvolume() const;

   private:

   minidyna<minihedron> SORT;

   double CONFIGURE_DEGENERATE_SIZE;
   double CONFIGURE_DEGENERATE_ASPECT;

   minigon polygonize(minigeom_segments segments) const;
   minimesh tetrahedralize(const minigeom_polyhedron &poly) const;

   void reject();
   void connect();

   unsigned int getdep(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &h,unsigned int omit) const;
   double getdet(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &p) const;

   void descend(const unsigned int idx,const miniv3d &eye);

   void configure_degenerate_size(float size);
   void configure_degenerate_aspect(float aspect);
   };

//! stream output
inline std::ostream& operator << (std::ostream &out,const minimesh &mesh)
   {
   unsigned int i;

   out << "minimesh[" << std::endl;

   for (i=0; i<mesh.getsize(); i++) out << mesh.get(i) << std::endl;

   out << "]" << std::endl;

   return(out);
   }

#endif
