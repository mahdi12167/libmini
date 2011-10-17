// (c) by Stefan Roettger

#include "mininodes.h"

double mininode_color::brightness=1.0;
miniv3d mininode_coord::lightdir=miniv3d(0,0,0);
BOOLINT mininode_coord::lightdirset=FALSE;

mininode_coord::mininode_coord(const minicoord &c)
   : mininode_transform()
   {
   minicoord ecef=c;
   if (ecef.type!=minicoord::MINICOORD_LINEAR) ecef.convert2(minicoord::MINICOORD_ECEF);

   miniv3d p=ecef.vec;
   miniv3d u=p;
   u.normalize();
   miniv3d d=miniv3d(0,0,1);
   miniv3d r=d/u;
   r.normalize();
   if (r.getlength2()==0.0) r=miniv3d(0,1,0);
   d=u/r;
   d.normalize();

   miniv4d mtx[3]={miniv4d(r.x,d.x,u.x,p.x),miniv4d(r.y,d.y,u.y,p.y),miniv4d(r.z,d.z,u.z,p.z)};
   mtxget(mtx,oglmtx);

   up=u;
   }

void mininode_coord::set_lightdir(const miniv3d &d)
   {
   lightdir=d;
   lightdirset=(d.getlength2()>0.0);
   }

void mininode_coord::traverse_pre()
   {
   if (lightdirset)
      {
      double l=up*lightdir;
      if (l<0.0) l=0.0;
      l=1.0-pow(1.0-l,10.0);

      mininode_color::set_brightness(0.5*l+0.5);
      }

   mininode_transform::traverse_pre();
   }

void mininode_coord::traverse_post()
   {
   if (lightdirset)
      mininode_color::set_brightness(1.0);

   mininode_transform::traverse_post();
   }

mininode_geometry_tube::mininode_geometry_tube(double radius,double height,int tessel)
   : mininode_geometry(0,3,0)
   {
   for (int i=0; i<=tessel; i++)
      {
      double w=2*PI*i/tessel;
      double x=sin(w)*radius;
      double y=cos(w)*radius;

      setnrm(miniv3d(x,y,0));
      addvtx(miniv3d(x,y,0));
      addvtx(miniv3d(x,y,height));
      }

   beginstrip();
   setnrm(miniv3d(0,0,-1));

   for (int i=0; i<=tessel; i++)
      {
      double w=2*PI*i/tessel;
      double x=sin(w)*radius;
      double y=cos(w)*radius;

      addvtx(miniv3d(0,0,0));
      addvtx(miniv3d(x,y,0));
      }

   beginstrip();
   setnrm(miniv3d(0,0,1));

   for (int i=0; i<=tessel; i++)
      {
      double w=2*PI*i/tessel;
      double x=sin(w)*radius;
      double y=cos(w)*radius;

      addvtx(miniv3d(x,y,height));
      addvtx(miniv3d(0,0,height));
      }
   }

mininode_geometry_tube::mininode_geometry_tube(const miniv3d &p1,const miniv3d &p2,double radius,int tessel)
   : mininode_geometry(0,3,0)
   {
   miniv3d dir=p2-p1;
   dir.normalize();

   miniv3d right,up;
   if (dabs(dir.x)>dabs(dir.y) && dabs(dir.x)>dabs(dir.z)) right=miniv3d(0,0,dir.x);
   else if (dabs(dir.y)>dabs(dir.x) && dabs(dir.y)>dabs(dir.z)) right=miniv3d(0,0,dir.y);
   else right=miniv3d(dir.z,0,0);
   up=right/dir;
   right=dir/up;
   right.normalize();
   right*=radius;

   miniv3d rot[3];
   rot_mtx(rot,-360.0/tessel,dir);

   for (int i=0; i<=tessel; i++)
      {
      setnrm(right);
      addvtx(p1+right);
      addvtx(p2+right);

      right=mlt_vec(rot,right);
      }

   beginstrip();
   setnrm(miniv3d(0,0,-1));

   for (int i=0; i<=tessel; i++)
      {
      addvtx(p1);
      addvtx(p1+right);

      right=mlt_vec(rot,right);
      }

   beginstrip();
   setnrm(miniv3d(0,0,1));

   for (int i=0; i<=tessel; i++)
      {
      addvtx(p2+right);
      addvtx(p2);

      right=mlt_vec(rot,right);
      }
   }
