// (c) by Stefan Roettger

#ifndef MINIMESH_H
#define MINIMESH_H

#include "minibase.h"

#include "miniv3d.h"
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
   miniv3d ref1,ref2,ref3,ref4; // reference coordinates
   };

typedef minidyna<minival,3> minivals;

class minihedron
   {
   public:

   //! default constructor
   minihedron()
      {
      dep123=dep142=dep243=dep341=0;
      flag=FALSE;
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
      flag=FALSE;
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
      flag=FALSE;
      }

   //! destructor
   ~minihedron();

   //! get volume
   double getvolume() const {return(0.0);} //!! calculate det/2

   miniv3d vtx1,vtx2,vtx3,vtx4; // corner vertices
   minivals vals; // embedded data values

   unsigned int dep123,dep142,dep243,dep341; // face dependencies
   BOOLINT flag; // spare flag
   };

class minimesh: public minidyna<minihedron>
   {
   public:

   //! default constructor
   minimesh(): minidyna<minihedron>() {}

   //! copy contructor
   minimesh(const minidyna<minihedron> &mesh): minidyna<minihedron>(mesh) {}

   //! append a tetrahdron
   void append(const minihedron &h) {minidyna<minihedron>::append(h);}

   //! append a tetrahedral mesh
   void append(const minimesh &m) {minidyna<minihedron>::append(m);}

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

   minigon polygonize(const minigeom_segments &segments) const;
   minimesh tetrahedralize(const minigeom_polyhedron &poly) const;

   void connect();
   unsigned int getdep(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &h) const;

   void descend(const unsigned int idx,const miniv3d &eye);
   };

class minibsptree
   {
   public:

   //! default constructor
   minibsptree();

   //! destructor
   ~minibsptree();

   //! clear bsp tree
   void clear();

   //! insert from tetrahedral mesh
   void insert(const minimesh &mesh);

   //! extract to tetrahedral mesh
   void extract(minimesh &mesh);

   //! extract to sorted tetrahedral mesh
   void extract(const miniv3d &eye,const double radius,minimesh &mesh);

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
   minimesh COLLECT;

   double VOL;
   BOOLINT VOLDONE;

   void insert(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &h,const minivals &vals);
   void insert(unsigned int idx,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const minivals &vals,const minigeom_plane &plane);

   void intersect(unsigned int idx);

   void collect(const unsigned int idx,const miniv3d &eye,const double radius=MAXFLOAT);
   };

#endif
