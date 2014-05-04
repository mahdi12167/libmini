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

   //! zscale
   static float zscale;

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

   //! create geometry from actual view point
   void create(double maxdev=1,double atdist=100, // maximum deviation at specific distance
               double maxwidth=1, // maximum width at specific distance
               double minv=0.0,double maxv=30.0,double sat=1.0,double val=1.0, // velocity color mapping
               double weight=1.0, // weight of color mapping
               int update=100); // vertices per update

   //! get path
   minipath *getpath()
      {return(&path_);}

   //! zscale
   static float zscale;

   protected:

   minipath path_;

   minidyna<float> dc_; // constant deviations

   minidyna<float> d2_; // geometric deviations
   minidyna<float> dm_; // geometric maximum

   void calcDC();

   float calcD2(int left,int right,int center);
   float calcDM(int left,int right);

   void calcD2();
   float calcD2(int left,int right);

   void addpoint(miniv3d p,double v,BOOLINT start=FALSE);
   BOOLINT subdiv(int left,int right);

   void calcpath();
   void calcpath(int left,int right);

   void calcpath_inc(int update);
   void calcpath_inc();

   virtual void traverse_pre()
      {
      if (UPDATE_>0) calcpath_inc(UPDATE_); // update path incrementally
      mininode_geometry::traverse_pre(); // traverse base class
      }

   private:

   struct state_struct
      {
      int left;
      int right;
      BOOLINT add;
      };

   miniv3d EYE_;
   float C_,D_,W_;

   float MINV_,MAXV_,SAT_,VAL_;
   float WEIGHT_;

   BOOLINT UPDATED_;
   int UPDATE_;

   minidyna<struct state_struct> STACK_;

   minidyna<miniv3d> POS_;
   minidyna<miniv3d> NRM_;
   minidyna<miniv3d> COL_;
   minidyna<double> WDT_;
   };

#endif
