// (c) by Stefan Roettger, licensed under LGPL 2.1

#include <mini/mini_util.h>

#include "viewer.h"
#include "objects.h"

Object::Object(const ministring &name,const ministring &repo)
   {
   filename=name;
   repository=repo;
   }

Object::~Object()
   {}

Objects::Objects()
   : minikeyval<Object *>()
   {}

Object_tileset::Object_tileset(const ministring &name,const ministring &repo,
                               Viewer *viewer)
   : Object(name,repo)
   {
   tileset_viewer=viewer;
   tileset_layer=NULL;
   }

Object_tileset::~Object_tileset()
   {}

BOOLINT Object_tileset::initGFX()
   {
   if (tileset_viewer!=NULL)
      if (tileset_layer==NULL)
         {
         tileset_layer=tileset_viewer->loadMap(repository+filename);

         if (tileset_layer!=NULL)
            {
            coord=tileset_layer->getcenter();
            miniv3d ext=tileset_layer->getextent();
            radius=sqrt(ext*ext)/2.0;

            return(TRUE);
            }
         }

   return(FALSE);
   }

void Object_tileset::exitGFX()
   {
   if (tileset_viewer!=NULL)
      if (tileset_layer!=NULL)
         {
         tileset_viewer->removeMap(tileset_layer);
         tileset_layer=NULL;
         }
   }

void Object_tileset::focus()
   {
   if (tileset_viewer!=NULL)
      if (tileset_layer!=NULL)
         tileset_viewer->getCamera()->focusOnMap(tileset_layer);
   }

Objects::~Objects()
   {clear();}

BOOLINT Objects::add(const ministring &key,Object *obj,const ministring &tag)
   {
   minikeyval<Object *>::add(key,obj,tag);
   return(obj->initGFX());
   }

BOOLINT Objects::add(const ministring &key,Object *obj,const ministrings &tags)
   {
   minikeyval<Object *>::add(key,obj,tags);
   return(obj->initGFX());
   }

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
   Object *obj=get(key);

   if (obj)
      {
      minikeyval<Object *>::remove(key);
      obj->exitGFX();
      delete obj;
      }
   }

void Objects::clear()
   {
   unsigned int i;

   for (i=0; i<get_num(); i++)
      {
      Object *obj=get(i);
      obj->exitGFX();
      delete obj;
      }

   minikeyval<Object *>::clear();
   }
