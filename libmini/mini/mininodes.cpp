// (c) by Stefan Roettger

#include "mininodes.h"

double mininode_color::brightness=1.0;

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
