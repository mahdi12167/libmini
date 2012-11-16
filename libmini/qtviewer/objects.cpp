// (c) by Stefan Roettger, licensed under LGPL 2.1

#include <mini/mini_util.h>

#include "viewer.h"
#include "nodes.h"

#include "objects.h"

// Object_tileset:

Object_tileset::Object_tileset(const ministring &name,const ministring &repo,
                               Viewer *viewer)
   : Object_extents(name,repo)
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

   int lts = tileset_viewer->getearth()->getterrain()->getLTSnum(tileset_layer);

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

int Object_tileset::initGFX()
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

            return(OBJECT_SUCCESS);
            }
         }

   return(OBJECT_FAILURE);
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

ministring Object_tileset::serialize()
   {return("Object_tileset["+repository+","+filename+"]");}

grid_extent Object_tileset::get_extent() const
   {
   miniv3d ext=tileset_layer->getgeoextent();
   minicoord center=tileset_layer->getgeocenter();

   grid_extent extent;
   extent.set(center,ext.x,ext.y);

   return(extent);
   }

// Object_image:

mininode *Object_image::image_groupnode=NULL;
mininode *Object_image::deferred_groupnode1=NULL;
mininode *Object_image::deferred_groupnode2=NULL;

Object_image::Object_image(const ministring &name,const ministring &repo,
                           Viewer *viewer)
   : Object_extents(name,repo)
   {
   is_imagery_resp_elevation=TRUE;

   extent=grid_extent();
   extent_geo=grid_extent();
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
   ministring info;

   info = ministring(is_imagery()?"Imagery":"Elevation")+
          "\n\nrepo = "+repository+"\n"+
          "file = "+filename+"\n\n"+
          "dim = "+size_x+" x "+size_y+"\n"+
          "size = "+size_ds/1000+"km x "+size_dt/1000+"km\n"+
          "spacing = "+spacing+"m\n"+
          "crs = "+extent_geo.get_center().getcrs()+"\n"+
          "datum = "+extent_geo.get_center().getdatum()+"\n\n"+

   info += get_data_info()+"\n\n";

   info += "extent = "+extent_geo;

   return(info);
   }

ministring Object_image::get_data_info()
   {
   ministring info;

   grid_list list;
   grid_layer *layer;

   layer=list.load(repository,filename);

   if (layer!=NULL)
      {
      grid_value minv,maxv;
      layer->get_minmax(minv,maxv);

      unsigned int n = layer->has_nodata();

      info = ministring("range = ")+double(minv)+" .. "+double(maxv)+"\n"+
             "nodata = "+layer->get_nodata()+"\n";

      if (n<1000)
         info += ministring("#nodata = ")+n;
      else if (n<1000000)
         info += ministring("#nodata = ")+n/1000+"k";
      else
         info += ministring("#nodata = ")+n/1000000+"m";
      }

   return(info);
   }

