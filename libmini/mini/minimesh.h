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

   minival(const unsigned int s,
           const miniv3d &c1,const miniv3d &c2,const miniv3d &c3,const miniv3d &c4)
      {
      slot=s;

      crd1=c1;
      crd2=c2;
      crd3=c3;
      crd4=c4;

      ref1=c1;
      ref2=c2;
      ref3=c3;
      ref4=c4;
      }

   minival(const unsigned int s,
           const miniv3d &c1,const miniv3d &c2,const miniv3d &c3,const miniv3d &c4,
           const miniv3d &r1,const miniv3d &r2,const miniv3d &r3,const miniv3d &r4)
      {
      slot=s;

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
   miniv3d crd1,crd2,crd3,crd4; // data coordinates

   protected:

   miniv3d ref1,ref2,ref3,ref4; // reference coordinates

   friend class minimesh;
   };

//! stream output
inline std::ostream& operator << (std::ostream &out,const minival &v)
   {return(out << "minival( slot=" << v.slot << ", " << v.crd1 << ", " << v.crd2 << ", " << v.crd3 << ", " << v.crd4 << " )");}

typedef minidyna<minival,3> minivals;

class minihedron
   {
   public:

   //! default constructor
   minihedron()
      {
      dep123=dep142=dep243=dep341=0;
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
      visit=FALSE;
      }

   //! destructor
   ~minihedron() {}

   //! get volume
   double getvolume() const
      {
      miniv3d mtx[3];

      mtx[0]=miniv3d(vtx1-vtx4);
      mtx[1]=miniv3d(vtx2-vtx4);
      mtx[2]=miniv3d(vtx3-vtx4);

      return(FABS(det_mtx(mtx))/2.0);
      }

   miniv3d vtx1,vtx2,vtx3,vtx4; // corner vertices
   minivals vals; // embedded data values

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

   //! sort a tetrahedral mesh with respect to the eye point
   minimesh sort(const miniv3d &eye);

   //! get volume
   double getvolume() const;

   private:

   minidyna<minihedron> SORT;

   minigon polygonize(minigeom_segments segments) const;
   minimesh tetrahedralize(const minigeom_polyhedron &poly) const;

   void connect();

   unsigned int getdep(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &h) const;
   double getdet(const miniv3d &p,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3) const;

   void descend(const unsigned int idx,const miniv3d &eye);
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

class minibsptree
   {
   public:

   //! default constructor
   minibsptree();

   //! destructor
   ~minibsptree();

   //! clear bsp tree
   void clear();

   //! insert tetrahedral mesh
   void insert(const minimesh &mesh);

   //! extract tetrahedral mesh
   minimesh extract();

   //! extract sorted tetrahedral mesh
   minimesh extract(const miniv3d &eye,const double radius);

   //! get volume of tetrahedralized polyhedra
   double getvolume();

   private:

   struct minibsptree_node
      {
      minigeom_plane plane;
      unsigned int left,right;

      minivals vals;
      minigeom_polyhedron poly;
      minimesh mesh;
      };

   minidyna<minibsptree_node> TREE;
   BOOLINT DONE;

   miniv3d EYE;
   double RADIUS;
   BOOLINT GOTEYE;

   minimesh COLLECT;

   double VOL;
   BOOLINT VOLDONE;

   void insert(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &h,const minivals &vals);
   void insert(unsigned int idx,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const minivals &vals,const minigeom_plane &plane);

   void append(const minivals &vals,const minigeom_plane &plane);

   void intersect(unsigned int idx);

   void collect(const unsigned int idx);

   friend inline std::ostream& operator << (std::ostream &out,const minibsptree &tree);
   friend inline std::ostream& operator << (std::ostream &out,const minibsptree_node &node);
   };

//! stream output
inline std::ostream& operator << (std::ostream &out,const minibsptree &tree)
   {
   unsigned int i;

   out << "minibsptree[" << std::endl;

   for (i=0; i<tree.TREE.getsize(); i++) out << tree.TREE.get(i) << std::endl;

   out << "]" << std::endl;

   return(out);
   }

//! stream output
inline std::ostream& operator << (std::ostream &out,const minibsptree::minibsptree_node &node)
   {return(out << "node( plane=" << node.plane << ", left=" << node.left << ", right=" << node.right << ", #vals=" << node.vals.getsize() << " )");}

#endif
