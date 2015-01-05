// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minibase.h"

#include "lunafunctor.h"

// default constructor
lunafunctor::lunafunctor()
   : functor()
   {
   parser=NULL;
   errors=0;
   }

// destructor
lunafunctor::~lunafunctor()
   {if (parser!=NULL) delete parser;}

// set the expression to be parsed and evaluated
void lunafunctor::expr(const char *expr,
                       const char *path)
   {
   if (parser!=NULL) delete parser;
   parser=new lunaparse;

   parser->setcode("var x; main(par p){x=p;}");
   parser->setpath(path,"include/");
   parser->include("std.luna");
   parser->include("functor.luna");
   errors=parser->parseEXPR(expr);
   }

// evaluate the previously parsed expression
float lunafunctor::evaluate(float x)
   {
   float value;

   if (parser==NULL)
      {
      errors++;
      value=-1;
      }
   else
      {
      parser->getcode()->init(FALSE);
      parser->getcode()->pushvalue(x);
      parser->getcode()->execute();

      value=parser->getcode()->popvalue();
      errors+=parser->getcode()->geterrors();
      }

   return(value);
   }

// evaluate the previously parsed code
BOOLINT lunafunctor::error()
   {return(errors!=0);}
