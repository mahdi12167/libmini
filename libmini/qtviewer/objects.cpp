// (c) by Stefan Roettger, licensed under LGPL 2.1

#include <mini/mini_util.h>

#include "viewer.h"
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

Object_tileset::Object_tileset(const ministring &name,const ministring &repo,
                               Viewer *viewer,minilayer *layer)
   : Object(name,repo)
   {
   tileset_viewer=viewer;
   tileset_layer=layer;
   }

void Object_tileset::focus()
   {
   if (tileset_viewer!=NULL && tileset_layer!=NULL)
      tileset_viewer->getCamera()->focusOnMap(tileset_layer);
   }

Objects::~Objects()
   {clear();}

void Objects::add(const ministring &key,Object *obj,const ministring &tag)
   {minikeyval<Object *>::add(key,obj,tag);}

void Objects::add(const ministring &key,Object *obj,const ministrings &tags)
   {minikeyval<Object *>::add(key,obj,tags);}

unsigned int Objects::get_num()
   {return(minikeyval<Object *>::get_pairs());}

Object *Objects::get(unsigned int i)
   {
   Object **obj=minikeyval<Object *>::get(i);
   return(obj?*obj:NULL);
   }

Object *Objects::get(const ministring &key)
   {
   Object **obj=minikeyval<Object *>::get(key);
   return(obj?*obj:NULL);
   }

ministrings *Objects::get_tags(const ministring &key)
   {return(minikeyval<Object *>::get_tags(key));}

ministrings Objects::list()
   {return(minikeyval<Object *>::get_items());}

ministrings Objects::list(const ministring &tag)
   {return(minikeyval<Object *>::get_items(tag));}

ministrings Objects::list(const ministrings &tags)
   {return(minikeyval<Object *>::get_items(tags));}

void Objects::remove(const ministring &key)
   {
   delete get(key);

   minikeyval<Object *>::remove(key);
   }

void Objects::clear()
   {
   unsigned int i;

   for (i=0; i<get_num(); i++)
      delete get(i);

   minikeyval<Object *>::clear();
   }
