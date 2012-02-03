// (c) by Stefan Roettger

#include "minibase.h"

#include "minimath.h"

#include "minicam.h"

// default constructor
minicam::minicam(miniearth *earth,
                 double lat,double lon,double height,
                 double mindist,
                 float fovy,float aspect,
                 double nearp,double farp)
   {
   EARTH=earth;

   eye_default=minicoord(miniv3d(3600*lon,3600*lat,height),minicoord::MINICOORD_LLH);
   eye_default.convert2(minicoord::MINICOORD_ECEF);

   set_eye(EARTH->getinitial(),0.0,-90.0,mindist);
   set_lens(fovy,aspect,nearp,farp);
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

   get_local_base(eye,eye_dir,eye_right,eye_up);

   rotate_right(angle);
   rotate_up(pitch);

   move_above(mindist);
   }

void minicam::set_eye(double latitude,double longitude,double altitude,
                      double heading,double pitch,
                      double mindist)
   {
   set_eye(minicoord(miniv3d(latitude*3600.0,
                             longitude*3600.0,
                             altitude),minicoord::MINICOORD_LLH),heading,pitch,mindist);
   }

miniv3d minicam::get_eye_opengl()
   {return(EARTH->map_g2o(eye).vec);}

miniv3d minicam::get_dir_opengl()
   {return(EARTH->rot_g2o(eye_dir,eye));}

miniv3d minicam::get_right_opengl()
   {return(EARTH->rot_g2o(eye_right,eye));}

miniv3d minicam::get_up_opengl()
   {return(EARTH->rot_g2o(eye_up,eye));}

miniv3d minicam::get_down()
   {return(get_down(eye));}

double minicam::get_elev()
   {return(get_elev(eye));}

double minicam::get_dist()
   {return(get_dist(eye));}

minicoord minicam::get_hit()
   {return(get_hit(eye,eye_dir));}

double minicam::get_hitdist()
   {return(get_hitdist(eye,eye_dir));}

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

   if (dir*eye_dir<0.0) angle=PI-asin(s);
   else angle=asin(s);

   if (angle<0.0) angle+=2.0*PI;

   rotate_up(pitch);

   return(angle*180.0/PI);
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

   if (up*eye_up<0.0) pitch=-PI-asin(s);
   else pitch=asin(s);

   if (pitch<-PI) pitch+=2.0*PI;

   return(pitch*180.0/PI);
   }

void minicam::set_lens(float fovy,float aspect,
                       double nearp,double farp)
   {
   this->fovy=fovy;
   this->aspect=aspect;
   this->nearp=nearp;
   this->farp=farp;
   }

float minicam::get_fovy()
   {
   if (fovy<=0.0f) fovy=EARTH->get()->fovy;
   return(fovy);
   }

float minicam::get_aspect()
   {
   if (aspect<=0.0f) aspect=1.0f;
   return(aspect);
   }

double minicam::get_nearp()
   {
   if (nearp<=0.0) nearp=EARTH->get()->nearp;
   return(nearp);
   }

double minicam::get_farp()
   {
   if (farp<=0.0) farp=EARTH->get()->farp;
   return(farp);
   }

double minicam::get_cone()
   {
   double h=tan(0.5*get_fovy()*PI/180);
   double w=get_aspect()*h;
   return(sqrt(w*w+h*h));
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
   }

void minicam::move_plain(const miniv3d &delta)
   {eye+=delta;}

void minicam::move_forward(double delta)
   {move(delta*eye_dir);}

void minicam::move_forward_plain(double delta)
   {move_plain(delta*eye_dir);}

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

void minicam::move_above(double mindist)
   {move_above(eye,mindist);}

// rotate counter-clockwise
void minicam::rotate(double delta,const miniv3d &axis)
   {
   miniv3d rot[3];

   rot_mtx(rot,delta,axis);

   miniv3d dir(rot[0]*eye_dir, rot[1]*eye_dir, rot[2]*eye_dir);
   miniv3d right(rot[0]*eye_right, rot[1]*eye_right, rot[2]*eye_right);
   miniv3d up(rot[0]*eye_up, rot[1]*eye_up, rot[2]*eye_up);

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

void minicam::rotate_limit(double llimit,double ulimit)
   {
   double pitch;

   pitch=get_pitch();

   if (pitch<llimit) rotate_up(llimit-pitch);
   else if (pitch>ulimit) rotate_up(ulimit-pitch);
   }

miniv3d minicam::get_down(const minicoord &pos)
   {
   miniv3d dir,right,up;

   get_local_base(pos,dir,right,up);

   return(-up);
   }

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

      dist=shoot(pos0,-up,-miniearth::EARTH_radius);
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

      dist=shoot(pos0,dir0);

      if (dist==MAXFLOAT) hit=pos0;
      else
         {
         hit=pos0+dist*dir0;
         move_above(hit);
         }
      }

   return(hit);
   }

double minicam::get_hitdist(const minicoord &pos,const miniv3d &dir)
   {return((get_hit(pos,dir).vec-pos.vec).getlength());}

// move point up so that it is above ground
void minicam::move_above(minicoord &pos,double mindist)
   {
   double elev;
   double dist;

   minilayer *nst;
   minicoord posl;

   elev=get_elev(pos);
   nst=EARTH->getnearest(pos);

   if (elev!=-MAXFLOAT && nst!=NULL)
      {
      posl=nst->map_g2l(pos);
      posl.vec.z=dmax(posl.vec.z,nst->len_g2l(elev+mindist));
      pos=nst->map_l2g(posl);
      }
   else
      {
      dist=get_dist(pos);
      if (dist<mindist) move_down(dist-mindist);
      }
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

// unproject viewport coordinate
miniv3d minicam::unproject_viewport(int vx,int vy,
                                    int vwidth,int vheight)
   {
   double mx=(double)vx/(vwidth-1)-0.5;
   double my=0.5-(double)vy/(vheight-1);

   double wy=tan(fovy*PI/360.0);
   double wx=aspect*wy;

   miniv3d dir=get_dir()+
               get_right()*2.0*wx*mx+
               get_up()*2.0*wy*my;

   dir.normalize();

   return(dir);
   }

// shoot a ray at the scene
double minicam::shoot(const minicoord &o,const miniv3d &d,double hitdist)
   {return(EARTH->shoot(o,d,hitdist));}
