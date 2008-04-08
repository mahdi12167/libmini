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

   class minival
      {
      public:

      minival() {}

      minival(const unsigned int s,const miniv3d &c1,const miniv3d &c2,const miniv3d &c3,const miniv3d &c4)
         {
         slot=s;

         crd[0]=c1;
         crd[1]=c2;
         crd[2]=c3;
         crd[3]=c4;
         }

      minival(const unsigned int s,const miniv3d c[4])
         {
         slot=s;

         crd[0]=c[0];
         crd[1]=c[1];
         crd[2]=c[2];
         crd[3]=c[3];
         }

      unsigned int slot; // data slot
      miniv3d crd[4]; // data coordinates
      };

   typedef minidyna<minival> minival_array;

   //! default constructor
   minitet() {}

   //! constructor
   minitet(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &v4,const minival_array &a)
      {
      vtx[0]=v1;
      vtx[1]=v2;
      vtx[2]=v3;
      vtx[3]=v4;

      val=a;
      }

   //! constructor
   minitet(const miniv3d v[4],const minival_array &a)
      {
      vtx[0]=v[0];
      vtx[1]=v[1];
      vtx[2]=v[2];
      vtx[3]=v[3];

      val=a;
      }

   //! destructor
   ~minitet();

   miniv3d vtx[4]; // corner vertices
   minival_array val; // embedded data values

   protected:

   private:
   };

typedef minidyna<minitet> minimesh;

class minibspt
   {
   public:

   //! default constructor
   minibspt();

   //! destructor
   ~minibspt();

   //! insert from tetrahedral mesh
   void insert(const minimesh &mesh);

   //! extract to tetrahedral mesh
   void extract(minimesh &mesh);

   protected:

   private:

   struct minibspt_struct
      {
      minigeom_plane plane;
      minitet::minival val;

      unsigned int left,right;

      minigeom_polyhedron poly;
      };

   minidyna<minibspt_struct> TREE;

   void insert(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &p,const minitet::minival &val);
   void insert(unsigned int idx,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const minibspt_struct &node);

   void intersect(unsigned int idx);
   };

#endif
