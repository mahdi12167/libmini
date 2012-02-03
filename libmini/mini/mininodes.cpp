// (c) by Stefan Roettger

#include "mininodes.h"

// mininode_group:

// shoot a ray and return the distance to the closest object
double mininode_group::shoot_ray(const miniv3d &o,const miniv3d &d,double hitdist) const
   {
   double mindist,dist;

   unsigned int s=get_children();

   mindist=MAXFLOAT;

   if (has_bsphere())
      if (itest_ray_sphere(o,d,bound_center,bound_radius*bound_radius))
         for (unsigned int i=0; i<s; i++)
            {
            mininode *child=get_child(i);
            if (child)
               {
               // get child group
               mininode_group *child_group=dynamic_cast<mininode_group *>(child);

               if (child_group)
                  {
                  // get child geometry
                  mininode_geometry *child_geo=dynamic_cast<mininode_geometry *>(child);

                  if (child_geo) dist=child_geo->shoot(o,d,hitdist);
                  else dist=child_group->shoot_ray(o,d,hitdist);

                  if (dist<mindist) mindist=dist;
                  }
               }
            }

   return(mindist);
   }

void mininode_group::traverse_init()
   {
   // state initialization
   color(1,1,1);
   enableAtest(0.0f);

   // texture state initialization
   bindtexmap(0);
   bind3Dtexmap(0);
   ministrip::setglobal_texgen(FALSE);
   }

void mininode_group::traverse_exit()
   {
   // state restoration
   disableAtest();
   }

void mininode_group::update_dirty()
   {
   miniv3d center;
   double radius;

   unsigned int s=get_children();

   // clear bounding sphere
   bound_center=miniv3d(0,0,0);
   bound_radius=0.0;

   // get bounding sphere
   get_bsphere(center,radius);

   // merge bounding sphere with children
   for (unsigned int i=0; i<s; i++)
      {
      mininode *child=get_child(i);
      if (child)
         {
         // get child group
         mininode_group *child_group=dynamic_cast<mininode_group *>(child);

         if (child_group)
            {
            miniv3d child_center;
            double child_radius;

            child_group->get_bsphere(child_center,child_radius);

            // merge with child bounding sphere
            if (child_radius>0.0)
               if (radius>0.0)
                  merge_spheres(center,radius,
                                child_center,child_radius);
               else
                  {
                  center=child_center;
                  radius=child_radius;
                  }
            }
         }
      }

   bound_center=center;
   bound_radius=radius;
   }

// mininode_culling:

minidyna<minicone> mininode_culling::cone_stack;

// shoot a ray and return the distance to the closest object
double mininode_culling::shoot_ray(const miniv3d &o,const miniv3d &d,double hitdist) const
   {
   minicone cone;

   double dist;
   miniv3d hit;

   // get shooting cone
   cone=minicone(o,d,0.0);

   // get transformed shooting cone
   transform_cone(cone);

   // shoot with transformed cone
   if (cone.valid)
      {
      dist=mininode_group::shoot_ray(cone.pos,cone.dir,hitdist);
      if (dist!=MAXFLOAT)
         {
         hit=cone.pos+dist*cone.dir;
         transform_point(hit);

         return(d*(hit-o)/d.getlength2());
         }
      }

   return(MAXFLOAT);
   }

void mininode_culling::traverse_init()
   {
   // state initialization
   mininode_group::traverse_init();

   mininode_cam *camera=get_camera();

   miniv3d eye=camera->get_eye().vec;
   miniv3d dir=camera->get_dir();
   double cone=camera->get_cone();

   cone_stack.push(minicone(eye,dir,cone));
   }

void mininode_culling::traverse_pre()
   {
   minicone cone;

   is_visible=TRUE;

   // intersect camera cone with bounding sphere
   if (has_bsphere())
      {
      // get camera cone
      cone=cone_stack.peek();

      // get transformed camera cone
      if (cone.valid) transform_cone(cone);

      // remember transformed camera cone
      cone_stack.push(cone);

      // intersect with original bounding sphere
      if (cone.valid)
         is_visible=itest_cone_sphere(cone.pos,cone.dir,cone.cone,
                                      bound_center,bound_radius);
      }
   }

