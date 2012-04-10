// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef OBJECTS_H
#define OBJECTS_H

#include <mini/minibase.h>
#include <mini/ministring.h>
#include <mini/minikeyval.h>
#include <mini/minicoord.h>
#include <mini/mininode.h>

//! object
class object
   {
   public:

   object(const ministring &name="",const ministring &repo="");
   virtual ~object();

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
typedef minidyna<object **> object_list;

//! object container
class objects: public minikeyval<object *>
   {
   public:

   //! default constructor
   objects();

   //! default destructor
   ~objects();

   void add(object *obj);
   object_list list(const ministring &tag);
   };

#endif
