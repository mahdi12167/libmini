// (c) by Stefan Roettger, licensed under LGPL 2.1

#include <mini/mini_util.h>

#include <grid/grid.h>

#include "viewer.h"
#include "objects.h"

// Object:

Object::Object(const ministring &name,const ministring &repo)
   {
   filename=name;
   repository=repo;
   }

Object::~Object()
   {}

// Object_tileset:

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

// Object_image:

mininode *Object_image::image_groupnode=NULL;

Object_image::Object_image(const ministring &name,const ministring &repo,
                           Viewer *viewer)
   : Object(name,repo)
   {
   image_viewer=viewer;
   image_node=NULL;
   }

Object_image::~Object_image()
   {}

BOOLINT Object_image::initGFX()
   {
   if (image_viewer!=NULL)
      {
      mininode_root *root=image_viewer->getRoot();

      if (image_groupnode==NULL)
         image_groupnode=root->append_child(new mininode_group());

      grid_list list;
      grid_layer *layer;

      // load input layer
      layer=list.load(repository,filename);

      // check for valid input layer
      if (layer!=NULL)
         {
         grid_extent ext=layer->extent;

         coord=ext.get_center();
         radius=ext.get_size();

         return(TRUE);
         }
      }

   return(FALSE);
   }

void Object_image::exitGFX()
   {
   if (image_viewer!=NULL)
      {
      //!!
      }
   }

void Object_image::focus()
   {
   if (image_viewer!=NULL)
      {
      //!!
      }
   }

// Objects:

Objects::Objects()
   : minikeyval<Object *>()
   {}

Objects::~Objects()
   {
   unsigned int i;

   for (i=0; i<get_num(); i++)
      {
      Object *obj=get(i);
      delete obj;
      }
   }

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

void Objects::remove(const ministrings &keys)
   {
   unsigned int i;

   for (i=0; i<keys.getsize(); i++)
      remove(keys[i]);
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
   }
