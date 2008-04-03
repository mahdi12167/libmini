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

   struct minitet_struct
      {
      unsigned int idx; // data index
      miniv3d crd[4]; // data coordinates
      };

   miniv3d vtx[4]; // corner vertices
   minidyna<minitet_struct> val; // data values

   protected:

   private:
   };

class minimesh: public minidyna<minitet>
   {
   public:

   //! default constructor
   minimesh();

   //! destructor
   ~minimesh();

   protected:

   private:
   };

#endif
