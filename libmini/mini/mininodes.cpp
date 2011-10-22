// (c) by Stefan Roettger

#include "mininodes.h"

double mininode_color::brightness=1.0;
miniv3d mininode_coord::lightdir=miniv3d(0,0,0);
BOOLINT mininode_coord::lightdirset=FALSE;

void mininode_transform::optimize()
   {
   while (get_children()==1 &&
          get_child()->get_id()==MININODE_TRANSFORM)
      {
      // get child transformation
      mininode_transform *transform=(mininode_transform *)get_child();

      // multiply with child's transformation matrix
      miniv4d mtx[3],mtx1[3],mtx2[3];
      mtxget(oglmtx,mtx1);
      mtxget(transform->oglmtx,mtx2);
      mlt_mtx(mtx,mtx2,mtx1);
      mtxget(mtx,oglmtx);

      // remove child
      remove_child();
      }

   mininode::optimize();
   }

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

miniv3d mininode_geometry::project(const miniv3d &p,const miniv3d &d,
                                   const miniv3d &o,const miniv3d &n)
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

miniv3d mininode_geometry::get_halfdir(const miniv3d &dir1,const miniv3d &dir2)
   {
   miniv3d d1=dir1,d2=dir2;

   d1.normalize();
   d2.normalize();

   return(d1+d2);
   }

miniv3d mininode_geometry::get_right(const miniv3d &dir)
   {
   miniv3d right;
   if (dabs(dir.x)>dabs(dir.y) && dabs(dir.x)>dabs(dir.z)) right=miniv3d(0,0,dir.x);
   else if (dabs(dir.y)>dabs(dir.x) && dabs(dir.y)>dabs(dir.z)) right=miniv3d(0,0,dir.y);
   else right=miniv3d(dir.z,0,0);

   miniv3d up;
   up=right/dir;
   right=dir/up;
   right.normalize();

   return(right);
   }

mininode_geometry_sphere::mininode_geometry_sphere(double radius,int tessel)
   : mininode_geometry(0,3,2)
   {
   int i,j;

   double u,v;
   double alpha,beta;
   miniv3d pos;

   int alpha_steps=4*tessel;
   int beta_steps=tessel;

   for (j=-beta_steps; j<beta_steps; j++)
      for (i=0; i<=alpha_steps; i++)
         {
         u=(double)i/alpha_steps;
         v=(double)j/beta_steps;

         alpha=u*2*PI;
         beta=v*PI/2;

         pos=miniv3d(sin(alpha)*cos(beta),
                     cos(alpha)*cos(beta),
                     sin(beta))*radius;

         setnrm(pos);
         settex(u,0.5-v/2);
         addvtx(pos);

         v=(double)(j+1)/beta_steps;
         beta=v*PI/2;

         pos=miniv3d(sin(alpha)*cos(beta),
                     cos(alpha)*cos(beta),
                     sin(beta))*radius;

         setnrm(pos);
         settex(u,0.5-v/2);
         addvtx(pos);
         }
   }

mininode_geometry_band::mininode_geometry_band(const minidyna<miniv3d> &pos,const minidyna<miniv3d> &nrm,double width)
   : mininode_geometry(0,3,0)
   {
   if (pos.getsize()<2) return;
   if (pos.getsize()!=nrm.getsize()) return;

   for (unsigned int i=0; i<pos.getsize(); i++)
      {
      miniv3d dir;

      if (i==0) dir=pos[i+1]-pos[i];
      else if (i==pos.getsize()-1) dir=pos[i]-pos[i-1];
      else dir=get_halfdir(pos[i]-pos[i-1],pos[i+1]-pos[i]);
      dir.normalize();

      miniv3d right=dir/nrm[i];
      right.normalize();

      setnrm(nrm[i]);
      addvtx(pos[i]-right*width/2);
      addvtx(pos[i]+right*width/2);
      }
   }

mininode_geometry_band::mininode_geometry_band(const minidyna<miniv3d> &pos,const minidyna<miniv3d> &nrm,const minidyna<double> &width)
   : mininode_geometry(0,3,0)
   {
   if (pos.getsize()<2) return;
   if (pos.getsize()!=nrm.getsize()) return;
   if (pos.getsize()!=width.getsize()) return;

   for (unsigned int i=0; i<pos.getsize(); i++)
      {
      miniv3d dir;

      if (i==0) dir=pos[i+1]-pos[i];
      else if (i==pos.getsize()-1) dir=pos[i]-pos[i-1];
      else dir=get_halfdir(pos[i]-pos[i-1],pos[i+1]-pos[i]);
      dir.normalize();

      miniv3d right=dir/nrm[i];
      right.normalize();

      setnrm(nrm[i]);
      addvtx(pos[i]-right*width[i]/2);
      addvtx(pos[i]+right*width[i]/2);
      }
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

mininode_geometry_tube::mininode_geometry_tube(const miniv3d &pos1,const miniv3d &pos2,double radius,int tessel)
   : mininode_geometry(0,3,0)
   {
   miniv3d dir=pos2-pos1;
   miniv3d right=get_right(dir);

   create_tube(pos1,pos2,
               dir,dir,
               right,
               radius,
               TRUE,TRUE,
               tessel);
   }

mininode_geometry_tube::mininode_geometry_tube(const minidyna<miniv3d> &pos,double radius,
                                               BOOLINT start_cap,BOOLINT end_cap,
                                               int tessel)
   : mininode_geometry(0,3,0)
   {
   if (pos.getsize()<2) return;

   miniv3d dir=pos[1]-pos[0];
   miniv3d right=get_right(dir);

   for (unsigned int i=0; i<pos.getsize()-1; i++)
      right=create_tube(pos[i],pos[i+1],
                        (i==0)?pos[i+1]-pos[i]:get_halfdir(pos[i]-pos[i-1],pos[i+1]-pos[i]),
                        (i==pos.getsize()-2)?pos[i+1]-pos[i]:get_halfdir(pos[i+1]-pos[i],pos[i+2]-pos[i+1]),
                        right,
                        radius,
                        (i==0)?start_cap:FALSE,(i==pos.getsize()-2)?end_cap:FALSE,
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
      miniv3d vtx1=project(start+right,dir,start,start_dir);
      miniv3d vtx2=project(end+right,dir,end,end_dir);

      setnrm(vtx1-start);
      addvtx(vtx1);
      setnrm(vtx2-end);
      addvtx(vtx2);

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

mininode_geometry_torus::mininode_geometry_torus(const minidyna<miniv3d> &pos,double radius,
                                                 int tessel)
   : mininode_geometry_tube()
   {
   if (pos.getsize()<4) return;

   miniv3d dir=pos[1]-pos[0];
   miniv3d right=get_right(dir);

   for (unsigned int i=0; i<pos.getsize(); i++)
      {
      unsigned int il1,ir1,ir2;

      if (i>0) il1=i-1;
      else il1=pos.getsize()-1;

      if (i<pos.getsize()-1) ir1=i+1;
      else ir1=0;

      if (ir1<pos.getsize()-1) ir2=ir1+1;
      else ir2=0;

      right=create_tube(pos[i],pos[ir1],
                        get_halfdir(pos[i]-pos[il1],pos[ir1]-pos[i]),
                        get_halfdir(pos[ir1]-pos[i],pos[ir2]-pos[ir1]),
                        right,
                        radius,
                        FALSE,FALSE,
                        tessel);
      }
   }
