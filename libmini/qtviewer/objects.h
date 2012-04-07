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

   object(const ministring &name="",const minicoord &crd=minicoord());

   ~object();

   ministring repo; // data repository
   ministring filename; // data file in repository

   minicoord coord; // barycenter

   protected:

   void *data; // data chunk
   unsigned int bytes; // data chunk length

   mininode *node; // node reference

   void load();
   };

//! object list
typedef minidyna<object *> object_list;

//! object container
class objects: public minikeyval<object>
   {
   public:

   //! default constructor
   objects();

   //! default destructor
   ~objects();

   void add(const ministring &key,const ministring &filename);
   object_list list(const ministring &tag);
   };

#endif
