// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINICAM_H
#define MINICAM_H

#include "miniv3d.h"
#include "miniv4d.h"
#include "minicoord.h"
#include "miniearth.h"

//! geo-referenced camera in ECEF coordinates
//! ECEF:
//! earth-centered earth-fixed
//! right-handed coordinate system
//! z axis is earth axis
//! x axis hits prime meridian (lon=0)
//! y axis hits lon=90 meridian
class minicam
   {
   public:

   //! default constructor
   minicam(miniearth *earth,
           double lat=21.39,double lon=-157.72,double height=7E6,
           double mindist=0.0,
           float fovy=0.0f,float aspect=0.0f,
           double nearp=0.0,double farp=0.0);

   //! destructor
   virtual ~minicam();

   void set_eye(const minicoord &e,
                double angle=0.0,double pitch=-90.0,
                double mindist=0.0);

   void set_eye(double latitude,double longitude,double altitude,
                double heading,double pitch,
                double mindist=0.0);

   minicoord get_eye() {return(eye);}
   miniv3d get_eye_opengl();
   miniv3d get_dir() {return(eye_dir);}
   miniv3d get_dir_opengl();
   miniv3d get_right() {return(eye_right);}
   miniv3d get_right_opengl();
   miniv3d get_up() {return(eye_up);}
   miniv3d get_up_opengl();
   miniv3d get_down();
   double get_elev();
   double get_dist();
   minicoord get_hit();
   double get_hitdist();
   double get_angle();
   double get_pitch();

   void set_lens(float fovy,float aspect,
                 double nearp,double farp);

   float get_fovy();
   float get_aspect();
   double get_nearp();
   double get_farp();
   double get_cone();

   void move(const miniv3d &delta);
   void move_plain(const miniv3d &delta);
   void move_forward(double delta);
   void move_forward_plain(double delta);
   void move_back(double delta);
   void move_right(double delta);
   void move_left(double delta);
   void move_up(double delta);
   void move_down(double delta);
   void move_above(double mindist);

   void rotate(double delta,const miniv3d &axis);
   void rotate_right(double delta);
   void rotate_up(double delta);
   void rotate_limit(double llimit=-90.0,double ulimit=90.0);

   miniv3d get_down(const minicoord &pos);
   double get_elev(const minicoord &pos);
   double get_dist(const minicoord &pos);
   minicoord get_hit(const minicoord &pos,const miniv3d &dir);
   double get_hitdist(const minicoord &pos,const miniv3d &dir);

   void move_above(minicoord &pos,double mindist=0.0);

   void get_local_base(const minicoord &pos,
                       miniv3d &dir,miniv3d &right,miniv3d &up);

   miniv3d unproject_viewport(int vx,int vy,
                              int vwidth,int vheight);

   double get_earth_radius()
      {return(EARTH->getradius());}

   void get_ecef_matrix(miniv4d matrix[3])
      {EARTH->get_ecef_matrix(matrix);}

   protected:

   minicoord eye;
   miniv3d eye_dir,eye_right,eye_up;

   float fovy,aspect;
   double nearp,farp;

   virtual double shoot(const minicoord &o,const miniv3d &d,double mindist=0.0);

   private:

   miniearth *EARTH;

   minicoord eye_default;
   };

#endif