void mininode_culling::traverse_post()
   {if (has_bsphere()) cone_stack.pop();}

void mininode_culling::traverse_exit()
   {
   cone_stack.pop();

   // state restoration
   mininode_group::traverse_exit();
   }

// mininode_dynamic:

double mininode_dynamic::m_time=0.0;

// mininode_color:

double mininode_color::brightness=1.0;

minidyna<miniv4d> mininode_color::rgba_stack;
miniv4d mininode_color::glcolor(1,1,1);

// mininode_texture:

unsigned int mininode_texture::texture_level=0;

// mininode_texture2D:

minidyna<unsigned int> mininode_texture2D::texid_stack;
unsigned int mininode_texture2D::tid=0;

// mininode_texture3D:

minidyna<unsigned int> mininode_texture3D::texid_stack;
unsigned int mininode_texture3D::tid=0;

// mininode_transform:

void mininode_transform::traverse_init()
   {
   // state initialization
   mininode_culling::traverse_init();

   mtxtex();
   mtxpush();
   mtxid();
   mtxmodel();
   }

void mininode_transform::traverse_exit()
   {
   mtxtex();
   mtxpop();
   mtxmodel();

   // state restoration
   mininode_culling::traverse_exit();
   }

void mininode_transform::update_dirty()
   {
   // merge two consecutive transform nodes
   if (get_links()==1)
      {
      mininode *child=get_child();
      if (child)
         {
         // get child transformation
         mininode_transform *child_transform=dynamic_cast<mininode_transform *>(child);

         if (child_transform)
            {
            // check child transform types
            BOOLINT child_translate=dynamic_cast<mininode_translate *>(child)!=NULL;
            BOOLINT child_rotate=dynamic_cast<mininode_rotate *>(child)!=NULL;
            BOOLINT child_affine=dynamic_cast<mininode_affine *>(child)!=NULL;
            BOOLINT child_scale=dynamic_cast<mininode_scale *>(child)!=NULL;
            BOOLINT child_coord=dynamic_cast<mininode_coord *>(child)!=NULL;

            // check node transform types
            BOOLINT node_translate=dynamic_cast<mininode_translate *>(this)!=NULL;
            BOOLINT node_rotate=dynamic_cast<mininode_rotate *>(this)!=NULL;
            BOOLINT node_affine=dynamic_cast<mininode_affine *>(this)!=NULL;
            BOOLINT node_scale=dynamic_cast<mininode_scale *>(this)!=NULL;
            BOOLINT node_coord=dynamic_cast<mininode_coord *>(this)!=NULL;

            // check child/node match
            if ((child_translate && node_translate) ||
                (child_rotate && node_rotate) ||
                (child_affine && node_affine) ||
                (child_scale && node_scale) ||
                (child_affine && node_translate) ||
                (child_translate && node_affine) ||
                (child_affine && node_rotate) ||
                (child_rotate && node_affine))
               {
               if (!child_coord || !node_coord)
                  {
                  // multiply with child's transformation matrix
                  miniv4d mtx[3],mtx1[3],mtx2[3];
                  mtxget(oglmtx,mtx1);
                  mtxget(child_transform->oglmtx,mtx2);
                  mlt_mtx(mtx,mtx1,mtx2);
                  mtxget(mtx,oglmtx);
                  }

               // remove child
               remove_child();

               // propagate node to affine transform
               if (child_affine)
                  if (node_translate)
                     *this=mininode_affine(*dynamic_cast<mininode_translate *>(this));
                  else if (node_rotate)
                     *this=mininode_affine(*dynamic_cast<mininode_rotate *>(this));
               }
            }
         }
      }

   mininode_group::update_dirty();
   }

// mininode_coord:

miniv3d mininode_coord::lightdir=miniv3d(0,0,0);
BOOLINT mininode_coord::lightdirset=FALSE;

mininode_coord::mininode_coord(const minicoord &c)
   : mininode_affine()
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

   // cull on backside of earth
   minicone cone=cone_stack.peek();
   if (has_bsphere())
      if (cone.valid)
         {
         double t=intersect_ray_ellipsoid(cone.pos,bound_center-cone.pos,
                                          miniv3d(0.0,0.0,-minicrs::EARTH_radius),
                                          minicrs::EARTH_radius-bound_radius,
                                          minicrs::EARTH_radius-bound_radius,
                                          minicrs::EARTH_radius-bound_radius);

         if (t>0.0 && t<1.0) is_visible=FALSE;
         }
   }

