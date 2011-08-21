// (c) by Stefan Roettger

#include "minibase.h"

#include "minicam.h"

// default constructor
minicam::minicam(miniearth *earth,
                 double lat,double lon,double height)
   {
   EARTH=earth;

   eye_default=minicoord(miniv3d(lat,lon,height),minicoord::MINICOORD_LLH);
   eye_default.convert2(minicoord::MINICOORD_ECEF);

   set_eye(EARTH->getinitial());

   hit_dist=MAXFLOAT;
   }

// destructor
minicam::~minicam() {}

void minicam::set_eye(const minicoord &e)
   {
   eye=e;

   if (eye.type!=minicoord::MINICOORD_LINEAR)
      eye.convert2(minicoord::MINICOORD_ECEF);

   if (eye.vec.getlength2()==0.0) eye=eye_default;

   move_above(eye);
   get_local_base(eye,eye_dir,eye_right,eye_up);

   hit_dist=MAXFLOAT;
   }

minicoord minicam::get_hit()
   {
   double dist;
   minicoord hit;

   if (hit_dist==MAXFLOAT)
      {
      hit_dist=EARTH->shoot(eye,eye_dir);

      if (hit_dist==MAXFLOAT) dist=EARTH->get()->farp;
      else dist=hit_dist;

      hit=eye+dist*eye_dir;
      move_above(hit);
      }

   return(hit);
   }

double minicam::get_angle()
   {
   double angle,pitch;
   miniv3d dir,right,up;

   pitch=get_pitch();
   rotate_up(-pitch);

   get_local_base(eye,dir,right,up);

   if (dir*eye_dir<0.0) angle=M_PI-asin(right*eye_dir);
   else angle=asin(right*eye_dir);

   if (angle>M_PI) angle-=2.0*M_PI;

   rotate_up(pitch);

   return(angle*180.0/M_PI);
   }

double minicam::get_pitch()
   {
   double pitch;
   miniv3d dir,right,up;

   get_local_base(eye,dir,right,up);

   if (dir*eye_dir<0.0) pitch=M_PI-asin(up*eye_dir);
   else pitch=asin(up*eye_dir);

   if (pitch>M_PI) pitch-=2.0*M_PI;

   return(pitch*180.0/M_PI);
   }

void minicam::move(const miniv3d &delta)
   {
   double pitch;
   miniv3d dir,right,up;

   pitch=get_pitch();
   rotate_up(-pitch);

   eye+=delta;

   get_local_base(eye,dir,right,up);

   right=up/eye_dir;
   dir=right/up;

   eye_dir=dir;
   eye_right=right;
   eye_up=up;

   rotate_up(pitch);

   hit_dist=MAXFLOAT;
   }

void minicam::move_forward(double delta)
   {move(delta*eye_dir);}

void minicam::move_right(double delta)
   {move(delta*eye_right);}

void minicam::move_up(double delta)
   {move(delta*eye_up);}

void minicam::rotate(double delta,const miniv3d &axis)
   {
   double x=axis.x;
   double y=axis.y;
   double z=axis.z;

   double s=sin(delta/180.0*M_PI);
   double c=cos(delta/180.0*M_PI);

   miniv3d rotx(x*x*(1-c)+c, x*y*(1-c)-z*s, x*z*(1-c)+y*s);
   miniv3d roty(y*x*(1-c)+z*s, y*y*(1-c)+c, y*z*(1-c)-x*s);
   miniv3d rotz(x*z*(1-c)-y*s, y*z*(1-c)+x*s, z*z*(1-c)+c);

   eye_dir=miniv3d(rotx*eye_dir, roty*eye_dir, rotz*eye_dir);
   eye_right=miniv3d(rotx*eye_right, roty*eye_right, rotz*eye_right);
   eye_up=miniv3d(rotx*eye_up, roty*eye_up, rotz*eye_up);
   }

void minicam::rotate_right(double delta)
   {rotate(delta,eye_up);}

void minicam::rotate_up(double delta)
   {rotate(delta,eye_right);}

void minicam::tilt(double delta)
   {rotate(delta,eye_dir);}

// get base vectors of local coordinate system
void minicam::get_local_base(const minicoord &pos,
                             miniv3d &dir,miniv3d &right,miniv3d &up)
   {
   static minicoord pos0=minicoord();

   static miniv3d dir0=miniv3d(0.0),
                  right0=miniv3d(0.0),
                  up0=miniv3d(0.0);

   minilayer *nst;
   int mode;

   minicoord posl;

   if (pos!=pos0)
      {
      pos0=pos;

      if (pos0.type!=minicoord::MINICOORD_LINEAR)
         pos0.convert2(minicoord::MINICOORD_ECEF);

      nst=EARTH->getnearest(pos0);
      mode=EARTH->get()->warpmode;

      if (nst!=NULL)
         {
         posl=nst->map_g2l(pos0);

         dir0=nst->rot_l2g(miniv3d(0,0,-1),posl);
         right0=nst->rot_l2g(miniv3d(1,0,0),posl);
         up0=nst->rot_l2g(miniv3d(0,1,0),posl);
         }
      else
         if (mode==0 || mode==1 || mode==2)
            // linear modes
            {
            dir0=miniv3d(0,1,0);
            right0=miniv3d(1,0,0);
            up0=miniv3d(0,0,1);
            }
         else
            // ECEF:
            // z axis is earth axis
            // x axis hits prime meridian (lon=0)
            // y axis hits lon=90 meridian
            {
            up0=pos0.vec;
            up0.normalize();

            dir0=miniv3d(0,0,1);
            right0=up0/dir0;
            if (right0.getlength2()==0.0) right0=miniv3d(0,1,0);
            dir0=right0/up0;
            }
      }

   dir=dir0;
   right=right0;
   up=up0;
   }

// move eye up so that it is above ground
void minicam::move_above(minicoord &pos,double mindist)
   {
   double dist;
   double elev;

   miniv3d dir,right,up;

   minicoord posl;

   if (pos.type!=minicoord::MINICOORD_LINEAR)
      pos.convert2(minicoord::MINICOORD_ECEF);

   get_local_base(pos,dir,right,up);

   dist=EARTH->shoot(pos,-up);

   if (dist==MAXFLOAT)
      {
      dist=EARTH->shoot(pos,up);
      if (dist!=MAXFLOAT)
         pos+=up*(mindist+dist);
      }
   else
      if (dist<mindist)
         pos+=up*(mindist-dist);

   elev=EARTH->getheight(pos);

   if (elev!=-MAXFLOAT)
      {
      minilayer *nst=EARTH->getnearest(pos);

      if (nst!=NULL)
         {
         posl=nst->map_g2l(pos);
         posl.vec.z=dmax(posl.vec.z,nst->len_g2l(elev+mindist));
         pos=nst->map_l2g(posl);
         }
      }

   hit_dist=MAXFLOAT;
   }
