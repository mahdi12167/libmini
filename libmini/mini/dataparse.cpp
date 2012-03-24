// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minibase.h"

#include "dataparse.h"

// default constructor
dataparse::dataparse()
   {parser=NULL;}

// destructor
dataparse::~dataparse()
   {if (parser!=NULL) delete parser;}

// set the program to be parsed and executed
void dataparse::setcode(const char *code,int bytes,
                        const char *path,const char *altpath)
   {
   if (parser!=NULL) delete parser;
   parser=new lunaparse;

   parser->setcode(code,bytes);
   parser->setpath(path,altpath);
   parser->parseLUNA();

   parser->getcode()->init();
   }

// push one value onto the computation stack
void dataparse::pushvalue(float v)
   {parser->getcode()->pushvalue(v);}

// pop one value from the computation stack
float dataparse::popvalue()
   {return(parser->getcode()->popvalue());}

// reset the interpreter
void dataparse::init()
   {parser->getcode()->init(FALSE);}

// execute the previously parsed code
void dataparse::execute()
   {parser->getcode()->execute();}
