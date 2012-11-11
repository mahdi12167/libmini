// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef OBJECT_H
#define OBJECT_H

#include <mini/mini_generic.h>
#include <mini/mini_object.h>

//! object (base class)
class Object
   {
   public:

   //! default constructor
   Object(const ministring &name="",const ministring &repo="");

   //! destructor
   virtual ~Object();

   //! get full name of object
   //!  fullname=repository+filename
   ministring get_full_name() const;

   //! set full name of object
   //!  repository=empty filename=name
   void set_full_name(ministring name);

   //! get repository of object
   ministring get_relative_path() const;

   //! get filename of object relative to repository
   ministring get_relative_name() const;

   //! set repository of object
   void set_relative_path(ministring repo);

   //! set filename of object relative to repository
   //!  if filename starts with repository filename is truncated
   //!  if filename is absolute repository is cleared
   void set_relative_name(ministring name);

   //! check for repository path
   BOOLINT has_repo_path();

   //! check for absolute path
   static BOOLINT is_absolute_path(ministring path);

   //! normalize non-empty "path" to match "path/"
   static ministring normalize_path(ministring path);

   //! normalize "file://name" to match "name"
   static ministring normalize_file(ministring file);

   protected:

   ministring repository; // data repository
   ministring filename; // data file in repository

   virtual void set_center(minicoord c,double r=0.0);

   public:

   virtual minicoord get_center();
   virtual miniv3d get_normal();
   virtual double get_radius();

   //! textual info
   virtual ministring get_info();

   //! abstract method to be called upon construction of graphic representation
   virtual int initGFX() = 0;

   //! abstract method to be called upon destruction of graphic representation
   virtual void exitGFX() = 0;

   //! hide and show object
   virtual void show(BOOLINT yes=TRUE);

   //! tell if object is shown
   virtual BOOLINT is_shown() const;

   //! focus camera on object
   virtual void focus();

   //! serialize object
   virtual ministring serialize();

   //! deserialize object
   virtual Object *deserialize(ministring info) = 0;

   protected:

   minicoord coord; // object barycenter
   double radius; // object radius
   };

#endif
