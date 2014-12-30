// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minibase.h"

#include "lunafunctor.h"

// default constructor
lunafunctor::lunafunctor()
   {parser=NULL;}

// destructor
lunafunctor::~lunafunctor()
   {if (parser!=NULL) delete parser;}

// set the program to be parsed and executed
void lunafunctor::setcode(ministring &code,
                          const char *path,const char *altpath)
   {
   if (parser!=NULL) delete parser;
   parser=new lunaparse;

   parser->setcode(code.c_str(),code.size());
   parser->setpath(path,altpath);
   parser->parseLUNA();
   }

// evaluate the previously parsed code
float lunafunctor::evaluate(float x)
   {
   parser->getcode()->init(FALSE);
   parser->getcode()->pushvalue(x);
   parser->getcode()->execute();

   return(parser->getcode()->popvalue());
   }
