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
   create_tube(miniv3d(0,0,0),miniv3d(0,0,height),
               miniv3d(0,0,1),miniv3d(0,0,1),
               miniv3d(1,0,0),
               radius,
               TRUE,TRUE,
               tessel);
   }

mininode_geometry_tube::mininode_geometry_tube(const miniv3d &p1,const miniv3d &p2,double radius,int tessel)
   : mininode_geometry(0,3,0)
   {
   miniv3d dir=p2-p1,right;
   if (dabs(dir.x)>dabs(dir.y) && dabs(dir.x)>dabs(dir.z)) right=miniv3d(0,0,dir.x);
   else if (dabs(dir.y)>dabs(dir.x) && dabs(dir.y)>dabs(dir.z)) right=miniv3d(0,0,dir.y);
   else right=miniv3d(dir.z,0,0);

   create_tube(p1,p2,
               dir,dir,
               right,
               radius,
               TRUE,TRUE,
               tessel);
   }

mininode_geometry_tube::mininode_geometry_tube(const minidyna<miniv3d> &p,double radius,
                                               BOOLINT start_cap,BOOLINT end_cap,
                                               int tessel)
   : mininode_geometry(0,3,0)
   {
   if (p.getsize()<2) return;

   miniv3d dir=p[1]-p[0],right;
   if (dabs(dir.x)>dabs(dir.y) && dabs(dir.x)>dabs(dir.z)) right=miniv3d(0,0,dir.x);
   else if (dabs(dir.y)>dabs(dir.x) && dabs(dir.y)>dabs(dir.z)) right=miniv3d(0,0,dir.y);
   else right=miniv3d(dir.z,0,0);

   for (unsigned int i=0; i<p.getsize()-1; i++)
      right=create_tube(p[i],p[i+1],
                        (i==0)?p[i+1]-p[i]:p[i+1]-p[i-1],(i==p.getsize()-2)?p[i+1]-p[i]:p[i+2]-p[i],
                        right,
                        radius,
                        (i==0)?start_cap:FALSE,(i==p.getsize()-2)?end_cap:FALSE,
                        tessel);
   }

miniv3d mininode_geometry_tube::create_tube(const miniv3d &start,const miniv3d &end,
                                            const miniv3d &start_dir,const miniv3d &end_dir,
                                            const miniv3d &start_right,
                                            double radius,
                                            BOOLINT start_cap,BOOLINT end_cap,
                                            int tessel)
   {
   miniv3d dir=end-start;
   dir.normalize();

   miniv3d right,up;
   up=start_right/dir;
   right=dir/up;
   right.normalize();
   right*=radius;

   miniv3d rot[3];
   rot_mtx(rot,-360.0/tessel,dir);

   beginstrip();

   for (int i=0; i<=tessel; i++)
      {
      setnrm(right);
      addvtx(project(start+right,dir,start,start_dir));
      addvtx(project(end+right,dir,end,end_dir));

      if (i<tessel) right=mlt_vec(rot,right);
      }

   if (start_cap)
      {
      beginstrip();
      setnrm(-start_dir);

      for (int i=0; i<=tessel; i++)
         {
         addvtx(start);
         addvtx(project(start+right,dir,start,start_dir));

         if (i<tessel) right=mlt_vec(rot,right);
         }
      }

   if (end_cap)
      {
      beginstrip();
      setnrm(end_dir);

      for (int i=0; i<=tessel; i++)
         {
         addvtx(project(end+right,dir,end,end_dir));
         addvtx(end);

         if (i<tessel) right=mlt_vec(rot,right);
         }
      }

   return(project(end+right,dir,end,end_dir)-end);
   }

miniv3d mininode_geometry_tube::project(const miniv3d &p,const miniv3d &d,
                                        const miniv3d &o,const miniv3d &n) const
   {
   miniv3d nrm=n;
   nrm.normalize();

   miniv3d dir=d;
   dir.normalize();

   double l=(p-o)*n;
   double c=n*d;

   if (c!=0.0) l/=-c;

   return(p+l*d);
   }
