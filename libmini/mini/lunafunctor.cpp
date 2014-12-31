// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minibase.h"

#include "lunafunctor.h"

// default constructor
lunafunctor::lunafunctor()
   {
   parser=NULL;
   errors=0;
   }

// destructor
lunafunctor::~lunafunctor()
   {if (parser!=NULL) delete parser;}

// set the program to be parsed and executed
void lunafunctor::setcode(const char *code,
                          const char *path)
   {
   if (parser!=NULL) delete parser;
   parser=new lunaparse;

   parser->setcode(code);
   parser->setpath(path,"include/");
   errors=parser->parseLUNA();
   }

// evaluate the previously parsed code
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
