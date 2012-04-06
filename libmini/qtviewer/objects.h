// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef OBJECTS_H
#define OBJECTS_H

#include <mini/ministring.h>
#include <mini/minikeyval.h>

//! object
class object
   {
   public:

   object()
      {
      filename="";
      data=NULL;
      }

   ~object() {}

   ministring filename;
   void *data;
   };

//! object list
class objects: public minikeyval<object>
   {
   public:

   //! default constructor
   objects();

   //! default destructor
   ~objects() {}
   };

#endif
