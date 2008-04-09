// (c) by Stefan Roettger

#ifndef MINIMESH_H
#define MINIMESH_H

#include "minibase.h"

#include "miniv3d.h"
#include "minidyna.h"
#include "minigeom.h"

typedef minidyna<miniv3d> minigon;

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

   //! constructor
   minihedron(const miniv3d v[4],const minival &c)
      {
      vtx1=v[0];
      vtx2=v[1];
      vtx3=v[2];
      vtx4=v[3];

      vals.set(c);

      dep123=dep142=dep243=dep341=0;
      flag=FALSE;
      }

   //! constructor
   minihedron(const miniv3d v[4],const minivals &a)
      {
      vtx1=v[0];
      vtx2=v[1];
      vtx3=v[2];
      vtx4=v[3];

      vals=a;

      dep123=dep142=dep243=dep341=0;
      flag=FALSE;
      }

   //! destructor
   ~minihedron();

   miniv3d vtx1,vtx2,vtx3,vtx4; // corner vertices
   minivals vals; // embedded data values

   unsigned int dep123,dep142,dep243,dep341; // face dependencies
   BOOLINT flag; // spare flag
   };

typedef minidyna<minihedron> minimesh;

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
   void extract(const miniv3d &eye,minimesh &mesh);

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
   BOOLINT GOTEYE;
   minimesh COLLECT;

   void insert(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &p,const minivals &vals);
   void insert(unsigned int idx,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const minivals &vals,const minigeom_plane &plane);

   void intersect(unsigned int idx);
   void polygonize(const minidyna<minigeom_segment> &segments,minigon &gon);
   void tetrahedralize(const minigeom_polyhedron &poly,minimesh &mesh);

   void collect(const unsigned int idx,const miniv3d &eye);
   };

#endif
