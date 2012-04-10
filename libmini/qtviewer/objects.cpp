// (c) by Stefan Roettger, licensed under LGPL 2.1

#include <mini/miniio.h>

#include "objects.h"

object::object(const ministring &name,const ministring &repo)
   {
   filename=name;
   repository=repo;

   valid=load_header(coord,radius);

   node=NULL;
   }

object::~object()
   {clear_file();}

void object::load()
   {valid=load_file();}

void object::clear()
   {clear_file();}

objects::objects()
   : minikeyval<object *>()
   {}

objects::~objects()
   {
   unsigned int i;

   for (i=0; i<get_pairs(); i++)
      delete get(i);
   }

void objects::add(object *obj)
   {minikeyval<object *>::add(obj->filename,obj);}

object_list objects::list(const ministring &tag)
   {return(minikeyval<object *>::get_tagged_items(tag));}
