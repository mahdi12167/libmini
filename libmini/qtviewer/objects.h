// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef OBJECTS_H
#define OBJECTS_H

#include <mini/mini_generic.h>
#include <mini/mini_tileset.h>
#include <mini/mini_object.h>

//! object (base class)
class Object
   {
   public:

   Object(const ministring &name="",const ministring &repo="");
   virtual ~Object();

   ministring repository; // data repository
   ministring filename; // data file in repository

   BOOLINT is_valid();

   minicoord get_coord() {return(coord);}
   double get_radius() {return(radius);}

   protected:

   BOOLINT valid;

   minicoord coord; // object barycenter
   double radius; // object radius

   mininode *node; // node reference
   };

//! tileset object
class Object_tileset: public Object
   {
   public:

   Object_tileset(const ministring &name="",const ministring &repo="",
                  minilayer *layer=NULL)
      : Object(name,repo)
      {tileset_layer=layer;}

   virtual ~Object_tileset() {}

   protected:

   minilayer *tileset_layer;
   };

//! object list
typedef minidyna<Object **> Object_list;

//! object container
class Objects: public minikeyval<Object *>
   {
   public:

   //! default constructor
   Objects();

   //! default destructor
   ~Objects();

   void add(Object *obj);
   void clear();

   Object *get(const ministring &key);
   Object_list list(const ministring &tag);
   Object_list list(const ministrings &tags);
   };

#endif
