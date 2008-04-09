// (c) by Stefan Roettger

#ifndef MINIMESH_H
#define MINIMESH_H

#include "minibase.h"

#include "miniv3d.h"
#include "minidyna.h"
#include "minigeom.h"

class minitet
   {
   public:

   //! default constructor
   minitet() {dep123=dep142=dep243=dep341=0;}

   //! constructor
   minitet(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &v4,const minival &c)
      {
      vtx1=v1;
      vtx2=v2;
      vtx3=v3;
      vtx4=v4;

      val.set(c);

      dep123=dep142=dep243=dep341=0;
      }

   //! constructor
   minitet(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &v4,const minival_array &a)
      {
      vtx1=v1;
      vtx2=v2;
      vtx3=v3;
      vtx4=v4;

      val=a;

      dep123=dep142=dep243=dep341=0;
      }

   //! constructor
   minitet(const miniv3d v[4],const minival &c)
      {
      vtx1=v[0];
      vtx2=v[1];
      vtx3=v[2];
      vtx4=v[3];

      val.set(c);

      dep123=dep142=dep243=dep341=0;
      }

   //! constructor
   minitet(const miniv3d v[4],const minival_array &a)
      {
      vtx1=v[0];
      vtx2=v[1];
      vtx3=v[2];
      vtx4=v[3];

      val=a;

      dep123=dep142=dep243=dep341=0;
      }

   //! destructor
   ~minitet();

   miniv3d vtx1,vtx2,vtx3,vtx4; // corner vertices
   minival_array val; // embedded data values

   unsigned int dep123,dep142,dep243,dep341; // face dependencies
   };

typedef minidyna<minitet> minimesh;

struct minibspt_node
   {
   minigeom_plane plane;
   unsigned int left,right;

   minival_array val;
   minigeom_polyhedron poly;
   minimesh mesh;
   };

typedef minidyna<minibspt_node> minibspt;

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

   //! extract to polyhedral tree
   void extract(minibspt &tree);

   private:

   minibspt TREE;
   BOOLINT DONE;

   void insert(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &p,const minival_array &val);
   void insert(unsigned int idx,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const minival_array &val,const minigeom_plane &plane);

   void intersect(unsigned int idx);
   void tetrahedrize(minigeom_polyhedron &poly,minimesh &mesh);
   };

#endif
