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

   //! set the expression to be evaluated
   void expr(const char *expr,
             const char *path=NULL);

   //! evaluate the previously parsed expression
   float evaluate(float x);

   //! check for errors
   BOOLINT error();

   protected:

   lunaparse *parser;
   int errors;
   };

#endif