void mininode_coord::traverse_post()
   {
   if (lightdirset)
      mininode_color::set_brightness(1.0);

   mininode_transform::traverse_post();
   }

// mininode_texgen:

unsigned int mininode_texgen::texgen_level=0;

// mininode_geometry:

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

mininode_geometry_tet::mininode_geometry_tet(double size)
   : mininode_geometry(0,3,0)
   {
   static const double sqrt13=sqrt(1.0/3);

   miniv3d p1(-size/2,-size*sqrt13/2,0);
   miniv3d p2(size/2,-size*sqrt13/2,0);
   miniv3d p3(0,size*sqrt13,0);
   miniv3d p4(0,0,1.5*size*sqrt13);

   // bottom
   setnrm(miniv3d(0,0,-1));
   addvtx(p1);
   addvtx(p3);
   addvtx(p2);

   // sides
   setnrm(p1,p2,p4);
   addvtx(p2);
   addvtx(p1);
   addvtx(p4);
   setnrm(p2,p3,p4);
   addvtx(p4);
   addvtx(p2);
   addvtx(p3);
   setnrm(p3,p1,p4);
   addvtx(p3);
   addvtx(p4);
   addvtx(p1);
   }

mininode_geometry_pyramid::mininode_geometry_pyramid(double sizex,double sizey,double sizez)
   : mininode_geometry(0,3,0)
   {
   miniv3d p1(-sizex/2,-sizey/2,0);
   miniv3d p2(sizex/2,-sizey/2,0);
   miniv3d p3(-sizex/2,sizey/2,0);
   miniv3d p4(sizex/2,sizey/2,0);
   miniv3d p5(0,0,sizez);

   // sides
   setnrm(p1,p2,p5);
   addvtx(p5);
   addvtx(p1);
   addvtx(p2);
   setnrm(p2,p4,p5);
   addvtx(p2);
   addvtx(p5);
   addvtx(p4);
   setnrm(p4,p3,p5);
   addvtx(p4);
   addvtx(p3);
   addvtx(p5);
   setnrm(p3,p1,p5);
   addvtx(p5);
   addvtx(p1);
   addvtx(p3);

   // bottom
   setnrm(miniv3d(0,0,-1));
   addvtx(p3);
   addvtx(p4);
   addvtx(p1);
   addvtx(p2);
   }

mininode_geometry_cube::mininode_geometry_cube(double sizex,double sizey,double sizez)
   : mininode_geometry(0,3,0)
   {
   sizex/=2.0;
   sizey/=2.0;
   sizez/=2.0;

   // sides
   setnrm(miniv3d(0,-1,0));
   addvtx(miniv3d(-sizex,-sizey,sizez));
   addvtx(miniv3d(-sizex,-sizey,-sizez));
   addvtx(miniv3d(sizex,-sizey,sizez));
   addvtx(miniv3d(sizex,-sizey,-sizez));
   setnrm(miniv3d(1,0,0));
   addvtx(miniv3d(sizex,-sizey,sizez));
   addvtx(miniv3d(sizex,-sizey,-sizez));
   addvtx(miniv3d(sizex,sizey,sizez));
   addvtx(miniv3d(sizex,sizey,-sizez));
   setnrm(miniv3d(0,1,0));
   addvtx(miniv3d(sizex,sizey,sizez));
   addvtx(miniv3d(sizex,sizey,-sizez));
   addvtx(miniv3d(-sizex,sizey,sizez));
   addvtx(miniv3d(-sizex,sizey,-sizez));
   setnrm(miniv3d(-1,0,0));
   addvtx(miniv3d(-sizex,sizey,sizez));
   addvtx(miniv3d(-sizex,sizey,-sizez));
   addvtx(miniv3d(-sizex,-sizey,sizez));
   addvtx(miniv3d(-sizex,-sizey,-sizez));

   // bottom
   beginstrip();
   setnrm(miniv3d(0,0,-1));
   addvtx(miniv3d(-sizex,-sizey,-sizez));
   addvtx(miniv3d(-sizex,sizey,-sizez));
   addvtx(miniv3d(sizex,-sizey,-sizez));
   addvtx(miniv3d(sizex,sizey,-sizez));

   // top
   beginstrip();
   setnrm(miniv3d(0,0,1));
   addvtx(miniv3d(-sizex,-sizey,sizez));
   addvtx(miniv3d(sizex,-sizey,sizez));
   addvtx(miniv3d(-sizex,sizey,sizez));
   addvtx(miniv3d(sizex,sizey,sizez));
   }

