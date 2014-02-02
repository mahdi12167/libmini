// (c) by Stefan Roettger, licensed under GPL 2+

#ifndef OBJECTS_H
#define OBJECTS_H

#include <mini/mini_generic.h>
#include <mini/mini_format.h>
#include <mini/mini_tileset.h>
#include <mini/mini_object.h>

#include <grid/grid.h>

#include "object.h"

class Viewer;

#define OBJECT_PHOTO_SIZE 200.0
#define OBJECT_INVALID_LAT 80.0
#define OBJECT_OBESITY_SIZE 2048

enum
   {
   OBJECT_SUCCESS=0,
   OBJECT_FAILURE=1,
   OBJECT_NOT_REFERENCED=2,
   OBJECT_NOT_GEOLOCATED=3,
   OBJECT_TOO_LARGE=4
   };

//! tileset object
class Object_tileset: public Object_extents
   {
   public:

   Object_tileset(const ministring &name="",const ministring &repo="",
                  Viewer *viewer=NULL);

   virtual ~Object_tileset();

   virtual ministring get_info();

   virtual int initGFX();
   virtual void exitGFX();

   virtual void show(BOOLINT yes=TRUE);
   virtual BOOLINT is_shown() const;

   virtual void focus();

   virtual ministring serialize();

   //! get grid-centered extents
   virtual grid_extent get_extent();

   protected:

   Viewer *tileset_viewer;
   minilayer *tileset_layer;

   BOOLINT shown;
   };

//! image object
class Object_image: public Object_extents
   {
   public:

   Object_image(const ministring &name="",const ministring &repo="",
                Viewer *viewer=NULL);

   virtual ~Object_image();

   virtual ministring get_info();
   virtual ministring get_data_info();

   virtual int initGFX();
   virtual void exitGFX();

   virtual void show(BOOLINT yes=TRUE);
   virtual BOOLINT is_shown() const;

   virtual void focus();

   virtual ministring serialize();

   //! tell if image is imagery (cell centered)
   BOOLINT is_imagery() const
      {return(is_imagery_resp_elevation && !is_geolocated);}

   //! tell if image is elevation (grid centered)
   BOOLINT is_elevation() const
      {return(!is_imagery_resp_elevation && !is_geolocated);}

   //! tell if image is a geo-tagged photo
   BOOLINT is_photo() const
      {return(!is_geolocated);}

   //! get grid-centered extents
   virtual grid_extent get_extent()
      {return(extent);}

   //! get geographic extents of image
   virtual grid_extent get_geo_extent()
      {return(extent_geo);}

   //! mark object
   virtual void mark(BOOLINT yes=TRUE);
   virtual BOOLINT is_marked() const;

   //! set thumb via db format
   void set_thumb(const databuf *buf);

   //! set full-res texture via db format
   void set_fullres(const databuf *buf);

   protected:

   BOOLINT is_imagery_resp_elevation;
   BOOLINT is_geolocated;

   grid_extent extent_geo;
   int size_x,size_y;
   double size_ds,size_dt;
   double spacing;
   double spacing_ds,spacing_dt;

   Viewer *image_viewer;

   mininode_ref ecef_node;
   mininode_lod *lod_node;
   mininode_geometry *image_node;

   static mininode *image_groupnode;
   static mininode *deferred_groupnode1;
   static mininode *deferred_groupnode2;

   BOOLINT shown;
   BOOLINT marked;
   };

//! extent object
class Object_extent: public Object_extents
   {
   public:

   Object_extent(const ministring &name,
                 const grid_extent &extent,
                 Viewer *viewer=NULL);

   virtual ~Object_extent();

   virtual ministring get_info();

   virtual int initGFX();
   virtual void exitGFX();

   virtual void show(BOOLINT yes=TRUE);
   virtual BOOLINT is_shown() const;

   virtual void focus();

   virtual void updateGFX();

   virtual ministring serialize();
   static Object_extent *deserialize(ministring key,ministring info,Viewer *viewer);

   //! get extents
   grid_extent get_extent()
      {return(extent);}

   //! mark object
   virtual void mark(BOOLINT yes=TRUE);
   virtual BOOLINT is_marked() const;

   //! move object (via two handles)
   virtual void move(const minicoord &pos0,const minicoord &pos1);

   //! rotate object (via two handles)
   virtual void rotate(const minicoord &pos0,const minicoord &pos1);

   //! scale object horizontally (via two handles)
   virtual void scale_ds(const minicoord &pos0,const minicoord &pos1);

   //! scale object vertically (via two handles)
   virtual void scale_dt(const minicoord &pos0,const minicoord &pos1);

   protected:

   Viewer *extent_viewer;

   mininode_ecef *ecef_node;
   mininode_geometry *extent_node;

   static mininode *extent_groupnode;

   BOOLINT shown;
   BOOLINT marked;
   };

#endif
