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
   minilist();

   //! destructor
   ~minilist();

   //! set number of display lists
   void setnum(unsigned int n);

   //! start compiling a display list
   void start(unsigned int n);

   //! add triangle fans to a display list
   void addtrianglefans(float *array,int num,int stride=0);

   //! stop compiling a display list
   void stop();

   //! render a display list
   void render(unsigned int n);

   private:

   minidyna<unsigned int> LIST;
   };

#endif
