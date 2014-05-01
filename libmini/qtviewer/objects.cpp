// (c) by Stefan Roettger, licensed under GPL 2+

#include <mini/mini_util.h>

#include "viewer.h"
#include "nodes.h"

#include "objects.h"

// Object_tileset:

Object_tileset::Object_tileset(Viewer *v,
                               const ministring &name,const ministring &repo)
   : Object_extents(v,name,repo)
   {
   tileset_layer=NULL;

   shown=TRUE;
   }

Object_tileset::~Object_tileset()
   {}

ministring Object_tileset::get_info()
   {
   ministring info;

   if (tileset_layer==NULL) return("");

   miniv3d ext = tileset_layer->getextent();
   minicoord center = tileset_layer->get()->offsetDAT;

   int lts = viewer->getearth()->getterrain()->getLTSnum(tileset_layer);

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
   if (viewer!=NULL)
      if (tileset_layer==NULL)
         {
         tileset_layer=viewer->loadMap(get_full_name());

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
   if (viewer!=NULL)
      if (tileset_layer!=NULL)
         {
         viewer->removeMap(tileset_layer);
         tileset_layer=NULL;
         }
   }

void Object_tileset::show(BOOLINT yes)
   {
   shown=yes;

   if (viewer!=NULL)
      if (tileset_layer!=NULL)
         {
         miniterrain *terrain=viewer->getearth()->getterrain();
         terrain->display(terrain->getnum(tileset_layer),yes);
         viewer->getCamera()->startIdling();
         }
   }

BOOLINT Object_tileset::is_shown() const
   {return(shown);}

void Object_tileset::focus()
   {
   if (viewer!=NULL)
      if (tileset_layer!=NULL)
         viewer->getCamera()->focusOnMap(tileset_layer);
   }

ministring Object_tileset::to_string()
   {return("Object_tileset["+repository+","+filename+"]");}

void Object_tileset::from_string(ministring &info)
   {
   if (info.startswith("Object_tileset"))
      {
      info=info.tail("Object_tileset[");
      ministring repo=info.prefix(",");
      info=info.tail(",");
      ministring name=info.prefix("]");
      info=info.tail("]");

      *this=Object_tileset(viewer,name,repo);
      }
   }

grid_extent Object_tileset::get_extent()
   {
   miniv3d ext;
   minicoord center;

   if (!extent.check())
      if (tileset_layer!=NULL)
         {
         ext=tileset_layer->getgeoextent();
         center=tileset_layer->getgeocenter();

         extent.set(center,ext.x,ext.y);
         }

   return(extent);
   }

// Object_image:

mininode *Object_image::image_groupnode=NULL;
mininode *Object_image::deferred_groupnode1=NULL;
mininode *Object_image::deferred_groupnode2=NULL;

Object_image::Object_image(Viewer *v,
                           const ministring &name,const ministring &repo)
   : Object_extents(v,name,repo)
   {
   is_imagery_resp_elevation=TRUE;
   is_geolocated=FALSE;

   extent=grid_extent();
   extent_geo=grid_extent();
   size_x=size_y=0;
   size_ds=size_dt=0.0;
   spacing=0.0;
   spacing_ds=spacing_dt=0.0;

   ecef_node=NULL;
   lod_node=NULL;
   image_node=NULL;

   shown=TRUE;
   marked=FALSE;
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
          "spacing_ds = "+spacing_ds+"m\n"+
          "spacing_dt = "+spacing_dt+"m\n"+
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

   if (viewer!=NULL)
      {
      mininode_root *root=viewer->getRoot();

      if (image_groupnode==NULL)
         image_groupnode=root->append_child(new mininode_color(miniv3d(1,1,1)));

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
            minicoord geolocation;
            double geosize;

            // check for valid geo-location (exif gps tag)
            if (layer->get_geolocation(geolocation,geosize))
               {
               geolocation.convert2llh();
               if (geosize<=0.0) geosize=OBJECT_PHOTO_SIZE;

               layer->set_extent(geolocation.vec.y/3600,geolocation.vec.x/3600,
                                 geosize/max(layer->get_size_x(),layer->get_size_y()),
                                 geolocation.vec.z,geolocation.vec.w);

               is_geolocated=TRUE;
               }
            else
               {
               // put invalid layer at the north pole
               layer->set_extent(OBJECT_INVALID_LAT,360*minirand(),1);
               errorcode=OBJECT_NOT_REFERENCED;

               // check for invalid photo
               if (!layer->get_metadata_tag("EXIF_ExifVersion").empty()) errorcode=OBJECT_NOT_GEOLOCATED;
               }
            }

         // obesity check
         if (sizeof(void *)<=4) // check for 32bit systems
            if (layer->get_estimated_mem()>OBJECT_OBESITY_SIZE) errorcode=OBJECT_TOO_LARGE;

         extent=layer->get_grid_extent();
         extent_geo=layer->get_extent();
         size_x=layer->get_size_x();
         size_y=layer->get_size_y();
         size_ds=layer->get_size_ds();
         size_dt=layer->get_size_dt();
         spacing=layer->get_spacing();
         spacing_ds=layer->get_spacing_ds();
         spacing_dt=layer->get_spacing_dt();

         set_center(extent_geo.get_center(),extent_geo.get_radius());

         ecef_node=new mininode_ecef();
         if (ecef_node==NULL) MEMERROR();

         image_node=new node_grid_extent(extent_geo);
         if (image_node==NULL) MEMERROR();

         // pass object key to image node
         image_node->set_name(get_full_name());

         // link image node
         if (is_imagery_resp_elevation)
            deferred_groupnode2->append_child(ecef_node)->append_child(image_node);
         else
            deferred_groupnode1->append_child(ecef_node)->append_child(image_node);

         // update dirty scene graph
         root->check_dirty();
         }
      }

   return(errorcode);
   }