int Object_image::initGFX()
   {
   int errorcode=OBJECT_FAILURE;

   if (image_viewer!=NULL)
      {
      mininode_root *root=image_viewer->getRoot();

      if (image_groupnode==NULL)
         image_groupnode=root->append_child(new mininode_culling())->
                         append_child(new mininode_color(miniv3d(1,1,1)));

      if (deferred_groupnode1==NULL)
         deferred_groupnode1=root->append_child(new mininode_deferred_semitransparent())->
                             append_child(new mininode_color(miniv4d(1.0,1.0,1.0,0.5)));

      if (deferred_groupnode2==NULL)
         deferred_groupnode2=root->append_child(new mininode_deferred_semitransparent())->
                             append_child(new mininode_color(miniv4d(1.0,1.0,1.0,0.9)));

      grid_list list;
      grid_layer *layer;

      // load input layer
      layer=list.load(repository,filename);

      // check for valid input layer
      if (layer!=NULL)
         {
         errorcode=OBJECT_SUCCESS;

         is_imagery_resp_elevation=layer->is_imagery();

         // check for valid geo-reference
         if (!layer->is_georeferenced())
            {
            // put invalid layer at the north pole
            layer->set_extent(80,360*minirand(),1);
            errorcode=OBJECT_NOT_REFERENCED;
            }

         // obesity check
         if (layer->get_estimated_mem()>1024) errorcode=OBJECT_TOO_LARGE;

         extent=layer->get_grid_extent();
         extent_geo=layer->get_extent();
         size_x=layer->get_size_x();
         size_y=layer->get_size_y();
         size_ds=layer->get_size_ds();
         size_dt=layer->get_size_dt();
         spacing=layer->get_spacing();

         set_center(extent_geo.get_center(),extent_geo.get_size());

         image_node=new node_grid_extent(extent_geo);
         if (image_node==NULL) MEMERROR();

         if (is_imagery_resp_elevation)
            deferred_groupnode2->append_child(image_node);
         else
            deferred_groupnode1->append_child(image_node);

         root->check_dirty();
         }
      }

   return(errorcode);
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

ministring Object_image::serialize()
   {return("Object_image["+repository+","+filename+"]");}

void Object_image::set_thumb(const databuf *buf)
   {
   mininode_texture2D *tex2d_node=new mininode_texture2D;
   if (tex2d_node==NULL) MEMERROR();

   mininode_ref image=image_viewer->getRoot()->remove_node(image_node);
   if (image==NULL) ERRORMSG();

   if (is_imagery_resp_elevation) deferred_groupnode2->append_child(tex2d_node);
   else deferred_groupnode1->append_child(tex2d_node);
   tex2d_node->append_child(image);
   tex2d_node->load(buf);

   image_viewer->getCamera()->startIdling();
   }

// Object_extent:

mininode *Object_extent::extent_groupnode=NULL;

Object_extent::Object_extent(const ministring &name,
                             const grid_extent &extent,
                             Viewer *viewer)
   : Object_extents(name)
   {
   this->extent=extent;

   extent_viewer=viewer;
   extent_node=NULL;

   shown=TRUE;
   }

Object_extent::~Object_extent()
   {}

ministring Object_extent::get_info()
   {
   ministring info;

   info = ministring("Extent")+
          "\n\ncrs = "+extent.get_center().getcrs()+"\n"+
          "datum = "+extent.get_center().getdatum()+"\n\n"+
          "extent = "+extent;

   return(info);
   }

int Object_extent::initGFX()
   {
   static const char *prog="\
      func checker(par x, par y, par n)\
         {\
         n = n-0.001;\
         return(((x*n)%2<1) ^ ((y*n)%2<1));\
         }\
      main(par x, par y, par z, par t)\
         {\
         var b = 0.025;\
         if (x<-0.5+b | x>0.5-b | y<-0.5+b | y>0.5-b) return(1, 1, 1, 1);\
         var a = checker(x, y, 64);\
         var v = 0.25*a;\
         return(v, v, v, a);\
         }";

   int errorcode=OBJECT_FAILURE;

   if (extent_viewer!=NULL)
      {
      mininode_root *root=extent_viewer->getRoot();

      if (extent_groupnode==NULL)
         {
         mininode_texture2D *tex2d_node=new mininode_texture2D;
         if (tex2d_node==NULL) MEMERROR();

         databuf buf;
         buf.set_implicit(prog,256,256,1,1,databuf::DATABUF_TYPE_RGBA);
         buf.automipmap();
         tex2d_node->load(&buf);

         extent_groupnode=root->append_child(new mininode_deferred_semitransparent())->
                          append_child(new mininode_color(miniv4d(1,0,0,0.999)))->
                          append_child(tex2d_node);
         }

      set_center(extent.get_center(),extent.get_size());

      extent_node=new node_grid_extent(extent);
      if (extent_node==NULL) MEMERROR();

      extent_groupnode->append_child(extent_node);

      root->check_dirty();

      errorcode=OBJECT_SUCCESS;
      }

   return(errorcode);
   }

void Object_extent::exitGFX()
   {
   if (extent_viewer!=NULL)
      {
      extent_viewer->getRoot()->remove_node(extent_node);
      extent_viewer->getCamera()->startIdling();
      }
   }

void Object_extent::show(BOOLINT yes)
   {
   shown=yes;

   if (extent_viewer!=NULL)
      if (extent_node!=NULL)
         {
         extent_node->show(yes);
         extent_viewer->getCamera()->startIdling();
         }
   }

BOOLINT Object_extent::is_shown() const
   {return(shown);}

void Object_extent::focus()
   {
   if (extent_viewer!=NULL)
      extent_viewer->getCamera()->focusOnObject(this);
   }

ministring Object_extent::serialize()
   {
   ministring info("Object_extent");

   info.append("[");
   info.append(extent.to_string());
   info.append("]");

   return(info);
   }

Object_extent *Object_extent::deserialize(ministring info,Viewer *viewer)
   {
   grid_extent ext;

   if (info.startswith("Object_extent"))
      {
      info=info.tail("Object_extent[");
      ext.from_string(info);
      info=info.tail("]");

      return(new Object_extent("extent",ext,viewer)); //!!
      }

   return(NULL);
   }
