// (c) by Stefan Roettger, licensed under LGPL 2.1

#include <mini/mini_util.h>

#include "objects.h"

Object::Object(const ministring &name,const ministring &repo)
   {
   filename=name;
   repository=repo;

   valid=FALSE;
   node=NULL;
   }

Object::~Object()
   {}

Objects::Objects()
   : minikeyval<Object *>()
   {}

Objects::~Objects()
   {
   for (unsigned i=0; i<get_pairs(); i++)
      delete get(i);
   }

void Objects::add(Object *obj)
   {minikeyval<Object *>::add(obj->filename,obj);}

void Objects::clear()
   {
   for (unsigned i=0; i<get_pairs(); i++)
      delete get(i);

   minikeyval<Object *>::clear();
   }

Object *Objects::get(const ministring &key)
   {return(*(minikeyval<Object *>::get(key)));}

Object_list Objects::list(const ministring &tag)
   {return(minikeyval<Object *>::get_tagged_items(tag));}

Object_list Objects::list(const ministrings &tags)
   {return(minikeyval<Object *>::get_tagged_items(tags));}
