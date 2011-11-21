// (c) by Stefan Roettger

#ifndef MININODES_H
#define MININODES_H

#include <iostream>

#include "minibase.h"
#include "minimath.h"

#include "minicoord.h"
#include "mininode.h"

#include "miniOGL.h"
#include "minicam.h"

enum
   {
   MININODE=0,
   MININODE_CAM,
   MININODE_COLOR,
   MININODE_SWITCH,
   MININODE_TRANSFORM,
   MININODE_ANIMATION,
   MININODE_GEOMETRY
   };

//! dynamic time-dependent node
class mininode_group: public mininode
   {
   public:

   //! default constructor
   mininode_group(unsigned int id=0)
      : mininode(id)
      {
      bound_center=miniv3d(0,0,0);
      bound_radius=0.0;
      }

   //! destructor
   virtual ~mininode_group()
      {}

   //! get bounding sphere
   virtual void get_bsphere(miniv3d &center,double &radius)
      {
      center=bound_center;
      radius=bound_radius;
      }

   protected:

   virtual void traverse_pre() {}
   virtual void traverse_past() {}
   virtual void traverse_post() {}

   virtual void update();

   miniv3d bound_center;
   double bound_radius;
   };

//! dynamic time-dependent node
class mininode_dynamic: public mininode_group
   {
   public:

   //! default constructor
   mininode_dynamic(unsigned int id=0)
      : mininode_group(id)
      {}

   //! destructor
   virtual ~mininode_dynamic()
      {}

   //! set global time
   static void set_time(double t)
      {m_time=t;}

   //! get global time
   static double get_time()
      {return(m_time);}

   protected:

   static double m_time;
   };

//! camera node
class mininode_cam: public mininode_dynamic, public minicam
   {
   public:

   //! default constructor
   mininode_cam(miniearth *earth,
                double lat=21.39,double lon=-157.72,double height=7E6,
                double mindist=0.0)
      : mininode_dynamic(MININODE_CAM), minicam(earth,lat,lon,height,mindist)
      {}

   //! destructor
   virtual ~mininode_cam()
      {}
   };

//! color node
class mininode_color: public mininode_group
   {
   public:

   //! default constructor
   mininode_color(const miniv4d &c)
      : mininode_group(MININODE_COLOR)
      {rgba=c;}

   mininode_color(const miniv3d &c)
      : mininode_group(MININODE_COLOR)
      {rgba=miniv4d(c,1);}

   //! destructor
   virtual ~mininode_color()
      {}

   static void set_brightness(double b=1.0)
      {brightness=b;}

   protected:

   miniv4d rgba;

   static double brightness;

   virtual void traverse_pre()
      {color(rgba*brightness);}

   virtual void traverse_past()
      {color(rgba*brightness);}
   };

//! switch node
class mininode_switch: public mininode_group
   {
   public:

   //! default constructor
   mininode_switch()
      : mininode_group(MININODE_SWITCH)
      {ison=TRUE;}

   //! destructor
   virtual ~mininode_switch()
      {}

   virtual mininode *get_child(unsigned int i=0) const
      {return(ison?get(i):NULL);}

   void toggle(BOOLINT on=TRUE)
      {
      if (ison!=on) set_dirty();
      ison=on;
      }

   protected:

   BOOLINT ison;
   };

//! transformation node
class mininode_transform: public mininode_group
   {
   public:

   //! default constructor
   mininode_transform(const miniv4d mtx[3]=NULL)
      : mininode_group(MININODE_TRANSFORM)
      {if (mtx!=NULL) mtxget(mtx,oglmtx);}

   //! destructor
   virtual ~mininode_transform()
      {}

   //! get bounding sphere
   virtual void get_bsphere(miniv3d &center,double &radius)
      {
      miniv4d mtx[3];
      mtxget(oglmtx,mtx);

      double max_scale=dabs(mtx[0].x);
      if (dabs(mtx[1].y)>max_scale) max_scale=dabs(mtx[1].y);
      if (dabs(mtx[2].z)>max_scale) max_scale=dabs(mtx[2].z);

      center=mlt_vec(mtx,bound_center);
      radius=bound_radius*max_scale;
      }

   protected:

   double oglmtx[16];

   virtual void traverse_pre()
      {mtxpush(); mtxmult(oglmtx);}

   virtual void traverse_post()
      {mtxpop();}

   virtual void update();
   };

