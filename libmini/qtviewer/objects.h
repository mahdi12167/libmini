// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef OBJECTS_H
#define OBJECTS_H

#include <mini/minibase.h>
#include <mini/ministring.h>
#include <mini/minikeyval.h>
#include <mini/minicoord.h>
#include <mini/mininode.h>

//! object
class Object
   {
   public:

   Object(const ministring &name="",const ministring &repo="");
   virtual ~Object();

   ministring repository; // data repository
   ministring filename; // data file in repository

   BOOLINT is_valid();

   void load();
   void clear();

   minicoord get_coord() {return(coord);}
   double get_radius() {return(radius);}

   protected:

   BOOLINT valid;

   minicoord coord; // object barycenter
   double radius; // object radius

   mininode *node; // node reference

   virtual BOOLINT load_header(minicoord &c,double &radius) {return(FALSE);}
   virtual BOOLINT load_file() {}
   virtual void clear_file() {}
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
