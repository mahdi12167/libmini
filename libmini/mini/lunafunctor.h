// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef LUNAFUNCTOR_H
#define LUNAFUNCTOR_H

#include "lunaparse.h"

class lunafunctor
   {
   public:

   //! default constructor
   lunafunctor();

   //! destructor
   ~lunafunctor();

   //! set the code to be parsed and executed
   void setcode(const char *code,int bytes,
                const char *path=NULL,const char *altpath=NULL);

   //! push one value onto the computation stack
   void pushvalue(float v);

   //! pop one value from the computation stack
   float popvalue();

   //! reset the interpreter
   void init();

   //! execute the previously parsed program
   void execute();

   protected:

   lunaparse *parser;

   private:
   };

#endif
