// (c) by Stefan Roettger

#ifndef MINICAM_H
#define MINICAM_H

#include "miniv3d.h"
#include "miniv4d.h"
#include "minicoord.h"
#include "miniearth.h"

//! geo-referenced camera
class minicam
   {
   public:

   //! default constructor
   minicam(miniearth *earth,
           double lat=21.39,double lon=-157.72,double height=30000.0);

   //! destructor
   ~minicam();

   void set_eye(const minicoord &e,double angle=0.0,double pitch=-90.0);

   minicoord get_eye() {return(eye);}
   miniv3d get_dir() {return(eye_dir);}
   miniv3d get_right() {return(eye_right);}
   miniv3d get_up() {return(eye_up);}
   minicoord get_hit();
   double get_angle();
   double get_pitch();

   void move(const miniv3d &delta);
   void move_forward(double delta);
   void move_right(double delta);
   void move_up(double delta);

   void rotate(double delta,const miniv3d &axis);
   void rotate_right(double delta);
   void rotate_up(double delta);

   protected:

   minicoord eye;
   miniv3d eye_dir,eye_right,eye_up;

   private:

   miniearth *EARTH;
   double hit_dist;

   minicoord eye_default;

   void get_local_base(const minicoord &pos,
                       miniv3d &dir,miniv3d &right,miniv3d &up);

   void move_above(minicoord &pos,double mindist=0.0);
   };

#endif
