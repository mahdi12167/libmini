// (c) by Stefan Roettger

#ifndef DATAFILL_H
#define DATAFILL_H

#include "database.h"

class datafill: public databuf
   {
   public:

   //! default constructor
   datafill(): databuf() {}

   //! copy constructor
   datafill(const databuf &buf): databuf(buf) {}

   //! grow boundary by extrapolation
   unsigned int grow_by_extrapolation(int radius=1);

   //! grow boundary by extrapolation and replace the rest
   unsigned int grow_by_extrapolation_and_replace(int radius=1,float value=0.0f);

   //! fill-in no-data values by region growing
   unsigned int fillin_by_regiongrowing(int radius_stop=5,int radius_start=2);

   //! fill-in no-data values by region growing and replace the rest
   unsigned int fillin_by_regiongrowing_and_replace(int radius_stop=5,int radius_start=2,float value=0.0f);

   protected:

   unsigned int fillin(int radius);
   };

#endif