void Object_image::exitGFX()
   {
   if (viewer!=NULL)
      if (image_node!=NULL)
         {
         viewer->getRoot()->remove_subgraph(ecef_node);
         viewer->getCamera()->startIdling();
         }
   }

void Object_image::show(BOOLINT yes)
   {
   shown=yes;

   if (viewer!=NULL)
      if (image_node!=NULL)
         {
         image_node->show(yes);
         viewer->getCamera()->startIdling();
         }
   }

BOOLINT Object_image::is_shown() const
   {return(shown);}

void Object_image::focus()
   {
   if (viewer!=NULL)
      viewer->getCamera()->focusOnObject(this);
   }

ministring Object_image::to_string()
   {return("Object_image["+repository+","+filename+"]");}

void Object_image::from_string(ministring &info)
   {
   if (info.startswith("Object_image"))
      {
      info=info.tail("Object_image[");
      ministring repo=info.prefix(",");
      info=info.tail(",");
      ministring name=info.prefix("]");
      info=info.tail("]");

      *this=Object_image(viewer,name,repo);
      }
   }

void Object_image::mark(BOOLINT yes)
   {
   marked=yes;

   if (image_node!=NULL)
      {
      image_node->set_color(miniv4d(1,0,0,0.5));
      image_node->enable_color(yes);

      viewer->getCamera()->startIdling();
      }
   }

BOOLINT Object_image::is_marked() const
   {return(marked);}

void Object_image::set_thumb(const databuf *buf)
   {
   if (ecef_node==NULL) return;
   if (lod_node!=NULL) return;

   lod_node=new mininode_lod(1.0);
   if (lod_node==NULL) MEMERROR();

   mininode_texture2D *tex2d_node=new mininode_texture2D;
   if (tex2d_node==NULL) MEMERROR();

   lod_node->append_child(tex2d_node);

   mininode_ref graph=viewer->getRoot()->remove_subgraph(ecef_node);
   if (graph==NULL) ERRORMSG();

   if (is_imagery_resp_elevation) deferred_groupnode2->append_child(lod_node);
   else deferred_groupnode1->append_child(lod_node);
   tex2d_node->append_child(graph);
   tex2d_node->load(buf);

   viewer->getCamera()->startIdling();
   }

