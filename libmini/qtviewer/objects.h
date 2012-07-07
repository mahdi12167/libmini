// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef OBJECTS_H
#define OBJECTS_H

#include <mini/mini_generic.h>
#include <mini/mini_tileset.h>
#include <mini/mini_object.h>

#include <grid/grid.h>

class Viewer;

//! object (base class)
class Object
   {
   public:

   Object(const ministring &name="",const ministring &repo="");
   virtual ~Object();

   ministring get_full_name() const;
   void set_full_name(ministring name);

   ministring get_relative_name() const;
   ministring get_relative_path() const;

   void set_relative_name(ministring name);
   void set_relative_path(ministring repo);

   protected:

   ministring repository; // data repository
   ministring filename; // data file in repository

   virtual void set_center(minicoord c,double r=0.0);

   public:

   virtual minicoord get_center();
   virtual miniv3d get_normal();
   virtual double get_radius();
   virtual ministring get_info();

   virtual BOOLINT initGFX() = 0;
   virtual void exitGFX() = 0;

   virtual void show(BOOLINT yes=TRUE);
   virtual BOOLINT is_shown() const;

   virtual void focus() {}

   protected:

   minicoord coord; // object barycenter
   double radius; // object radius
   };

//! tileset object
class Object_tileset: public Object
   {
   public:

   Object_tileset(const ministring &name="",const ministring &repo="",
                  Viewer *viewer=NULL);

   virtual ~Object_tileset();

   virtual ministring get_info();

   virtual BOOLINT initGFX();
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

   virtual BOOLINT initGFX();
   virtual void exitGFX();

   virtual void show(BOOLINT yes=TRUE);
   virtual BOOLINT is_shown() const;

   virtual void focus();

   BOOLINT is_imagery() const
      {return(is_imagery_resp_elevation);}

   BOOLINT is_elevation() const
      {return(!is_imagery_resp_elevation);}

   grid_extent get_extent() const
      {return(extent);}

   protected:

   BOOLINT is_imagery_resp_elevation;

   grid_extent extent;
   int size_x,size_y;
   double size_ds,size_dt;

   Viewer *image_viewer;
   mininode_geometry *image_node;

   static mininode *image_groupnode;

   BOOLINT shown;
   };

//! object container
class Objects: public minikeyval<Object *>
   {
   public:

   //! default constructor
   Objects();

   //! default destructor
   ~Objects();

   BOOLINT add(const ministring &key,Object *obj,const ministring &tag);
   BOOLINT add(const ministring &key,Object *obj,const ministrings &tags);
   unsigned int get_num() const;
   Object *get(unsigned int i);
   Object *get(const ministring &key);
   ministrings *get_tags(const ministring &key);
   void add_tag(ministring key,ministring tag);
   void remove_tag(ministring key,ministring tag);
   BOOLINT has_tag(ministring key,ministring tag);
   ministrings list();
   ministrings list(const ministring &tag);
   ministrings list(const ministrings &tags);
   ministrings list_wo(const ministring &tag);
   ministrings list_wo(const ministrings &tags);
   void set_repo(const ministring &repo);
   void remove(const ministring &key);
   void remove(const ministrings &keys);
   void clear();
   };

#endif
