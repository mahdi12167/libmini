// (c) by Stefan Roettger, licensed under LGPL 2.1

#include <mini/miniio.h>

#include "objects.h"

Object::Object(const ministring &name,const ministring &repo)
   {
   filename=name;
   repository=repo;

   valid=load_header(coord,radius);

   node=NULL;
   }

Object::~Object()
   {clear_file();}

void Object::load()
   {valid=load_file();}

void Object::clear()
   {clear_file();}

Objects::Objects()
   : minikeyval<Object *>()
   {}

Objects::~Objects()
   {
   unsigned int i;

   for (i=0; i<get_pairs(); i++)
      delete get(i);
   }

void Objects::add(Object *obj)
   {minikeyval<Object *>::add(obj->filename,obj);}

void Objects::clear()
   {minikeyval<Object *>::clear();}

Object *Objects::get(const ministring &key)
   {return(*(minikeyval<Object *>::get(key)));}

Object_list Objects::list(const ministring &tag)
   {return(minikeyval<Object *>::get_tagged_items(tag));}

Object_list Objects::list(const ministrings &tags)
   {return(minikeyval<Object *>::get_tagged_items(tags));}
