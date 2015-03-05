// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MININODE_CLOD_H
#define MININODE_CLOD_H

#include "mininodes.h"
#include "minipath.h"
#include "miniclod.h"

//! a geometry node that implements C-LOD for geo-referenced paths
class mininode_geometry_clod: public mininode_geometry, public miniCLOD
   {
   public:

   //! default constructor
   mininode_geometry_clod();

   //! destructor
   virtual ~mininode_geometry_clod();

   //! set path
   void set(const minipath &path);

   //! read path
   void read(const std::string &csv,
             double max_delta=MAXFLOAT, // maximum continuous point distance
             double max_length=MAXFLOAT, // maximum continuous time difference
             double min_accuracy=MAXFLOAT, // minimum required measurement accuracy
             int orb=minicoord::MINICOORD_ORB_NONE); // path orbital

   //! load path
   void load(ministring filename,
             double max_delta=MAXFLOAT, // maximum continuous point distance
             double max_length=MAXFLOAT, // maximum continuous time difference
             double min_accuracy=MAXFLOAT, // minimum required measurement accuracy
             int orb=minicoord::MINICOORD_ORB_NONE); // path orbital

   //! create geometry from actual view point
   void create(double maxdev=1,double atdist=100, // maximum deviation at specific distance
               double maxwidth=1, // maximum width at specific distance
               double minv=0.0,double maxv=30.0,double sat=1.0,double val=1.0, // velocity color mapping
               double weight=1.0, // weight of color mapping
               int update=100); // vertices per update

   //! set zscale
   void set_zscale(float s=0.975f)
      {zscale=s;}

   //! serialize node to string
   virtual ministring to_string();

   //! deserialize node from string
   virtual BOOLINT from_string(ministring &info);

   protected:

   float zscale;

   virtual void traverse_pre();

   virtual void updated(const std::vector<mini3D::joint_struct> &points);
   };

#endif
