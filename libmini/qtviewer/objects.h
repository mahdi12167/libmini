// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef OBJECTS_H
#define OBJECTS_H

#include <mini/mini_generic.h>
#include <mini/mini_format.h>
#include <mini/mini_tileset.h>
#include <mini/mini_object.h>

#include <grid/grid.h>

#include "object.h"

class Viewer;

enum
   {
   OBJECT_SUCCESS=0,
   OBJECT_FAILURE=1,
   OBJECT_NOT_REFERENCED=2,
   OBJECT_TOO_LARGE=3
   };

//! tileset object
class Object_tileset: public Object
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

   protected:

   Viewer *tileset_viewer;
   minilayer *tileset_layer;

   BOOLINT shown;
   };

//! image object
class Object_image: public Object
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

   //! tell if image is imagery (cell centered)
   BOOLINT is_imagery() const
      {return(is_imagery_resp_elevation);}

   //! tell if image is elevation (grid centered)
   BOOLINT is_elevation() const
      {return(!is_imagery_resp_elevation);}

   //! get geographic extents of image
   grid_extent get_extent() const
      {return(extent);}

   //! get grid-centered extents
   grid_extent get_grid_extent() const
      {return(extent_grid);}

   //! set thumb via db format
   void set_thumb(const databuf *buf);

   protected:

   BOOLINT is_imagery_resp_elevation;

   grid_extent extent;
   grid_extent extent_grid;
   int size_x,size_y;
   double size_ds,size_dt;
   double spacing;

   Viewer *image_viewer;
   mininode_geometry *image_node;

   static mininode *image_groupnode;
   static mininode *deferred_groupnode1;
   static mininode *deferred_groupnode2;

   BOOLINT shown;
   };

//! extent object
class Object_extent: public Object
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

   //! get grid-centered extents of image
   grid_extent get_extent() const
      {return(extent);}

   protected:

   grid_extent extent;

   Viewer *extent_viewer;
   mininode_geometry *extent_node;

   static mininode *extent_groupnode;

   BOOLINT shown;
   };

#endif