void Object_image::set_fullres(const databuf *buf)
   {
   if (ecef_node==NULL) return;
   if (lod_node==NULL) return;
   if (lod_node->get_selections()>1) return;

   mininode_texture2D *tex2d_node=new mininode_texture2D;
   if (tex2d_node==NULL) MEMERROR();

   lod_node->prepend_child(tex2d_node);

   tex2d_node->append_child(ecef_node);
   tex2d_node->load(buf);

   viewer->getCamera()->startIdling();
   }

// Object_extent:

mininode *Object_extent::extent_groupnode=NULL;

Object_extent::Object_extent(Viewer *v,
                             const ministring &name,const ministring &repo,
                             const grid_extent &extent)
   : Object_extents(v,name,repo)
   {
   this->extent=extent;

   ecef_node=NULL;
   extent_node=NULL;

   shown=TRUE;
   marked=FALSE;
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

   if (viewer!=NULL)
      {
      mininode_root *root=viewer->getRoot();

      if (extent_groupnode==NULL)
         {
         mininode_texture2D *tex2d_node=new mininode_texture2D;
         if (tex2d_node==NULL) MEMERROR();

         databuf buf;
         buf.set_implicit(prog,256,256,1,1,databuf::DATABUF_TYPE_RGBA);
         buf.automipmap();
         tex2d_node->load(&buf);

         extent_groupnode=root->append_child(new mininode_deferred_semitransparent())->
                          append_child(new mininode_color(miniv4d(0,0,1,0.999)))->
                          append_child(tex2d_node);
         }

      set_center(extent.get_center(),extent.get_radius());

      ecef_node=new mininode_ecef();
      if (ecef_node==NULL) MEMERROR();

      extent_node=new node_grid_extent(extent);
      if (extent_node==NULL) MEMERROR();

      // pass object key to extent node
      extent_node->set_name(get_full_name());

      // link extent node
      extent_groupnode->append_child(ecef_node)-> append_child(extent_node);

      // update dirty scene graph
      root->check_dirty();

      errorcode=OBJECT_SUCCESS;
      }

   return(errorcode);
   }

void Object_extent::exitGFX()
   {
   if (viewer!=NULL)
      if (extent_node!=NULL)
         {
         viewer->getRoot()->remove_subgraph(ecef_node);
         viewer->getCamera()->startIdling();
         }
   }

void Object_extent::show(BOOLINT yes)
   {
   shown=yes;

   if (viewer!=NULL)
      if (extent_node!=NULL)
         {
         extent_node->show(yes);
         viewer->getCamera()->startIdling();
         }
   }

BOOLINT Object_extent::is_shown() const
   {return(shown);}

void Object_extent::focus()
   {
   if (viewer!=NULL)
      viewer->getCamera()->focusOnObject(this);
   }

void Object_extent::updateGFX()
   {
   exitGFX();
   initGFX();

   show(shown);
   mark(marked);
   }

ministring Object_extent::to_string()
   {
   ministring info("Object_extent");

   info.append("[");
   info.append(extent.to_string());
   info.append("]");

   return(info);
   }

void Object_extent::from_string(ministring &info)
   {
   grid_extent ext;

   if (info.startswith("Object_extent"))
      {
      info=info.tail("Object_extent[");
      ext.from_string(info);
      info=info.tail("]");

      *this=Object_extent(viewer,filename,repository,ext);
      }
   }

void Object_extent::mark(BOOLINT yes)
   {
   marked=yes;

   if (extent_node!=NULL)
      {
      extent_node->set_color(miniv4d(1,0,0,0.5));
      extent_node->enable_color(yes);

      viewer->getCamera()->startIdling();
      }
   }

BOOLINT Object_extent::is_marked() const
   {return(marked);}

void Object_extent::move(const minicoord &pos0,const minicoord &pos1)
   {
   grid_extent ext=get_extent();
   ext.metric_move(pos0,pos1);
   set_extent(ext);
   }