//! translation node
class mininode_translate: public mininode_transform
   {
   public:

   //! default constructor
   mininode_translate(const miniv3d &v)
      : mininode_transform()
      {
      miniv4d mtx[3]={miniv4d(1,0,0,v.x),miniv4d(0,1,0,v.y),miniv4d(0,0,1,v.z)};
      mtxget(mtx,oglmtx);
      }
   };

//! rotation node
class mininode_rotate: public mininode_transform
   {
   public:

   //! default constructor
   mininode_rotate(double d,const miniv3d &a)
      : mininode_transform()
      {
      miniv3d rot[3];
      rot_mtx(rot,d,a);
      mtxget(rot,oglmtx);
      }
   };

//! scale node
class mininode_scale: public mininode_transform
   {
   public:

   //! default constructor
   mininode_scale(double s)
      : mininode_transform()
      {
      miniv3d mtx[3]={miniv3d(s,0,0),miniv3d(0,s,0),miniv3d(0,0,s)};
      mtxget(mtx,oglmtx);
      }

   //! constructor
   mininode_scale(double sx,double sy,double sz)
      : mininode_transform()
      {
      miniv3d mtx[3]={miniv3d(sx,0,0),miniv3d(0,sy,0),miniv3d(0,0,sz)};
      mtxget(mtx,oglmtx);
      }
   };

//! coordinate node
class mininode_coord: public mininode_transform
   {
   public:

   //! default constructor
   mininode_coord(const minicoord &c);

   static void set_lightdir(const miniv3d &d);

   protected:

   miniv3d up;

   static miniv3d lightdir;
   static BOOLINT lightdirset;

   virtual void traverse_pre();
   virtual void traverse_post();
   };

//! animation node
class mininode_animation: public mininode_dynamic
   {
   public:

   //! default constructor
   mininode_animation()
      : mininode_dynamic(MININODE_ANIMATION)
      {
      miniv3d mtx[3];
      set_mtx(mtx);
      mtxget(mtx,oglmtx);
      }

   //! destructor
   virtual ~mininode_animation()
      {}

   //! get bounding sphere
   virtual void get_bsphere(miniv3d &center,double &radius)
      {
      miniv4d mtx[3];
      mtxget(oglmtx,mtx);

      double max_scale=mtx[0].x;
      if (mtx[1].y>max_scale) max_scale=mtx[1].y;
      if (mtx[2].z>max_scale) max_scale=mtx[2].z;

      center=mlt_vec(mtx,bound_center);
      radius=bound_radius*max_scale;
      }

   protected:

   double oglmtx[16];

   virtual void traverse_pre()
      {mtxpush(); mtxmult(oglmtx);}

   virtual void traverse_post()
      {mtxpop(); set_dirty();}
   };

//! rotate animation node
class mininode_animation_rotate: public mininode_animation
   {
   public:

   //! default constructor
   mininode_animation_rotate(double w,const miniv3d &a)
      : mininode_animation()
      {m_omega=w; m_axis=a;}

   protected:

   double m_omega;
   miniv3d m_axis;

   virtual void update()
      {
      miniv3d rot[3];
      rot_mtx(rot,get_time()*m_omega,m_axis);
      mtxget(rot,oglmtx);
      }
   };

