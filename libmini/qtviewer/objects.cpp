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

Object_tileset::~Object_tileset()
   {
   if (tileset_viewer!=NULL && tileset_layer!=NULL)
      tileset_viewer->removeMap(tileset_layer);
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

Object *Objects::get(const ministring &key)
   {return(*(minikeyval<Object *>::get(key)));}

ministrings Objects::list()
   {return(minikeyval<Object *>::get_items());}

ministrings Objects::list(const ministring &tag)
   {return(minikeyval<Object *>::get_items(tag));}

ministrings Objects::list(const ministrings &tags)
   {return(minikeyval<Object *>::get_items(tags));}

void Objects::remove(const ministring &key)
   {minikeyval<Object *>::remove(key);}

void Objects::clear()
   {
   unsigned int i;

   for (i=0; i<get_pairs(); i++)
      delete get(i);

   minikeyval<Object *>::clear();
   }