void Object_extent::rotate(const minicoord &pos0,const minicoord &pos1)
   {
   grid_extent ext=get_extent();
   ext.metric_rotate(pos0,pos1);
   set_extent(ext);
   }

void Object_extent::scale_ds(const minicoord &pos0,const minicoord &pos1)
   {
   grid_extent ext=get_extent();
   ext.metric_scale_ds(pos0,pos1);
   set_extent(ext);
   }

void Object_extent::scale_dt(const minicoord &pos0,const minicoord &pos1)
   {
   grid_extent ext=get_extent();
   ext.metric_scale_dt(pos0,pos1);
   set_extent(ext);
   }

// Object_path:

mininode *Object_path::path_groupnode=NULL;

Object_path::Object_path(Viewer *v,
                         const ministring &name,const ministring &repo)
   : Object_serializable(v,name,repo)
   {
   shown=TRUE;
   marked=FALSE;
   }

Object_path::~Object_path()
   {}

ministring Object_path::get_info()
   {
   ministring info;

   if (path_node==NULL) return("");

   info = ministring("Path")+
          "\n\nrepo = "+repository+"\n"+
          "file = "+filename+"\n\n"+
          "length = "+path_node->getpath()->get_length()/1000+"km\n"+
          "duration = "+path_node->getpath()->get_time_period()/3600+"h";

   return(info);
   }

int Object_path::initGFX()
   {
   int errorcode=OBJECT_FAILURE;

   if (viewer!=NULL)
      {
      mininode_root *root=viewer->getRoot();

      if (path_groupnode==NULL)
         path_groupnode=root->append_child(new mininode_color(miniv3d(1,1,1)));

      ecef_node=new mininode_ecef();
      if (ecef_node==NULL) MEMERROR();

      path_node=new mininode_geometry_path_clod();
      if (path_node==NULL) MEMERROR();

      path_node->load(get_full_name());

      path_node->create(0.25,100, // maximum deviation at specific distance
                        1, // maximum width at specific distance
                        0.0,30.0,1.0,1.0, // velocity color mapping
                        1.0, // weight of color mapping
                        100); // vertices per update

      if (!path_node->getpath()->empty())
         {
         miniv3d center;
         double radius2;

         // get bounding sphere
         path_node->getpath()->getbsphere(center,radius2);
         set_center(path_node->getpath()->get(0),2.0*sqrt(radius2));

         // link path node
         path_groupnode->append_child(ecef_node)->append_child(path_node);

         // pass object key to image node
         path_node->set_name(get_full_name());

         // update dirty scene graph
         root->check_dirty();

         errorcode=OBJECT_SUCCESS;
         }
      }

   return(errorcode);
   }

void Object_path::exitGFX()
   {
   if (viewer!=NULL)
      if (path_node!=NULL)
         {
         viewer->getRoot()->remove_subgraph(ecef_node);
         viewer->getCamera()->startIdling();
         }
   }

void Object_path::show(BOOLINT yes)
   {
   shown=yes;

   if (viewer!=NULL)
      if (path_node!=NULL)
         {
         path_node->show(yes);
         viewer->getCamera()->startIdling();
         }
   }

BOOLINT Object_path::is_shown() const
   {return(shown);}

void Object_path::focus()
   {
   if (viewer!=NULL)
      viewer->getCamera()->focusOnObject(this);
   }

ministring Object_path::to_string()
   {return("Object_path["+repository+","+filename+"]");}

void Object_path::from_string(ministring &info)
   {
   if (info.startswith("Object_path"))
      {
      info=info.tail("Object_path[");
      ministring repo=info.prefix(",");
      info=info.tail(",");
      ministring name=info.prefix("]");
      info=info.tail("]");

      *this=Object_path(viewer,name,repo);
      }
   }

void Object_path::mark(BOOLINT yes)
   {
   marked=yes;

   if (path_node!=NULL)
      {
      path_node->set_color(miniv4d(1,0,0));
      path_node->enable_color(yes);

      viewer->getCamera()->startIdling();
      }
   }

BOOLINT Object_path::is_marked() const
   {return(marked);}
