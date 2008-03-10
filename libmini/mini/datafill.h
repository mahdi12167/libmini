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

   //! fill-in no-data values by region growing
   unsigned int fillin_by_regiongrowing(int radius=5);
   };

#endif
