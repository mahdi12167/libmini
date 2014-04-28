// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MININODE_PATH_H
#define MININODE_PATH_H

#include "mininodes.h"
#include "minipath.h"

//! a geometry node for geo-referenced paths
class mininode_geometry_path: public mininode_geometry
   {
   public:

   //! default constructor
   mininode_geometry_path();

   //! constructor
   mininode_geometry_path(ministring filename,double width=10.0);

   //! destructor
   virtual ~mininode_geometry_path();

   //! recreate geometry with a particular band width
   void recreate(double width);

   //! get path
   minipath *getpath()
      {return(&path_);}

   protected:

   minipath path_;
   };

//! a geometry node that implements C-LOD for geo-referenced paths
class mininode_geometry_path_clod: public mininode_geometry
   {
   public:

   //! default constructor
   mininode_geometry_path_clod();

   //! destructor
   virtual ~mininode_geometry_path_clod();

   //! load path
   void load(ministring filename);

   //! recreate geometry from actual view point
   void recreate(miniv3d eye,
                 double maxdiff,double atdist);

   //! get path
   minipath *getpath()
      {return(&path_);}

   protected:

   minipath path_;
   minidyna<float> d2_;
   minidyna<float> md_;

   protected:

   double calcdist(miniv3d a,miniv3d b,miniv3d p);

   void calcD2();
   float calcD2(int left,int right);

   void calcpath();
   void calcpath(int left,int right);

   private:

   miniv3d EYE_;
   float C_;
   };

#endif
