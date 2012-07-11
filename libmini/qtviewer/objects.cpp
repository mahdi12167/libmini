// (c) by Stefan Roettger, licensed under LGPL 2.1

#include <mini/mini_util.h>

#include "viewer.h"
#include "objects.h"
#include "nodes.h"

// Object:

Object::Object(const ministring &name,const ministring &repo)
   {
   set_relative_path(repo);
   set_relative_name(name);

   coord=minicoord();
   radius=0.0;
   }

Object::~Object()
   {}

ministring Object::get_full_name() const
   {return(repository+filename);}

void Object::set_full_name(ministring name)
   {
   repository="";
   filename=name;
   }

ministring Object::get_relative_path() const
   {return(repository);}

ministring Object::get_relative_name() const
   {return(filename);}

void Object::set_relative_path(ministring path)
   {
   repository=path;

   if (repository.endswith("\\"))
      if (repository.size()>1)
         repository.shrinksize();
      else
         repository="/";

   if (repository.size()>0)
      if (!repository.endswith("/"))
         repository+="/";
   }

void Object::set_relative_name(ministring name)
   {
   filename=name;

   if (filename.startswith("file://"))
      filename=filename.suffix("file://");

   // check for relative path to truncate path
   if (repository.size()>0)
      if (filename.startswith(repository))
         filename=filename.suffix(repository);

   // check for absolute path to clear repository
   if (filename.startswith("/") ||
       filename.startswith("\\") ||
       filename.startswith("http://") ||
       filename.startswith("https://") ||
       filename.startswith("ftp://") ||
       filename.startswith("ftps://"))
      repository="";
   }

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
   {
   return(ministring("Object")+
          "\n\nrepo = "+repository+"\n"+
          "file = "+filename);
   }

void Object::show(BOOLINT yes) {}
BOOLINT Object::is_shown() const {return(TRUE);}

// Object_tileset:

Object_tileset::Object_tileset(const ministring &name,const ministring &repo,
                               Viewer *viewer)
   : Object(name,repo)
   {
   tileset_viewer=viewer;
   tileset_layer=NULL;

   shown=TRUE;
   }

Object_tileset::~Object_tileset()
   {}

ministring Object_tileset::get_info()
   {
   ministring info;

   miniv3d ext = tileset_layer->getextent();
   minicoord center = tileset_layer->get()->offsetDAT;

   int lts = tileset_viewer->get_earth()->get_terrain()->getLTSnum(tileset_layer);

   info = ministring("Tileset")+
          "\n\nrepo = "+repository+"\n"+
          "file = "+filename+"\n\n"+
          "dim = "+tileset_layer->getcols()+" x "+tileset_layer->getrows()+"\n"+
          "size = "+ext.x/1000+"km x "+ext.y/1000+"km\n"+
          "crs = "+center.getcrs()+"\n"+
          "datum = "+center.getdatum();

   if (lts>1)
      info += ministring("lods = ")+lts;

   return(info);
   }

BOOLINT Object_tileset::initGFX()
   {
   if (tileset_viewer!=NULL)
      if (tileset_layer==NULL)
         {
         tileset_layer=tileset_viewer->loadMap(get_full_name());

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

void Object_tileset::show(BOOLINT yes)
   {
   shown=yes;

   if (tileset_viewer!=NULL)
      if (tileset_layer!=NULL)
         {
         miniterrain *terrain=tileset_viewer->getearth()->getterrain();
         terrain->display(terrain->getnum(tileset_layer),yes);
         tileset_viewer->getCamera()->startIdling();
         }
   }

BOOLINT Object_tileset::is_shown() const
   {return(shown);}

void Object_tileset::focus()
   {
   if (tileset_viewer!=NULL)
      if (tileset_layer!=NULL)
         tileset_viewer->getCamera()->focusOnMap(tileset_layer);
   }

// Object_image:

mininode *Object_image::image_groupnode=NULL;
mininode *Object_image::deferred_groupnode=NULL;

Object_image::Object_image(const ministring &name,const ministring &repo,
                           Viewer *viewer)
   : Object(name,repo)
   {
   is_imagery_resp_elevation=TRUE;

   extent=grid_extent();
   size_x=size_y=0;
   size_ds=size_dt=0.0;
   spacing=0.0;

   image_viewer=viewer;
   image_node=NULL;

   shown=TRUE;
   }

Object_image::~Object_image()
   {}

ministring Object_image::get_info()
   {
   //!! also show: grid type, nodata value (nan), min max range
   return(ministring(is_imagery()?"Imagery":"Elevation")+
          "\n\nrepo = "+repository+"\n"+
          "file = "+filename+"\n\n"+
          "dim = "+size_x+" x "+size_y+"\n"+
          "size = "+size_ds/1000+"km x "+size_dt/1000+"km\n"+
          "spacing = "+spacing+"m\n"+
          "crs = "+extent.get_center().getcrs()+"\n"+
          "datum = "+extent.get_center().getdatum()+"\n\n"+
          "extent = "+extent);
   }

BOOLINT Object_image::initGFX()
   {
   if (image_viewer!=NULL)
      {
      mininode_root *root=image_viewer->getRoot();

      if (image_groupnode==NULL)
         image_groupnode=root->append_child(new mininode_group())->
                         append_child(new mininode_color(miniv3d(1,1,1)));

      if (deferred_groupnode==NULL)
         deferred_groupnode=root->append_child(new mininode_deferred_semitransparent())->
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
         size_x=layer->get_size_x();
         size_y=layer->get_size_y();
         size_ds=layer->get_size_ds();
         size_dt=layer->get_size_dt();
         spacing=layer->get_spacing();

         set_center(extent.get_center(),extent.get_size());

         image_node=new node_grid_extent(extent);
         deferred_groupnode->append_child(image_node);

         return(TRUE);
         }
      }

   return(FALSE);
   }

void Object_image::exitGFX()
   {
   if (image_viewer!=NULL)
      {
      image_viewer->getRoot()->remove_node(image_node);
      image_viewer->getCamera()->startIdling();
      }
   }

void Object_image::show(BOOLINT yes)
   {
   shown=yes;

   if (image_viewer!=NULL)
      if (image_node!=NULL)
         {
         image_node->show(yes);
         image_viewer->getCamera()->startIdling();
         }
   }

BOOLINT Object_image::is_shown() const
   {return(shown);}

void Object_image::focus()
   {
   if (image_viewer!=NULL)
      image_viewer->getCamera()->focusOnObject(this);
   }

void Object_image::set_thumb(const databuf *buf)
   {
   mininode_texture2D *tex2d_node=new mininode_texture2D;

   mininode_ref image=deferred_groupnode->remove_node(image_node);
   image_groupnode->append_child(tex2d_node);
   tex2d_node->append_child(image);
   tex2d_node->load(buf);

   image_viewer->getCamera()->startIdling();
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
      {
         minikeyval<Object *>::remove(key);

         MINILOG((ministring)" object failed to initialize");
      }
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

unsigned int Objects::get_num() const
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

ministrings Objects::list_wo(const ministring &tag)
   {return(minikeyval<Object *>::get_items_wo(tag));}

ministrings Objects::list_wo(const ministrings &tags)
   {return(minikeyval<Object *>::get_items_wo(tags));}

void Objects::set_repo(const ministring &repo)
   {
   unsigned int i;

   MINILOG("change repository to " + repo);

   for (i=0; i<get_num(); i++)
      {
      ministring path=get(i)->get_full_name();

      get(i)->set_relative_path(repo);
      get(i)->set_relative_name(path);
      }
   }

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
