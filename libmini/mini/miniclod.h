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

   //! set paths
   void set(const minipaths &paths);

   //! append to paths
   void append(const minipath &path);

   //! read path
   void read(const std::string &csv,
             double max_delta=MAXFLOAT, // maximum continuous point distance
             double max_length=MAXFLOAT, // maximum continuous time difference
             double min_accuracy=MAXFLOAT, // minimum required measurement accuracy
             int orb=minicoord::MINICOORD_ORB_NONE); // path orbital

   //! read paths
   void read(const std::vector<std::string> &csvs,
             double max_delta=MAXFLOAT, // maximum continuous point distance
             double max_length=MAXFLOAT, // maximum continuous time difference
             double min_accuracy=MAXFLOAT, // minimum required measurement accuracy
             int orb=minicoord::MINICOORD_ORB_NONE); // path orbital

   //! append to paths
   void append(const std::string &csv);

   //! load path
   void load(ministring filename,
             double max_delta=MAXFLOAT, // maximum continuous point distance
             double max_length=MAXFLOAT, // maximum continuous time difference
             double min_accuracy=MAXFLOAT, // minimum required measurement accuracy
             int orb=minicoord::MINICOORD_ORB_NONE); // path orbital

   //! clear path
   void clear();

   //! create path geometry
   void create(vec3 eye, // actual eye point
               double maxdev=1,double atdist=100, // maximum deviation at specific distance
               double maxwidth=1, // maximum width at specific distance
               double minv=0.0,double maxv=30.0,double sat=1.0,double val=1.0, // velocity color mapping
               double weight=1.0, // weight of color mapping
               double start=1.0, // weight of start points
               int update=100); // vertices per update

   //! create path geometry incrementally
   void create_inc(vec3 eye); // actual eye point

   //! is the path creation idling?
   BOOLINT idle()
      {return(STACK_.empty());}

   //! get actual path
   minipath *getpath()
      {return(&path_);}

   protected:

   struct state_struct
      {
      int left;
      int right;
      BOOLINT add;
      };

   minipath path_; // actual concatenated path
   minipath path0_; // new path to be concatenated
   minipath paths0_; // new concatenated path sequence

   std::vector<float> dc_; // constant deviations

   std::vector<float> d2_; // geometric deviations
   std::vector<float> dm_; // geometric maximum

   BOOLINT updateDX();

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
   float WEIGHT_,START_;

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
      : miniCLOD(),
      updated_(false)
      {}

   virtual void updated(const std::vector<mini3D::joint_struct> &points)
      {
      points_=points;
      updated_=true;
      }

   const std::vector<mini3D::joint_struct> *getPoints()
      {
      updated_=false;
      return(&points_);
      }

   BOOLINT isupdated()
      {return(updated_);}

   protected:

   std::vector<mini3D::joint_struct> points_;
   BOOLINT updated_;
   };

#endif
