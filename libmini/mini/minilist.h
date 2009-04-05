// (c) by Stefan Roettger

#ifndef MINILIST_H
#define MINILIST_H

#include "minibase.h"

#include "minidyna.h"

//! class for compiling a set of triangles to a display list
class minilist
   {
   public:

   //! default constructor
   minilist(unsigned int num);

   //! destructor
   ~minilist();

   //! start compiling the display list
   void start(unsigned int n);

   //! add triangle fans to the display list
   void addtrianglefans(float *array,int index,int num,int stride=0);

   //! stop compiling the display list
   void stop();

   //! render the display list
   void render(unsigned int n);

   private:

   minidyna<unsigned int> LIST;
   };

#endif
