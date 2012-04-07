// (c) by Stefan Roettger, licensed under LGPL 2.1

#include <mini/miniio.h>

#include "objects.h"

object::object(const ministring &name,const minicoord &crd)
   {
   filename=name;
   coord=crd;

   data=NULL;
   bytes=0;

   node=NULL;
   }

object::~object()
   {if (data!=NULL) free(data);}

void object::load()
   {
   data=readfile((repo+filename).c_str(),&bytes);
   if (data==NULL) ERRORMSG();
   }

objects::objects()
   : minikeyval<object>()
   {}

objects::~objects()
   {}

void objects::add(const ministring &key,const ministring &filename)
   {
   //!!
   }

object_list list(const ministring &tag)
   {
   //!!
   }
