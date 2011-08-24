// (c) by Stefan Roettger

#include "minibase.h"

#include "minicam.h"

// default constructor
minicam::minicam(miniearth *earth,
                 double lat,double lon,double height,
                 double mindist)
   {
   EARTH=earth;

   eye_default=minicoord(miniv3d(lat,lon,height),minicoord::MINICOORD_LLH);
   eye_default.convert2(minicoord::MINICOORD_ECEF);

   set_eye(EARTH->getinitial(),0.0,-90.0,mindist);
   }

// destructor
minicam::~minicam() {}

void minicam::set_eye(const minicoord &e,
                      double angle,double pitch,
                      double mindist)
   {
   eye=e;

   if (eye.type!=minicoord::MINICOORD_LINEAR)
      eye.convert2(minicoord::MINICOORD_ECEF);

   if (eye.vec.getlength2()==0.0) eye=eye_default;

   eye_mindist=mindist;

   get_local_base(eye,eye_dir,eye_right,eye_up);

   rotate_right(angle);
   rotate_up(pitch);

   move_above();
   }

miniv3d minicam::get_eye_opengl()
   {return(EARTH->map_g2o(eye).vec);}

miniv3d minicam::get_dir_opengl()
   {return(EARTH->rot_g2o(eye_dir,eye));}

miniv3d minicam::get_right_opengl()
   {return(EARTH->rot_g2o(eye_right,eye));}

miniv3d minicam::get_up_opengl()
   {return(EARTH->rot_g2o(eye_up,eye));}

double minicam::get_elev()
   {return(get_elev(eye));}

double minicam::get_dist()
   {return(get_dist(eye));}

minicoord minicam::get_hit()
   {return(get_hit(eye,eye_dir));}

double minicam::get_angle()
   {
   double angle,pitch;
   miniv3d dir,right,up;
   double s;

   pitch=get_pitch();
   rotate_up(-pitch);

   get_local_base(eye,dir,right,up);

   s=right*eye_dir;
   if (s<-1.0) s=-1.0;
   else if (s>1.0) s=1.0;

   if (dir*eye_dir<0.0) angle=M_PI-asin(s);
   else angle=asin(s);

   if (angle<0.0) angle+=2.0*M_PI;

   rotate_up(pitch);

   return(angle*180.0/M_PI);
   }

double minicam::get_pitch()
   {
   double pitch;
   miniv3d dir,right,up;
   double s;

   get_local_base(eye,dir,right,up);

   s=up*eye_dir;
   if (s<-1.0) s=-1.0;
   else if (s>1.0) s=1.0;

   if (up*eye_up<0.0) pitch=-M_PI-asin(s);
   else pitch=asin(s);

   if (pitch<-M_PI) pitch+=2.0*M_PI;

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

   right=eye_dir/up;
   right.normalize();
   dir=up/right;
   dir.normalize();

   eye_dir=dir;
   eye_right=right;
   eye_up=up;

   rotate_up(pitch);

   move_above();
   }

void minicam::move_forward(double delta)
   {move(delta*eye_dir);}

void minicam::move_back(double delta)
   {
   double pitch;

   pitch=get_pitch();
   rotate_up(-pitch);

   move(-delta*eye_dir);

   rotate_up(pitch);
   }

void minicam::move_right(double delta)
   {move(delta*eye_right);}

void minicam::move_left(double delta)
   {move(-delta*eye_right);}

void minicam::move_up(double delta)
   {move(delta*eye_up);}

void minicam::move_down(double delta)
   {
   miniv3d dir,right,up;

   get_local_base(eye,dir,right,up);

   move(-delta*up);
   }

void minicam::move_above()
   {move_above(eye,eye_mindist);}

// rotate counter-clockwise
void minicam::rotate(double delta,const miniv3d &axis)
   {
   double x=axis.x;
   double y=axis.y;
   double z=axis.z;

   double s=sin(delta/180.0*M_PI);
   double c=cos(delta/180.0*M_PI);
   double c1=1.0-c;

   miniv3d rotx(x*x*c1+c,   x*y*c1-z*s, x*z*c1+y*s);
   miniv3d roty(y*x*c1+z*s, y*y*c1+c,   y*z*c1-x*s);
   miniv3d rotz(z*x*c1-y*s, z*y*c1+x*s, z*z*c1+c);

   miniv3d dir(rotx*eye_dir, roty*eye_dir, rotz*eye_dir);
   miniv3d right(rotx*eye_right, roty*eye_right, rotz*eye_right);
   miniv3d up(rotx*eye_up, roty*eye_up, rotz*eye_up);

   dir.normalize();
   right.normalize();
   up.normalize();

   eye_dir=dir;
   eye_right=right;
   eye_up=up;
   }

