// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef LUNAFUNCTOR_H
#define LUNAFUNCTOR_H

#include "lunaparse.h"

class functor
   {
   public:

   //! default constructor
   functor() : range_min(0.0f), range_max(1.0f) {}

   //! destructor
   virtual ~functor() {}

   //! set the functor domain range
   void range(float min=0.0f,float max=1.0f)
      {
      range_min=min;
      range_max=max;
      }

   //! evaluate the denormalized functor
   float f(float x)
      {return(evaluate(range_min+x*(range_max-range_min)));}

   //! evaluate the functor
   virtual float evaluate(float x) = 0;

   //! check for errors
   BOOLINT error()
      {return(FALSE);}

   protected:

   float range_min;
   float range_max;
   };

class lunafunctor: public functor
   {
   public:

   //! default constructor
   lunafunctor();

   //! destructor
   virtual ~lunafunctor();

   //! set the expression to be evaluated
   void expr(const char *expr,
             const char *path=NULL);

   //! evaluate the previously parsed expression
   virtual float evaluate(float x);

   //! check for errors
   BOOLINT error();

   protected:

   lunaparse *parser_;
   char *expr_;
   int errors_;
   };

#endif