mininode_geometry_prism::mininode_geometry_prism(double sizex,double sizey,double sizez)
   : mininode_geometry(0,3,0)
   {
   sizex/=2.0;
   sizey/=2.0;

   // bottom
   setnrm(miniv3d(0,0,-1));
   addvtx(miniv3d(-sizex,sizey,0));
   addvtx(miniv3d(sizex,sizey,0));
   addvtx(miniv3d(-sizex,-sizey,0));
   addvtx(miniv3d(sizex,-sizey,0));

   // top
   setnrm(miniv3d(0,-sizez,sizey));
   addvtx(miniv3d(-sizex,-sizey,0));
   addvtx(miniv3d(sizex,-sizey,0));
   addvtx(miniv3d(-sizex,0,sizez));
   addvtx(miniv3d(sizex,0,sizez));
   setnrm(miniv3d(0,sizez,sizey));
   addvtx(miniv3d(-sizex,0,sizez));
   addvtx(miniv3d(sizex,0,sizez));
   addvtx(miniv3d(-sizex,sizey,0));
   addvtx(miniv3d(sizex,sizey,0));

   // sides
   beginstrip();
   setnrm(miniv3d(-1,0,0));
   addvtx(miniv3d(-sizex,sizey,0));
   addvtx(miniv3d(-sizex,-sizey,0));
   addvtx(miniv3d(-sizex,0,sizez));
   beginstrip();
   setnrm(miniv3d(1,0,0));
   addvtx(miniv3d(sizex,sizey,0));
   addvtx(miniv3d(sizex,-sizey,0));
   addvtx(miniv3d(sizex,0,sizez));
   }

mininode_geometry_sphere::mininode_geometry_sphere(double radius,int wotex,int tessel)
   : mininode_geometry(0,3,2,1,0,wotex)
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
               radius,radius,
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
               radius,radius,
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
                        radius,radius,
                        (i==0)?start_cap:FALSE,(i==pos.getsize()-2)?end_cap:FALSE,
                        tessel);
   }

mininode_geometry_tube::mininode_geometry_tube(const minidyna<miniv3d> &pos,const minidyna<double> &radius,
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
                        radius[i],radius[i+1],
                        (i==0)?start_cap:FALSE,(i==pos.getsize()-2)?end_cap:FALSE,
                        tessel);
   }

miniv3d mininode_geometry_tube::create_tube(const miniv3d &start,const miniv3d &end,
                                            const miniv3d &start_dir,const miniv3d &end_dir,
                                            const miniv3d &start_right,
                                            double start_radius,double end_radius,
                                            BOOLINT start_cap,BOOLINT end_cap,
                                            int tessel)
   {
   miniv3d dir=end-start;
   dir.normalize();

   miniv3d right,up;
   up=start_right/dir;
   right=dir/up;
   right.normalize();

   miniv3d rot[3];
   rot_mtx(rot,-360.0/tessel,dir);

   beginstrip();

   for (int i=0; i<=tessel; i++)
      {
      miniv3d vtx1=project(start+right*start_radius,dir,start,start_dir);
      miniv3d vtx2=project(end+right*end_radius,dir,end,end_dir);

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
         addvtx(project(start+right*start_radius,dir,start,start_dir));

         if (i<tessel) right=mlt_vec(rot,right);
         }
      }

   if (end_cap)
      {
      beginstrip();
      setnrm(end_dir);

      for (int i=0; i<=tessel; i++)
         {
         addvtx(project(end+right*end_radius,dir,end,end_dir));
         addvtx(end);

         if (i<tessel) right=mlt_vec(rot,right);
         }
      }

   return(project(end+right*end_radius,dir,end,end_dir)-end);
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
