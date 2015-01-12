// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minibase.h"

#include "lunafunctor.h"

// default constructor
lunafunctor::lunafunctor()
   : functor()
   {
   parser_=NULL;
   expr_=NULL;
   errors_=0;
   }

// destructor
lunafunctor::~lunafunctor()
   {
   if (parser_!=NULL) delete parser_;
   if (expr_!=NULL) free(expr_);
   }

// set the expression to be parsed and evaluated
void lunafunctor::expr(const char *expr,
                       const char *path)
   {
   if (expr_!=NULL)
      if (strcmp(expr,expr_)==0) return;

   if (parser_!=NULL) delete parser_;
   parser_=new lunaparse;

   if (expr_!=NULL) free(expr_);
   expr_=strdup(expr);

   parser_->setcode("var x; main(par p){x=p;}");
   parser_->setpath(path,"include/");
   parser_->include("functor.luna");
   parser_->include("std.luna");
   errors_=parser_->parseEXPR(expr_);
   }

// evaluate the previously parsed expression
float lunafunctor::evaluate(float x)
   {
   float value;

   if (parser_==NULL)
      {
      errors_++;
      value=-1;
      }
   else
      {
      parser_->getcode()->init(FALSE);
      parser_->getcode()->pushvalue(x);
      parser_->getcode()->execute();

      value=parser_->getcode()->popvalue();
      errors_+=parser_->getcode()->geterrors();
      }

   return(value);
   }

// evaluate the previously parsed code
BOOLINT lunafunctor::error()
   {return(errors_!=0);}
