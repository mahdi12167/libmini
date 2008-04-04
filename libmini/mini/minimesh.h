// (c) by Stefan Roettger

#ifndef MINIMESH_H
#define MINIMESH_H

#include "minibase.h"

#include "miniv3d.h"
#include "minidyna.h"

class minitet
   {
   public:

   //! default constructor
   minitet();

   //! destructor
   ~minitet();

   struct minival_struct
      {
      unsigned int slot; // data slot
      miniv3d crd[4]; // data coordinates
      };

   typedef minidyna<minival_struct> minival_type;

   miniv3d vtx[4]; // corner vertices
   minival_type val; // embedded data values

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
      miniv3d nrm;
      minitet::minival_type val;

      unsigned int left,right;
      };

   minidyna<minibspt_struct> TREE;

   void insert(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &p,const minitet::minival_type &val);
   };

#endif
