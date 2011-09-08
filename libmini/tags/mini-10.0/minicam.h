// (c) by Stefan Roettger

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
           double lat=21.39,double lon=-157.72,double height=1E6,
           double mindist=0.0);

   //! destructor
   ~minicam();

   void set_eye(const minicoord &e,
                double angle=0.0,double pitch=-90.0,
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

   protected:

   minicoord eye;
   miniv3d eye_dir,eye_right,eye_up;

   private:

   miniearth *EARTH;

   minicoord eye_default;
   };

#endif
