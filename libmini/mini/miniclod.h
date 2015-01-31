// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINICLOD_H
#define MINICLOD_H

#include "mini3D.h"
#include "minipath.h"

//! C-LOD core class for geo-referenced paths
class miniclod
   {
   public:

   struct point_struct
      {
      miniv3d pos;
      miniv3d nrm;
      miniv3d col;
      double wdt;
      };

   //! default constructor
   miniclod();

   //! destructor
   virtual ~miniclod();

   //! set path
   void set(const minipath &path);

   //! load path
   void load(ministring filename);

   //! create path geometry
   void create(miniv3d eye, // actual eye point
               double maxdev=1,double atdist=100, // maximum deviation at specific distance
               double maxwidth=1, // maximum width at specific distance
               double minv=0.0,double maxv=30.0,double sat=1.0,double val=1.0, // velocity color mapping
               double weight=1.0, // weight of color mapping
               int update=100); // vertices per update

   // create path geometry incrementally
   void create_inc(miniv3d eye); // actual eye point

   //! get path
   minipath *getpath()
      {return(&path_);}

   protected:

   struct state_struct
      {
      int left;
      int right;
      BOOLINT add;
      };

   minipath path_,path0_;

   minidyna<float> dc_; // constant deviations

   minidyna<float> d2_; // geometric deviations
   minidyna<float> dm_; // geometric maximum

   void updateDX();

   void calcDC();

   float calcD2(int left,int right,int center);
   float calcDM(int left,int right);

   void calcD2();
   float calcD2(int left,int right);

   void addpoint(const minimeas &m,BOOLINT start=FALSE);
   BOOLINT subdiv(int left,int right);

   void calcpath();
   void calcpath(int left,int right);

   void calcpath_inc(miniv3d eye,int update=100);
   void calcpath_inc();

   virtual miniv3d point2rgb(const minimeas &m,double v,float hue,float sat,float val);
   virtual void updated(const minidyna<mini3D::point_struct_band> &points) = 0;

   private:

   miniv3d EYE_,EYE0_;
   float C_,D_,W_;

   float MINV_,MAXV_,SAT_,VAL_;
   float WEIGHT_;

   BOOLINT UPDATED_;
   int UPDATE_;

   minidyna<struct state_struct> STACK_;
   minidyna<mini3D::point_struct_band> POINTS_;
   };

#endif
