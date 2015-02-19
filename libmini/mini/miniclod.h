// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINICLOD_H
#define MINICLOD_H

#include "mini3D.h"
#include "minipath.h"

#include "glslmath.h"

//! C-LOD core class for geo-referenced paths
class miniCLOD
   {
   public:

   //! default constructor
   miniCLOD();

   //! destructor
   virtual ~miniCLOD();

   //! set path
   void set(const minipath &path);

   //! load path
   void load(ministring filename);

   //! create path geometry
   void create(vec3 eye, // actual eye point
               double maxdev=1,double atdist=100, // maximum deviation at specific distance
               double maxwidth=1, // maximum width at specific distance
               double minv=0.0,double maxv=30.0,double sat=1.0,double val=1.0, // velocity color mapping
               double weight=1.0, // weight of color mapping
               int update=100); // vertices per update

   // create path geometry incrementally
   void create_inc(vec3 eye); // actual eye point

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

   std::vector<float> dc_; // constant deviations

   std::vector<float> d2_; // geometric deviations
   std::vector<float> dm_; // geometric maximum

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

   void calcpath_inc(vec3 eye,int update=100);
   void calcpath_inc();

   double distance2line(vec3 p,vec3 a,vec3 b);
   vec3f hsv2rgb(float hue,float sat,float val);

   virtual vec3f point2rgb(const minimeas &m,double v,float hue,float sat,float val);
   virtual void updated(const std::vector<mini3D::joint_struct> &points) = 0;

   private:

   vec3 EYE_,EYE0_;
   float C_,D_,W_;

   float MINV_,MAXV_,SAT_,VAL_;
   float WEIGHT_;

   BOOLINT UPDATED_;
   int UPDATE_;

   std::vector<struct state_struct> STACK_;
   std::vector<mini3D::joint_struct> POINTS_;
   };

//! C-LOD container class
class miniCLODcontainer: public miniCLOD
   {
   public:

   //! default constructor
   miniCLODcontainer()
      : miniCLOD()
      {}

   virtual void updated(const std::vector<mini3D::joint_struct> &points)
      {points_ = points;}

   const std::vector<mini3D::joint_struct> *getPoints()
      {return(&points_);}

   protected:

   std::vector<mini3D::joint_struct> points_;
   };

#endif