void minicam::rotate_right(double delta)
   {
   double pitch;

   pitch=get_pitch();
   rotate_up(-pitch);

   rotate(-delta,eye_up);

   rotate_up(pitch);
   }

void minicam::rotate_up(double delta)
   {rotate(delta,eye_right);}

double minicam::get_elev(const minicoord &pos)
   {
   static minicoord pos0=minicoord();

   static double elev=-MAXFLOAT;

   if (pos!=pos0)
      {
      pos0=pos;

      if (pos0.type!=minicoord::MINICOORD_LINEAR)
         pos0.convert2(minicoord::MINICOORD_ECEF);

      elev=EARTH->getheight(pos0);
      }

   return(elev);
   }

double minicam::get_dist(const minicoord &pos)
   {
   static minicoord pos0=minicoord();

   static double dist=MAXFLOAT;

   miniv3d dir,right,up;

   if (pos!=pos0)
      {
      pos0=pos;

      if (pos0.type!=minicoord::MINICOORD_LINEAR)
         pos0.convert2(minicoord::MINICOORD_ECEF);

      get_local_base(pos0,dir,right,up);

      dist=EARTH->shoot(pos0,-up);

      if (dist==MAXFLOAT)
         dist=-EARTH->shoot(pos0,up);
      }

   return(dist);
   }

minicoord minicam::get_hit(const minicoord &pos,const miniv3d &dir)
   {
   static minicoord pos0=minicoord();
   static miniv3d dir0=miniv3d(0.0);

   static minicoord hit=minicoord();

   double dist;

   if (pos!=pos0 || dir!=dir0)
      {
      pos0=pos;
      dir0=dir;

      if (pos0.type!=minicoord::MINICOORD_LINEAR)
         pos0.convert2(minicoord::MINICOORD_ECEF);

      dist=EARTH->shoot(pos0,dir0);
      if (dist==MAXFLOAT) dist=0.0;

      hit=pos0+dist*dir0;
      move_above(hit);
      }

   return(hit);
   }

// get base vectors of local coordinate system
void minicam::get_local_base(const minicoord &pos,
                             miniv3d &dir,miniv3d &right,miniv3d &up)
   {
   static minicoord pos0=minicoord();
   static int mode0=WARPMODE_LINEAR;

   static miniv3d dir0=miniv3d(0.0),
                  right0=miniv3d(0.0),
                  up0=miniv3d(0.0);

   int mode;

   minilayer *ref;

   mode=EARTH->get()->warpmode;

   if (pos!=pos0 || mode!=mode0)
      {
      pos0=pos;
      mode0=mode;

      if (pos0.type!=minicoord::MINICOORD_LINEAR)
         pos0.convert2(minicoord::MINICOORD_ECEF);

      ref=EARTH->getreference();

      if (mode==WARPMODE_LINEAR ||
          mode==WARPMODE_FLAT ||
          (mode==WARPMODE_FLAT_REF && ref==NULL))
         // linear mode
         {
         dir0=miniv3d(0,1,0);
         right0=miniv3d(1,0,0);
         up0=miniv3d(0,0,1);
         }
      else if (mode==WARPMODE_FLAT_REF && ref!=NULL)
         // flat mode
         {
         up0=ref->getnormal();
         dir0=(ref->getnorth()-ref->getcenter()).vec;
         dir0.normalize();
         right0=dir0/up0;
         right0.normalize();
         dir0=up0/right0;
         dir0.normalize();
         }
      else
         // ECEF mode
         {
         up0=pos0.vec;
         up0.normalize();

         dir0=miniv3d(0,0,1);
         right0=dir0/up0;
         right0.normalize();
         if (right0.getlength2()==0.0) right0=miniv3d(0,1,0);
         dir0=up0/right0;
         dir0.normalize();
         }
      }

   dir=dir0;
   right=right0;
   up=up0;
   }

// move eye up so that it is above ground
void minicam::move_above(minicoord &pos,double mindist)
   {
   minilayer *nst;

   double elev;
   minicoord posl;

   if (pos.type!=minicoord::MINICOORD_LINEAR)
      pos.convert2(minicoord::MINICOORD_ECEF);

   nst=EARTH->getnearest(pos);

   if (nst!=NULL)
      {
      elev=EARTH->getheight(pos);

      if (elev!=-MAXFLOAT)
         {
         posl=nst->map_g2l(pos);
         posl.vec.z=dmax(posl.vec.z,nst->len_g2l(elev+mindist));
         pos=nst->map_l2g(posl);
         }
      }
   }
