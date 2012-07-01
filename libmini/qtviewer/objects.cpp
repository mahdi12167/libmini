// (c) by Stefan Roettger, licensed under LGPL 2.1

#include <mini/mini_util.h>

#include "viewer.h"
#include "objects.h"
#include "nodes.h"

// Object:

Object::Object(const ministring &name,const ministring &repo)
   {
   filename=name;
   repository=repo;

   if (filename.startswith("file://"))
      filename=filename.suffix("file://");

   if (repository.size()>0)
      if (!repository.endswith("/"))
         repository+="/";

   if (repository.size()>0)
      if (filename.startswith(repository))
         filename=filename.tail(repository.size());

   // check for absolute path to clear repository
   if (filename.startswith("/") ||
       filename.startswith("\\") ||
       filename.startswith("http://") ||
       filename.startswith("https://") ||
       filename.startswith("ftp://") ||
       filename.startswith("ftps://"))
      repository="";

   coord=minicoord();
   radius=0.0;
   }

Object::~Object()
   {}

void Object::set_center(minicoord c,double r)
   {
   coord=c;
   coord.convert2ecef();
   radius=r;
   }

minicoord Object::get_center()
   {
   coord.convert2ecef();
   return(coord);
   }

miniv3d Object::get_normal()
   {
   coord.convert2ecef();
   miniv3d normal=coord.vec;
   normal.normalize();
   return(normal);
   }

double Object::get_radius()
   {return(radius);}

ministring Object::get_info()
   {return("Object");}

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

ministring Object_tileset::get_info()
   {
   return(ministring("Tileset[")+
          tileset_layer->getcols()+"x"+
          tileset_layer->getrows()+
          "]");
   }

BOOLINT Object_tileset::initGFX()
   {
   if (tileset_viewer!=NULL)
      if (tileset_layer==NULL)
         {
         tileset_layer=tileset_viewer->loadMap(repository+filename);

         if (tileset_layer!=NULL)
            {
            minicoord center=tileset_layer->getcenter();
            miniv3d ext=tileset_layer->getextent();
            double r=sqrt(ext*ext)/2.0;

            set_center(center,r);

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
   is_imagery_resp_elevation=TRUE;
   extent=grid_extent();

   image_viewer=viewer;
   image_node=NULL;
   }

Object_image::~Object_image()
   {}

ministring Object_image::get_info()
   {
   return(ministring("Image[")+
          get_extent()+
          "]");
   }

BOOLINT Object_image::initGFX()
   {
   if (image_viewer!=NULL)
      {
      mininode_root *root=image_viewer->getRoot();

      if (image_groupnode==NULL)
         image_groupnode=root->append_child(new mininode_deferred_semitransparent())->
                         append_child(new mininode_color(miniv4d(0.5,0.5,1.0,0.5)));

      grid_list list;
      grid_layer *layer;

      // load input layer
      layer=list.load(repository,filename);

      // check for valid input layer
      if (layer!=NULL)
         {
         is_imagery_resp_elevation=layer->is_imagery();
         extent=layer->extent;

         set_center(extent.get_center(),extent.get_size());

         image_node=image_groupnode->append_child(new node_grid_extent(extent));

         return(TRUE);
         }
      }

   return(FALSE);
   }

void Object_image::exitGFX()
   {
   if (image_viewer!=NULL)
      if (image_groupnode!=NULL)
         {
         image_groupnode->remove_node(image_node);
         image_viewer->getCamera()->startIdling();
         }
   }

void Object_image::focus()
   {
   if (image_viewer!=NULL)
      image_viewer->getCamera()->focusOnObject(this);
   }

// Objects:

Objects::Objects()
   : minikeyval<Object *>()
   {}

Objects::~Objects()
   {}

BOOLINT Objects::add(const ministring &key,Object *obj,const ministring &tag)
   {
   MINILOG("adding object with key=" + key + " and tag=" + tag);

   if (minikeyval<Object *>::add(key,obj,tag))
      if (obj->initGFX())
         {
         MINILOG((ministring)" object extent: " + obj->get_center());
         MINILOG((ministring)" object size: " + obj->get_radius());

         return(TRUE);
         }
      else
         MINILOG((ministring)" object failed to initialize");
   else
      MINILOG((ministring)" object failed to be added");

   return(FALSE);
   }

BOOLINT Objects::add(const ministring &key,Object *obj,const ministrings &tags)
   {
   MINILOG("adding object with key=" + key + " and tags=" + tags.to_string());

   if (minikeyval<Object *>::add(key,obj,tags))
      if (obj->initGFX())
         {
         MINILOG((ministring)" object extent: " + obj->get_center());
         MINILOG((ministring)" object size: " + obj->get_radius());

         return(TRUE);
         }
      else
         MINILOG((ministring)" object failed to initialize");
   else
      MINILOG((ministring)" object failed to be added");

   return(FALSE);
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

void Objects::add_tag(ministring key,ministring tag)
   {
   MINILOG("adding tag to object with key=" + key + " and tag=" + tag);
   minikeyval<Object *>::tag(key,tag);
   }

void Objects::remove_tag(ministring key,ministring tag)
   {
   MINILOG("removing tag from object with key=" + key + " and tag=" + tag);
   minikeyval<Object *>::untag(key,tag);
   }

BOOLINT Objects::has_tag(ministring key,ministring tag)
   {return(minikeyval<Object *>::has_tag(key,tag));}

ministrings Objects::list()
   {return(minikeyval<Object *>::get_items());}

ministrings Objects::list(const ministring &tag)
   {return(minikeyval<Object *>::get_items(tag));}

ministrings Objects::list(const ministrings &tags)
   {return(minikeyval<Object *>::get_items(tags));}

void Objects::remove(const ministring &key)
   {
   MINILOG("removing object with key=" + key);

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