//! geometry node
class mininode_geometry: public mininode_group, public ministrip
   {
   public:

   //! default constructor
   mininode_geometry(int colcomps=0,int nrmcomps=0,int texcomps=0)
      : mininode_group(MININODE_GEOMETRY), ministrip(colcomps,nrmcomps,texcomps)
      {}

   //! destructor
   virtual ~mininode_geometry()
      {}

   //! add geo-referenced point
   void addcoord(minicoord c)
      {
      if (c.type!=minicoord::MINICOORD_LINEAR) c.convert2(minicoord::MINICOORD_ECEF);
      addvtx(c.vec);
      }

   //! get bounding sphere
   virtual void get_bsphere(miniv3d &center,double &radius)
      {
      double radius2;
      getbsphere(center,radius2);
      radius=sqrt(radius2);
      }

   protected:

   virtual void traverse_pre()
      {render(1);}

   static miniv3d project(const miniv3d &pos,const miniv3d &dir,
                          const miniv3d &org,const miniv3d &nrm);

   static miniv3d get_halfdir(const miniv3d &dir1,const miniv3d &dir2);
   static miniv3d get_right(const miniv3d &dir);
   };

//! tetrahedron geometry node
class mininode_geometry_tet: public mininode_geometry
   {
   public:

   //! default constructors
   mininode_geometry_tet() : mininode_geometry(0,3,0) {}
   mininode_geometry_tet(double size);
   };

//! pyramid geometry node
class mininode_geometry_pyramid: public mininode_geometry
   {
   public:

   //! default constructors
   mininode_geometry_pyramid() : mininode_geometry(0,3,0) {}
   mininode_geometry_pyramid(double sizex,double sizey,double sizez);
   };

//! cube geometry node
class mininode_geometry_cube: public mininode_geometry
   {
   public:

   //! default constructors
   mininode_geometry_cube() : mininode_geometry(0,3,0) {}
   mininode_geometry_cube(double sizex,double sizey,double sizez);
   };

//! prism geometry node
class mininode_geometry_prism: public mininode_geometry
   {
   public:

   //! default constructors
   mininode_geometry_prism() : mininode_geometry(0,3,0) {}
   mininode_geometry_prism(double sizex,double sizey,double sizez);
   };

//! sphere geometry node
class mininode_geometry_sphere: public mininode_geometry
   {
   public:

   //! default constructors
   mininode_geometry_sphere() : mininode_geometry(0,3,2) {}
   mininode_geometry_sphere(double radius,int tessel=16);
   };

//! band geometry node
class mininode_geometry_band: public mininode_geometry
   {
   public:

   //! default constructors
   mininode_geometry_band() : mininode_geometry(0,3,0) {}
   mininode_geometry_band(const minidyna<miniv3d> &pos,const minidyna<miniv3d> &nrm,double width);
   mininode_geometry_band(const minidyna<miniv3d> &pos,const minidyna<miniv3d> &nrm,const minidyna<double> &width);
   };

//! tube geometry node
class mininode_geometry_tube: public mininode_geometry
   {
   public:

   //! default constructors
   mininode_geometry_tube() : mininode_geometry(0,3,0) {}
   mininode_geometry_tube(double radius,double height,int tessel=16);
   mininode_geometry_tube(const miniv3d &pos1,const miniv3d &pos2,double radius,int tessel=16);

   //! multi-tube constructors
   mininode_geometry_tube(const minidyna<miniv3d> &pos,double radius,
                          BOOLINT start_cap=TRUE,BOOLINT end_cap=TRUE,
                          int tessel=16);
   mininode_geometry_tube(const minidyna<miniv3d> &pos,const minidyna<double> &radius,
                          BOOLINT start_cap=TRUE,BOOLINT end_cap=TRUE,
                          int tessel=16);

   protected:

   miniv3d create_tube(const miniv3d &start,const miniv3d &end,
                       const miniv3d &start_dir,const miniv3d &end_dir,
                       const miniv3d &start_right,
                       double start_radius,double end_radius,
                       BOOLINT start_cap=TRUE,BOOLINT end_cap=TRUE,
                       int tessel=16);
   };

//! torus geometry node
class mininode_geometry_torus: public mininode_geometry_tube
   {
   public:

   //! default constructor
   mininode_geometry_torus(const minidyna<miniv3d> &pos,double radius,
                           int tessel=16);
   };

#endif
