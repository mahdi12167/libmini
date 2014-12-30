// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef LUNAFUNCTOR_H
#define LUNAFUNCTOR_H

#include "ministring.h"
#include "lunaparse.h"

class lunafunctor
   {
   public:

   //! default constructor
   lunafunctor();

   //! destructor
   ~lunafunctor();

   //! set the code to be parsed and executed
   void setcode(ministring &code,
                const char *path=NULL,const char *altpath=NULL);

   //! evaluate the previously parsed program
   float evaluate(float x);

   protected:

   lunaparse *parser;
   };

#endif
