// (c) by Stefan Roettger

#include "minibase.h"

#include "minicam.h"

// default constructor
minicam::minicam(miniearth *earth)
   {
   EARTH=earth;

   eye_default=minicoord(miniv3d(21.39,-157.72,30000),minicoord::MINICOORD_LLH);
   eye_default.convert2(minicoord::MINICOORD_ECEF);

   set_eye(EARTH->getinitial());

   hit_dist=MAXFLOAT;
   }

void minicam::set_eye(const minicoord &e)
   {
   eye=e;

   if (eye.type!=minicoord::MINICOORD_LINEAR)
      eye.convert2(minicoord::MINICOORD_ECEF);

   if (miniv3d(eye.vec)==miniv3d(0.0)) eye=eye_default;

   move_above(eye);
   get_local_base(eye,eye_dir,eye_right,eye_up);
   }

minicoord minicam::get_hit()
   {
   double dist;
   minicoord hit;

   if (hit_dist==MAXFLOAT)
      hit_dist=EARTH->shoot(eye,eye_dir);

   if (hit_dist==MAXFLOAT) dist=EARTH->get()->farp;
   else dist=hit_dist;

   hit=eye+dist*eye_dir;
   move_above(hit);

   return(hit);
   }

// destructor
minicam::~minicam() {}

// get base vectors of local coordinate system
void minicam::get_local_base(const minicoord &pos,
                             miniv3d &dir,miniv3d &right,miniv3d &up)
   {
   minilayer *nst;

   minicoord p,pl;

   p=pos;

   if (p.type!=minicoord::MINICOORD_LINEAR)
      p.convert2(minicoord::MINICOORD_ECEF);

   nst=EARTH->getnearest(p);

   if (nst!=NULL)
      {
      pl=nst->map_g2l(p);

      dir=nst->rot_l2g(miniv3d(0,0,-1),pl);
      right=nst->rot_l2g(miniv3d(1,0,0),pl);
      up=nst->rot_l2g(miniv3d(0,1,0),pl);
      }
   else
      {
      up=p.vec;
      up.normalize();

      if (up.getlength2()>0.0)
         {
         dir=miniv3d(0,1,0);
         right=up/dir;
         dir=right/up;
         }
      else
         {
         dir=miniv3d(0,1,0);
         right=miniv3d(1,0,0);
         up=miniv3d(0,0,1);
         }
      }
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
   }
