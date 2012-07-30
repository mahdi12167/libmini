// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MININODES_H
#define MININODES_H

#include <iostream>

#include "minibase.h"
#include "minimath.h"

#include "minicrs.h"
#include "minicoord.h"
#include "minicurve.h"
#include "mininode.h"

#include "ministring.h"
#include "miniimg.h"

#include "miniOGL.h"
#include "ministrip.h"
#include "minibrick.h"
#include "minicam.h"

#include "database.h"

enum
   {
   MININODE=0,
   MININODE_CAM,
   MININODE_COLOR,
   MININODE_TEXTURE,
   MININODE_SWITCH,
   MININODE_SELECTOR,
   MININODE_TRANSFORM,
   MININODE_GEOMETRY
   };

class mininode_cam;

//! group node
//!  groups arbitrary number of children links
//!  has merged bounding sphere of its children
//!  provides first enabled camera
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
   virtual ~mininode_group() {}

   //! get first enabled camera
   virtual mininode_cam *get_camera()
      {return((mininode_cam *)get_first(MININODE_CAM));}

   //! check bounding sphere
   virtual BOOLINT has_bsphere() const
      {return(bound_radius>0.0);}

   //! get bounding sphere
   virtual void get_bsphere(miniv3d &center,double &radius) const
      {
      center=bound_center;
      radius=bound_radius;
      }

   //! shoot a ray and return the distance to the closest object
   //! o is the origin of the ray, d is the ray direction
   //! a return value of MAXFLOAT indicates that there was no hit
   virtual double shoot_ray(const miniv3d &o,const miniv3d &d,double mindist=0.0) const;

   protected:

   virtual void traverse_init();
   virtual void traverse_pre() {}
   virtual void traverse_past() {}
   virtual void traverse_post() {}
   virtual void traverse_exit();

   virtual void update_dirty();

   miniv3d bound_center;
   double bound_radius;
   };

//! helper class that defines a view cone
class minicone
   {
   public:

   minicone()
      {valid=FALSE;}

   minicone(const miniv3d &p,const miniv3d &d,double c)
      {
      valid=TRUE;
      pos=p; dir=d; cone=c;
      }

   BOOLINT valid;

   miniv3d pos,dir;
   double cone;
   };

inline std::ostream& operator << (std::ostream &out,const minicone &c)
   {return(out << '(' << c.pos << ',' << c.dir << ',' << c.cone << ')');}

//! culling node (base class)
//!  culls bounding sphere with camera cone
//!  transforms camera cone with inverse of local transform
class mininode_culling: public mininode_group
   {
   public:

   //! default constructor
   mininode_culling(unsigned int id=0)
      : mininode_group(id)
      {is_visible=TRUE;}

   //! destructor
   virtual ~mininode_culling() {}

   //! get number of children
   virtual unsigned int get_children() const
      {return(is_visible?getsize():0);}

   //! shoot a ray and return the distance to the closest object
   double shoot_ray(const miniv3d &o,const miniv3d &d,double mindist=0.0) const;

   protected:

   static minidyna<minicone> cone_stack;

   BOOLINT is_visible;

   virtual void traverse_init();
   virtual void traverse_pre();
   virtual void traverse_post();
   virtual void traverse_exit();

   virtual void transform_cone(minicone &) const {}
   virtual void transform_point(miniv3d &) const {}
   };

//! dynamic time-dependent node (base class)
//!  has global time setter and getter
class mininode_dynamic: public mininode_culling
   {
   public:

   //! default constructor
   mininode_dynamic(unsigned int id=0)
      : mininode_culling(id)
      {}

   //! destructor
   virtual ~mininode_dynamic() {}

   //! set global time start
   static void set_time_start(double t)
      {m_time_start=t;}

   //! set global time
   static void set_time(double t)
      {m_time=t-m_time_start;}

   //! get global time
   static double get_time()
      {return(m_time);}

   protected:

   static double m_time_start;
   static double m_time;
   };

//! color node
//!  attributes children with color
class mininode_color: public mininode_group
   {
   public:

   //! custom constructor
   mininode_color(const miniv4d &c)
      : mininode_group(MININODE_COLOR)
      {rgba=c;}

   //! custom constructor
   mininode_color(const miniv3d &c)
      : mininode_group(MININODE_COLOR)
      {rgba=miniv4d(c,1);}

   //! destructor
   virtual ~mininode_color() {}

   //! set local brightness
   static void set_brightness(double b=1.0)
      {brightness=b;}

   //! get actual color
   static miniv4d get_color()
      {return(glcolor);}

   protected:

   miniv4d rgba;

   static double brightness;

   static minidyna<miniv4d> rgba_stack;
   static miniv4d glcolor;

   virtual void traverse_pre()
      {
      miniv4d c;

      // init color state
      if (rgba_stack.empty()) glcolor=miniv4d(1,1,1);

      // compute actual node color
      c=rgba*brightness;

      // push actual color
      rgba_stack.push(c);

      // lazy color state change
      if (c!=glcolor)
         {
         glcolor=c;
         color(c);
         }
      }

   virtual void traverse_past()
      {
      miniv4d c;

      // peek actual color
      c=rgba_stack.peek();

      // lazy color state change
      if (c!=glcolor)
         {
         glcolor=c;
         color(c);
         }
      }

   virtual void traverse_post()
      {rgba_stack.pop();}

   };

//! texture node (base class)
//!  has texture object id
//!  auto-deletes texture object
class mininode_texture: public mininode_group
   {
   public:

   //! default constructor
   mininode_texture(BOOLINT clamp=FALSE)
      : mininode_group(MININODE_TEXTURE)
      {
      is_on=FALSE;
      is_managed=TRUE;

      texid=0;
      texclamp=clamp;
      }

   //! destructor
   virtual ~mininode_texture()
      {if (is_managed && texid!=0) deletetexmap(texid);}

   //! clear texture
   void clear()
      {
      is_on=FALSE;
      if (is_managed && texid!=0) deletetexmap(texid);
      is_managed=TRUE;

      texid=0;
      texclamp=FALSE;
      }

   //! get active texture status
   BOOLINT is_active() const
      {return(is_on && texid!=0);}

   //! set active texture status
   void set_active(BOOLINT on)
      {is_on=on;}

   //! get texture clamping
   BOOLINT get_clamp() const
      {return(texclamp);}

   protected:

   BOOLINT is_on;
   BOOLINT is_managed;

   unsigned int texid;
   BOOLINT texclamp;

   static unsigned int texture_level;

   virtual void traverse_pre()
      {if (is_active()) texture_level++;}

   virtual void traverse_post()
      {if (is_active()) texture_level--;}

   };

//! texture2D node
//!  2D texture object
//!  has texture matrix for fitting normalized 2D texture coordinates
class mininode_texture2D: public mininode_texture
   {
   public:

   //! default constructor
   mininode_texture2D(BOOLINT clamp=FALSE)
      : mininode_texture(clamp)
      {}

   //! copy constructor
   mininode_texture2D(const mininode_texture2D &t)
      : mininode_texture(t.texclamp)
      {set(t.texid,t.width,t.height,t.mipmaps);}

   //! texture setter (id)
   void set(int texid,int width,int height,int mipmaps=1)
      {
      clear();
      is_managed=FALSE;

      this->texid=texid;

      this->width=width;
      this->height=height;

      this->mipmaps=mipmaps;
      }

   //! texture loader (raw data)
   void load(unsigned char *image,int width,int height,int components,
             int bits=24,int mipmaps=1,int s3tc=0,int bytes=0,int mipmapped=0)
      {
      clear();

      texid=buildtexmap(image,&width,&height,components,
                        bits,mipmaps,s3tc,bytes,mipmapped);

      this->width=width;
      this->height=height;

      this->mipmaps=mipmaps;
      }

   //! texture loader (db format)
   void load(const databuf *buf)
      {
      clear();
      texid=db2texid(buf,&width,&height,&mipmaps);
      }

   //! get actual texture id
   static unsigned int get_texid()
      {return(tid);}

   protected:

   int width,height;
   int mipmaps;

   static minidyna<unsigned int> texid_stack;
   static unsigned int tid;

   virtual void traverse_pre()
      {
      unsigned int t=texid;

      // init texture state
      if (texid_stack.empty())
         {
         tid=0;
         set_active(texture_level==0);
         }
      else set_active(TRUE);

      // check activity
      if (!is_active()) return;

      // push actual texture
      texid_stack.push(t);

      // push texture stack
      mtxtex();
      mtxpush();
      mtxtranslate(0.5/width,0.5/height,0.0);
      mtxscale((double)(width-1)/width,(double)(height-1)/height,0.0);
      mtxmodel();

      // lazy texture state change
      if (t!=tid)
         {
         tid=t;
         if (texclamp) bindtexmap(t,1,1,1,mipmaps);
         else bindtexmap(t,0,0,0,mipmaps);
         ministrip::setglobal_tex(TRUE);
         }

      mininode_texture::traverse_pre();
      }

   virtual void traverse_past()
      {
      unsigned int t;

      // check activity
      if (!is_active()) return;

      // peek actual texture
      t=texid_stack.peek();

      // lazy texture state change
      if (t!=tid)
         {
         tid=t;
         if (texclamp) bindtexmap(t,1,1,1,mipmaps);
         else bindtexmap(t,0,0,0,mipmaps);
         ministrip::setglobal_tex(TRUE);
         }

      mininode_texture::traverse_past();
      }

   virtual void traverse_post()
      {
      // check activity
      if (!is_active()) return;

      // pop actual texture
      texid_stack.pop();

      // pop texture stack
      mtxtex();
      mtxpop();
      mtxmodel();

      // lazy texture state change
      if (tid!=0)
         {
         tid=0;
         bindtexmap(0,0,0,0,0);
         ministrip::setglobal_tex(FALSE);
         }

      mininode_texture::traverse_post();
      }

   };

//! texture3D node
//!  3D texture object
//!  has texture matrix for fitting normalized 3D texture coordinates
class mininode_texture3D: public mininode_texture
   {
   public:

   //! default constructor
   mininode_texture3D()
      : mininode_texture()
      {}

   //! copy constructor
   mininode_texture3D(const mininode_texture3D &t)
      : mininode_texture()
      {set(t.texid,t.width,t.height,t.depth);}

   //! texture setter (id)
   void set(int texid,int width,int height,int depth)
      {
      clear();
      is_managed=FALSE;

      this->texid=texid;

      this->width=width;
      this->height=height;
      this->depth=depth;
      }

   //! texture loader
   void load(unsigned char *volume,unsigned int width,unsigned int height,unsigned int depth,unsigned int components)
      {
      clear();

      texid=build3Dtexmap(volume,&width,&height,&depth,components);

      this->width=width;
      this->height=height;
      this->depth=depth;
      }

   //! texture loader (db format)
   void load(const databuf *buf)
      {
      clear();
      texid=db2texid3D(buf,&width,&height,&depth);
      }

   //! get actual texture id
   static unsigned int get_texid()
      {return(tid);}

   protected:

   unsigned int width,height,depth;

   static minidyna<unsigned int> texid_stack;
   static unsigned int tid;

   virtual void traverse_pre()
      {
      unsigned int t=texid;

      // init texture state
      if (texid_stack.empty())
         {
         tid=0;
         set_active(texture_level==0);
         }
      else set_active(TRUE);

      // check activity
      if (!is_active()) return;

      // push actual texture
      texid_stack.push(t);

      // push texture stack
      mtxtex();
      mtxpush();
      mtxtranslate(0.5/width,0.5/height,0.5/depth);
      mtxscale((double)(width-1)/width,(double)(height-1)/height,(double)(depth-1)/depth);
      mtxmodel();

      // lazy texture state change
      if (t!=tid)
         {
         tid=t;
         bind3Dtexmap(t);
         ministrip::setglobal_tex3(TRUE);
         }

      mininode_texture::traverse_pre();
      }

   virtual void traverse_past()
      {
      unsigned int t;

      // check activity
      if (!is_active()) return;

      // peek actual texture
      t=texid_stack.peek();

      // lazy texture state change
      if (t!=tid)
         {
         tid=t;
         bind3Dtexmap(t);
         ministrip::setglobal_tex3(TRUE);
         }

      mininode_texture::traverse_past();
      }

   virtual void traverse_post()
      {
      // check activity
      if (!is_active()) return;

      // pop actual texture
      texid_stack.pop();

      // pop texture stack
      mtxtex();
      mtxpop();
      mtxmodel();

      // lazy texture state change
      if (tid!=0)
         {
         tid=0;
         bind3Dtexmap(0);
         ministrip::setglobal_tex3(FALSE);
         }

      mininode_texture::traverse_post();
      }

   };

//! image node
//!  has 2D texture object
class mininode_image: public mininode_texture2D
   {
   public:

   //! custom constructor
   mininode_image(ministring filename,BOOLINT clamp=FALSE)
      : mininode_texture2D(clamp)
      {
      databuf buf;
      if (miniimg::loadimg(buf,filename.c_str()))
         {
         if (buf.xsize>=2 && buf.ysize>=2)
            if (buf.type==databuf::DATABUF_TYPE_BYTE)
               load((unsigned char *)buf.data,buf.xsize,buf.ysize,1,8);
            else if (buf.type==databuf::DATABUF_TYPE_SHORT)
               {
               buf.convertdata(databuf::DATABUF_TYPE_BYTE);
               load((unsigned char *)buf.data,buf.xsize,buf.ysize,1,8);
               }
            else if (buf.type==databuf::DATABUF_TYPE_RGB)
               load((unsigned char *)buf.data,buf.xsize,buf.ysize,3,8);
            else if (buf.type==databuf::DATABUF_TYPE_RGBA)
               load((unsigned char *)buf.data,buf.xsize,buf.ysize,4,8);
            else if (buf.type==databuf::DATABUF_TYPE_RGB_S3TC)
               load((unsigned char *)buf.data,buf.xsize,buf.ysize,3,8,0,1,buf.bytes);
            else if (buf.type==databuf::DATABUF_TYPE_RGBA_S3TC)
               load((unsigned char *)buf.data,buf.xsize,buf.ysize,4,8,0,1,buf.bytes);
            else if (buf.type==databuf::DATABUF_TYPE_RGB_MM_S3TC)
               load((unsigned char *)buf.data,buf.xsize,buf.ysize,3,8,1,1,buf.bytes,1);
            else if (buf.type==databuf::DATABUF_TYPE_RGBA_MM_S3TC)
               load((unsigned char *)buf.data,buf.xsize,buf.ysize,4,8,1,1,buf.bytes,1);

         buf.release();
         }
      }

   };

//! volume node
//!  has 3D texture object
class mininode_volume: public mininode_texture3D
   {
   public:

   //! custom constructor
   mininode_volume(ministring filename)
      : mininode_texture3D()
      {
      databuf buf;
      if (miniimg::loadimg(buf,filename.c_str()))
         {
         if (buf.xsize>=2 && buf.ysize>=2 && buf.zsize>=2)
            if (buf.type==databuf::DATABUF_TYPE_BYTE)
               load((unsigned char *)buf.data,buf.xsize,buf.ysize,buf.zsize,1);
            else if (buf.type==databuf::DATABUF_TYPE_RGB)
               load((unsigned char *)buf.data,buf.xsize,buf.ysize,buf.zsize,3);
            else if (buf.type==databuf::DATABUF_TYPE_RGBA)
               load((unsigned char *)buf.data,buf.xsize,buf.ysize,buf.zsize,4);

         buf.release();
         }
      }

   };

//! switch node
//!  turns render traversal on or off
class mininode_switch: public mininode_group
   {
   public:

   //! default constructor
   mininode_switch()
      : mininode_group(MININODE_SWITCH)
      {is_on=TRUE;}

   //! destructor
   virtual ~mininode_switch() {}

   virtual unsigned int get_children() const
      {return(is_on?getsize():0);}

   virtual mininode *get_child(unsigned int i=0) const
      {return(is_on?get(i):NULL);}

   void toggle(BOOLINT on=TRUE)
      {
      if (is_on!=on) set_dirty();
      is_on=on;
      }

   BOOLINT is_toggled() const
      {return(is_on);}

   protected:

   BOOLINT is_on;
   };

//! selector node
//!  turns render traversal on for one indexed child
class mininode_selector: public mininode_group
   {
   public:

   //! default constructor
   mininode_selector()
      : mininode_group(MININODE_SELECTOR)
      {index=0;}

   //! destructor
   virtual ~mininode_selector() {}

   virtual unsigned int get_children() const
      {return(1);}

   virtual mininode *get_child(unsigned int=0) const
      {return(get(index));}

   void select(unsigned int i=0)
      {
      if (i!=index) set_dirty();
      index=i;
      }

   unsigned int get_selection() const
      {return(index);}

   protected:

   unsigned int index;
   };

//! transformation node (base class)
//!  provides linear local modelling transfrom
//!  pushes and pops transform before and after render traversal
//!  transforms bounding sphere with local transform
//!  transforms camera cone with inverse local transform
class mininode_transform: public mininode_dynamic
   {
   public:

   //! default constructor
   mininode_transform(const miniv4d mtx[3]=NULL)
      : mininode_dynamic(MININODE_TRANSFORM)
      {
      if (mtx!=NULL) mtxget(mtx,oglmtx);
      else
         {
         miniv3d mtx[3];
         set_mtx(mtx);
         mtxget(mtx,oglmtx);
         }
      }

   //! destructor
   virtual ~mininode_transform() {}

   //! get bounding sphere
   virtual void get_bsphere(miniv3d &center,double &radius) const
      {
      center=bound_center;
      radius=bound_radius;
      }

   protected:

   double oglmtx[16];

   virtual void traverse_init();

   virtual void traverse_pre()
      {
      mininode_culling::traverse_pre();
      mtxpush(); mtxmult(oglmtx);
      }

   virtual void traverse_post()
      {
      mtxpop();
      mininode_culling::traverse_post();
      }

   virtual void traverse_exit();

   virtual void transform_cone(minicone &cone) const
      {
      if (cone.cone==0.0)
         {
         miniv3d mtx[3],vec;
         mtxget(oglmtx,mtx);
         mtxget(oglmtx,vec);

         miniv3d inv[3];
         inv_mtx(inv,mtx);
         miniv3d tra[3];
         tra_mtx(tra,inv);

         cone.pos=mlt_vec(inv,cone.pos-vec);
         cone.dir=mlt_vec(tra,cone.dir);
         }
      else
         {
         miniv3d mtx[3];
         mtxget(oglmtx,mtx);

         if (!chk_mtx(mtx)) cone.valid=FALSE;
         }
      }

   virtual void transform_point(miniv3d &point) const
      {
      miniv3d mtx[3],vec;
      mtxget(oglmtx,mtx);
      mtxget(oglmtx,vec);

      point=mlt_vec(mtx,point)+vec;
      }

   virtual void update_dirty();
   };

//! translation node
//!  provides translation transform
class mininode_translate: public mininode_transform
   {
   public:

   //! custom constructor
   mininode_translate(const miniv3d &v)
      : mininode_transform()
      {
      miniv4d mtx[3]={miniv4d(1,0,0,v.x),miniv4d(0,1,0,v.y),miniv4d(0,0,1,v.z)};
      mtxget(mtx,oglmtx);
      }

   //! get bounding sphere
   virtual void get_bsphere(miniv3d &center,double &radius) const
      {
      miniv3d vec;
      mtxget(oglmtx,vec);

      center=bound_center+vec;
      radius=bound_radius;
      }

   protected:

   virtual void transform_cone(minicone &cone) const
      {
      miniv3d vec;
      mtxget(oglmtx,vec);

      cone.pos-=vec;
      }

   };

//! rotation node
//!  provides rotation transform
class mininode_rotate: public mininode_transform
   {
   public:

   //! custom constructor
   mininode_rotate(double d,const miniv3d &a)
      : mininode_transform()
      {
      miniv3d rot[3];
      rot_mtx(rot,d,a);
      mtxget(rot,oglmtx);
      }

   //! get bounding sphere
   virtual void get_bsphere(miniv3d &center,double &radius) const
      {
      miniv3d mtx[3];
      mtxget(oglmtx,mtx);

      center=mlt_vec(mtx,bound_center);
      radius=bound_radius;
      }

   protected:

   virtual void transform_cone(minicone &cone) const
      {
      miniv3d mtx[3];
      mtxget(oglmtx,mtx);

      miniv3d tra[3];
      tra_mtx(tra,mtx);

      cone.pos=mlt_vec(tra,cone.pos);
      cone.dir=mlt_vec(tra,cone.dir);
      }

   };

//! affine transform node
//!  provides affine transform
class mininode_affine: public mininode_transform
   {
   public:

   //! default constructor
   mininode_affine()
      : mininode_transform()
      {}

   //! copy constructor
   mininode_affine(const mininode_translate &translate)
      : mininode_transform(translate)
      {}

   //! copy constructor
   mininode_affine(const mininode_rotate &rotate)
      : mininode_transform(rotate)
      {}

   //! custom constructor
   mininode_affine(double d,const miniv3d &a,const miniv3d &v)
      : mininode_transform()
      {
      miniv3d rot[3];
      miniv4d mtx[3];
      rot_mtx(rot,d,a);
      cpy_mtx(mtx,rot,v);
      mtxget(rot,oglmtx);
      }

   //! get bounding sphere
   virtual void get_bsphere(miniv3d &center,double &radius) const
      {
      miniv4d mtx[3];
      mtxget(oglmtx,mtx);

      center=mlt_vec(mtx,bound_center);
      radius=bound_radius;
      }

   protected:

   virtual void transform_cone(minicone &cone) const
      {
      miniv3d mtx[3],vec;
      mtxget(oglmtx,mtx);
      mtxget(oglmtx,vec);

      miniv3d tra[3];
      tra_mtx(tra,mtx);

      cone.pos=mlt_vec(tra,cone.pos-vec);
      cone.dir=mlt_vec(tra,cone.dir);
      }

   };

//! scale node
//!  provides scale transform
class mininode_scale: public mininode_transform
   {
   public:

   //! custom constructor
   mininode_scale(double s)
      : mininode_transform()
      {
      miniv3d mtx[3]={miniv3d(s,0,0),miniv3d(0,s,0),miniv3d(0,0,s)};
      mtxget(mtx,oglmtx);
      }

   //! custom constructor
   mininode_scale(double sx,double sy,double sz)
      : mininode_transform()
      {
      miniv3d mtx[3]={miniv3d(sx,0,0),miniv3d(0,sy,0),miniv3d(0,0,sz)};
      mtxget(mtx,oglmtx);
      }

   //! get bounding sphere
   virtual void get_bsphere(miniv3d &center,double &radius) const
      {
      miniv4d mtx[3];
      mtxget(oglmtx,mtx);

      double max_scale=dabs(mtx[0].x);
      if (dabs(mtx[1].y)>max_scale) max_scale=dabs(mtx[1].y);
      if (dabs(mtx[2].z)>max_scale) max_scale=dabs(mtx[2].z);

      center=miniv3d(bound_center.x*mtx[0].x,bound_center.y*mtx[1].y,bound_center.z*mtx[2].z);
      radius=bound_radius*max_scale;
      }

   protected:

   virtual void transform_cone(minicone &cone) const
      {
      miniv3d mtx[3];
      mtxget(oglmtx,mtx);

      double s1=mtx[0].x;
      double s2=mtx[1].y;
      double s3=mtx[2].z;

      if (s1!=0.0 && s2!=0.0 && s3!=0.0)
         if (s1==s2 && s2==s3)
            {
            cone.pos/=s1;
            cone.dir/=s1;
            }
         else if (cone.cone==0.0)
            {
            cone.pos.x/=s1;
            cone.pos.y/=s2;
            cone.pos.z/=s3;

            cone.dir.x/=s1;
            cone.dir.y/=s2;
            cone.dir.z/=s3;
            }
         else cone.valid=FALSE;
      else cone.valid=FALSE;
      }

   };

//! coordinate node
//!  provides affine transform into a local geo-referenced coordinate system
class mininode_coord: public mininode_affine
   {
   public:

   //! custom constructor
   mininode_coord(const minicoord &c);

   //! set coord position
   void set_coord(const minicoord &c);

   //! set global light direction
   static void set_lightdir(const miniv3d &d);

   protected:

   miniv3d up;

   static miniv3d lightdir;
   static BOOLINT lightdirset;

   virtual void traverse_pre();
   virtual void traverse_post();
   };

//! animated coordinate node
//!  provides animated affine transform into a local geo-referenced coordinate system
class mininode_coord_animation: public mininode_coord
   {
   public:

   //! custom constructor
   mininode_coord_animation(minicurve &c);

   protected:

   minicurve curve;

   virtual void traverse_post()
      {
      mininode_coord::traverse_post();
      set_dirty();
      }

   virtual void update_dirty();
   };

//! animation node
//!  provides dynamic time-dependent transform (base class)
class mininode_animation: public mininode_transform
   {
   public:

   //! default constructor
   mininode_animation()
      : mininode_transform()
      {}

   protected:

   virtual void traverse_post()
      {
      mininode_transform::traverse_post();
      set_dirty();
      }

   };

//! rotate animation node
//!  provides time-dependent rotation
class mininode_animation_rotate: public mininode_animation
   {
   public:

   //! custom constructor
   mininode_animation_rotate(double w,const miniv3d &a)
      : mininode_animation()
      {m_omega=w; m_axis=a;}

   //! get bounding sphere
   virtual void get_bsphere(miniv3d &center,double &radius) const
      {
      miniv3d mtx[3];
      mtxget(oglmtx,mtx);

      center=mlt_vec(mtx,bound_center);
      radius=bound_radius;
      }

   protected:

   double m_omega;
   miniv3d m_axis;

   virtual void transform_cone(minicone &cone) const
      {
      miniv3d mtx[3];
      mtxget(oglmtx,mtx);

      miniv3d tra[3];
      tra_mtx(tra,mtx);

      cone.pos=mlt_vec(tra,cone.pos);
      cone.dir=mlt_vec(tra,cone.dir);
      }

   virtual void update_dirty();
   };

//! texgen node
//!  enables object space texture coordinate generation
//!  provides texture coordinate transform
class mininode_texgen: public mininode_transform
   {
   public:

   //! default constructor
   mininode_texgen()
      : mininode_transform()
      {}

   //! get actual texgen level
   static int get_level()
      {return(texgen_level);}

   protected:

   static unsigned int texgen_level;

   virtual void traverse_pre()
      {
      if (texgen_level++==0) ministrip::setglobal_texgen(TRUE);

      mtxtex();
      mtxpush();
      mtxmult(oglmtx);
      mtxmodel();
      }

   virtual void traverse_post()
      {
      if (--texgen_level==0) ministrip::setglobal_texgen(FALSE);

      mtxtex();
      mtxpop();
      mtxmodel();
      }

   virtual void transform_cone(minicone &) const {}
   virtual void transform_point(miniv3d &) const {}
   };

//! translate texgen node
//!  provides texture coordinate translation
class mininode_texgen_translate: public mininode_texgen
   {
   public:

   //! custom constructor
   mininode_texgen_translate(const miniv3d &v)
      : mininode_texgen()
      {
      miniv4d mtx[3]={miniv4d(1,0,0,v.x),miniv4d(0,1,0,v.y),miniv4d(0,0,1,v.z)};
      mtxget(mtx,oglmtx);
      }
   };

//! rotate texgen node
//!  provides texture coordinate rotation
class mininode_texgen_rotate: public mininode_texgen
   {
   public:

   //! custom constructor
   mininode_texgen_rotate(double d,const miniv3d &a)
      : mininode_texgen()
      {
      miniv3d rot[3];
      rot_mtx(rot,d,a);
      mtxget(rot,oglmtx);
      }

   };

//! scale texgen node
//!  provides texture coordinate scaling
class mininode_texgen_scale: public mininode_texgen
   {
   public:

   //! custom constructor
   mininode_texgen_scale(double s)
      : mininode_texgen()
      {
      miniv3d mtx[3]={miniv3d(s,0,0),miniv3d(0,s,0),miniv3d(0,0,s)};
      mtxget(mtx,oglmtx);
      }

   //! custom constructor
   mininode_texgen_scale(double sx,double sy,double sz)
      : mininode_texgen()
      {
      miniv3d mtx[3]={miniv3d(sx,0,0),miniv3d(0,sy,0),miniv3d(0,0,sz)};
      mtxget(mtx,oglmtx);
      }

   };

//! geometry node (base class)
//!  provides triangle-stripped geometry
//!  has optional per-vertex color, normals and texture coordinates
class mininode_geometry_base: public mininode_group, public ministrip
   {
   public:

   //! default constructor
   mininode_geometry_base(int colcomps=0,int nrmcomps=0,int texcomps=0,
                          int wocol=1,int wonrm=0,int wotex=0)
      : mininode_group(MININODE_GEOMETRY), ministrip(colcomps,nrmcomps,texcomps)
      {
      this->wocol=wocol;
      this->wonrm=wonrm;
      this->wotex=wotex;
      }

   //! copy constructor
   mininode_geometry_base(const ministrip &strip,
                          int wocol=1,int wonrm=0,int wotex=0)
      : mininode_group(MININODE_GEOMETRY), ministrip(strip)
      {
      this->wocol=wocol;
      this->wonrm=wonrm;
      this->wotex=wotex;
      }

   //! copy constructor
   mininode_geometry_base(const mininode_geometry_base &geo)
      : mininode_group(MININODE_GEOMETRY), ministrip(geo)
      {
      this->wocol=geo.wocol;
      this->wonrm=geo.wonrm;
      this->wotex=geo.wotex;
      }

   //! destructor
   virtual ~mininode_geometry_base() {}

   //! add geo-referenced point
   void add_coord(minicoord c)
      {
      if (c.type!=minicoord::MINICOORD_LINEAR) c.convert2(minicoord::MINICOORD_ECEF);
      addvtx(c.vec);
      }

   //! check color array
   BOOLINT has_color() const {return(hascolor() && wocol==0);}

   //! check normal array
   BOOLINT has_normal() const {return(hasnormal() && wonrm==0);}

   //! check tex coord array
   BOOLINT has_tex() const {return(hastex() && wotex==0);}

   //! render geometry
   void render()
      {ministrip::render(wocol,wonrm,wotex);}

   //! get bounding sphere
   virtual void get_bsphere(miniv3d &center,double &radius) const
      {
      double radius2;
      getbsphere(center,radius2);
      radius=sqrt(radius2);
      }

   protected:

   int wocol,wonrm,wotex;

   virtual void traverse_pre()
      {
      BOOLINT texgen=ministrip::getglobal_texgen(); // get texgen state
      if (has_tex() && texgen) ministrip::setglobal_texgen(FALSE); // override texgen with tex coords
      ministrip::render(wocol,wonrm,wotex); // render triangle strip
#ifdef MININODES_RENDERBBOX
      renderbbox();
#endif
      if (hastex() && texgen) ministrip::setglobal_texgen(TRUE); // restore texgen state
      }

   static miniv3d project(const miniv3d &pos,const miniv3d &dir,
                          const miniv3d &org,const miniv3d &nrm);

   static miniv3d get_halfdir(const miniv3d &dir1,const miniv3d &dir2);
   static miniv3d get_right(const miniv3d &dir);
   };

//! deferred geometry node
//!  provides deferred geometry as list
class mininode_geometry: public mininode_geometry_base
   {
   public:

   struct geometry_deferred_struct
      {
      mininode_geometry_base *node;
      unsigned int pass_first,pass_last;
      double mv_matrix[16];
      double t_matrix[16];
      unsigned int texid2D,texid3D;
      miniv4d color;
      };

   typedef struct geometry_deferred_struct geometry_deferred_type;
   typedef minidyna<geometry_deferred_type> geometry_deferred_list;

   //! default constructor
   mininode_geometry(int colcomps=0,int nrmcomps=0,int texcomps=0,
                     int wocol=1,int wonrm=0,int wotex=0)
      : mininode_geometry_base(colcomps,nrmcomps,texcomps,
                               wocol,wonrm,wotex)
      {shown=TRUE;}

   //! destructor
   virtual ~mininode_geometry() {}

   //! show geometry or not
   void show(BOOLINT yes=TRUE)
      {shown=yes;}

   BOOLINT is_shown() const
      {return(shown);}

   static void enable_deferred(BOOLINT on)
      {deferred=on;}

   static void enable_deferred_semitransparen(BOOLINT on)
      {deferred_semitransparent=on;}

   static void set_pass_frame(unsigned int first,unsigned int last)
      {
      pass_first=first;
      pass_last=last;
      }

   static void clear_deferred()
      {
      list.clear();
      list_tex2D.clear();
      list_tex3D.clear();
      }

   static const geometry_deferred_list *get_deferred()
      {return(&list);}

   static const geometry_deferred_list *get_deferred_tex2D()
      {return(&list_tex2D);}

   static const geometry_deferred_list *get_deferred_tex3D()
      {return(&list_tex3D);}

   protected:

   BOOLINT shown;

   static BOOLINT deferred;
   static BOOLINT deferred_semitransparent;
   static unsigned int pass_first,pass_last;

   static geometry_deferred_list list;
   static geometry_deferred_list list_tex2D,list_tex3D;

   virtual void traverse_pre()
      {
      if (!shown) return;

      BOOLINT dfrd=FALSE;

      if (deferred) dfrd=TRUE;

      if (deferred_semitransparent)
         if (!isopaque()) dfrd=TRUE;
         else if (!hascolor() && mininode_color::get_color().w<1.0) dfrd=TRUE;

      if (dfrd)
         if (ministrip::getglobal_texgen())
            if (mininode_texture2D::get_texid()!=0 ||
                mininode_texture3D::get_texid()!=0)
               if (!has_tex()) dfrd=FALSE;

      if (!dfrd) mininode_geometry_base::traverse_pre();
      else
         {
         geometry_deferred_type geo;

         geo.node=this;
         geo.pass_first=pass_first;
         geo.pass_last=pass_last;
         mtxgetmodel(geo.mv_matrix);
         mtxgettex(geo.t_matrix);
         geo.texid2D=mininode_texture2D::get_texid();
         geo.texid3D=mininode_texture3D::get_texid();
         geo.color=mininode_color::get_color();

         if (geo.texid2D!=0) list_tex2D.append(geo);
         else if (geo.texid3D!=0) list_tex3D.append(geo);
         else list.append(geo);
         }
      }

   };

//! tetrahedron geometry node
//!  provides triangle-stripped tetrahedron
//!  has normals
class mininode_geometry_tet: public mininode_geometry
   {
   public:

   //! default constructors
   mininode_geometry_tet() : mininode_geometry(0,3,0) {}
   mininode_geometry_tet(double size);
   };

//! pyramid geometry node
//!  provides triangle-stripped pyramid
//!  has normals
class mininode_geometry_pyramid: public mininode_geometry
   {
   public:

   //! default constructors
   mininode_geometry_pyramid() : mininode_geometry(0,3,0) {}
   mininode_geometry_pyramid(double sizex,double sizey,double sizez);
   };

//! cube geometry node
//!  provides triangle-stripped cube
//!  has normals
class mininode_geometry_cube: public mininode_geometry
   {
   public:

   //! default constructors
   mininode_geometry_cube() : mininode_geometry(0,3,0) {}
   mininode_geometry_cube(double sizex,double sizey,double sizez);
   };

//! prism geometry node
//!  provides triangle-stripped prism
//!  has normals
class mininode_geometry_prism: public mininode_geometry
   {
   public:

   //! default constructors
   mininode_geometry_prism() : mininode_geometry(0,3,0) {}
   mininode_geometry_prism(double sizex,double sizey,double sizez);
   };

//! sphere geometry node
//!  provides triangle-stripped sphere
//!  has normals
//!  has texture coordinates from polar coordinates
class mininode_geometry_sphere: public mininode_geometry
   {
   public:

   //! default constructors
   mininode_geometry_sphere() : mininode_geometry(0,3,2) {}
   mininode_geometry_sphere(double radius,int wotex=0,int tessel=16);
   };

//! band geometry node
//!  provides triangle-stripped band
//!  has normals
class mininode_geometry_band: public mininode_geometry
   {
   public:

   //! default constructors
   mininode_geometry_band() : mininode_geometry(0,3,0) {}
   mininode_geometry_band(const minidyna<miniv3d> &pos,const minidyna<miniv3d> &nrm,double width);
   mininode_geometry_band(const minidyna<miniv3d> &pos,const minidyna<miniv3d> &nrm,const minidyna<double> &width);

   //! constructors from curve
   mininode_geometry_band(const minicurve &curve,const minidyna<miniv3d> &nrm,double width);
   mininode_geometry_band(const minicurve &curve,const minidyna<miniv3d> &nrm,const minidyna<double> &width);
   };

//! tube geometry node
//!  provides triangle-stripped tube
//!  has normals
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

   //! constructors from curve
   mininode_geometry_tube(const minicurve &curve,double radius,
                          BOOLINT start_cap=TRUE,BOOLINT end_cap=TRUE,
                          int tessel=16);
   mininode_geometry_tube(const minicurve &curve,const minidyna<double> &radius,
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
//!  provides triangle-stripped torus
//!  has normals
class mininode_geometry_torus: public mininode_geometry_tube
   {
   public:

   //! default constructor
   mininode_geometry_torus(const minidyna<miniv3d> &pos,double radius,
                           int tessel=16);

   };

//! geometry evaluator node
//!  provides implicit triangle-stripped triangle mesh
//!  has normals
//!  has texture coordinates from evaluation domain
class mininode_geometry_evaluator: public mininode_geometry
   {
   public:

   //! default constructor
   mininode_geometry_evaluator();

   //! construct mesh by evaluating each mesh vertex
   void construct(int tesselx=64,int tessely=64);

   protected:

   virtual miniv3d evaluate(double x,double y) = 0;

   private:

   void construct_vtx(double x,double y,double s);
   };

//! deferred transform node (base class)
//!  enables deferred rendering after graph traversal
class mininode_deferred: public mininode_transform
   {
   public:

   //! custom constructor
   mininode_deferred(unsigned int first=0,unsigned int last=0)
      : mininode_transform()
      {
      pass_first=first;
      pass_last=last;
      }

   //! destructor
   virtual ~mininode_deferred() {}

   protected:

   unsigned int pass_first,pass_last;

   virtual void traverse_init()
      {
      mininode_transform::traverse_init();

      mininode_geometry::clear_deferred();

      deferred_first=1;
      deferred_last=0;
      }

   virtual void traverse_pre()
      {
      mininode_geometry::set_pass_frame(pass_first,pass_last);

      if (deferred_first>deferred_last)
         {
         deferred_first=pass_first;
         deferred_last=pass_last;
         }
      else
         {
         if (pass_first<deferred_first) deferred_first=pass_first;
         if (pass_last>deferred_last) deferred_last=pass_last;
         }

      mininode_transform::traverse_pre();
      }

   virtual void traverse_past()
      {
      mininode_geometry::set_pass_frame(pass_first,pass_last);

      mininode_transform::traverse_past();
      }

   virtual void traverse_exit()
      {
      const mininode_geometry::geometry_deferred_list *list=mininode_geometry::get_deferred();
      unsigned int s=list->getsize();

      const mininode_geometry::geometry_deferred_list *list_tex2D=mininode_geometry::get_deferred_tex2D();
      unsigned int s2=list_tex2D->getsize();

      const mininode_geometry::geometry_deferred_list *list_tex3D=mininode_geometry::get_deferred_tex3D();
      unsigned int s3=list_tex3D->getsize();

      mininode_transform::traverse_exit();

      if (s>0)
         {
         mtxpush();
         mtxtex();
         mtxpush();
         mtxmodel();

         deferred_init();

         for (unsigned int pass=deferred_first; pass<=deferred_last; pass++)
            {
            int dorender=deferred_pre(pass);

            if (dorender)
               {
               // plain geometry
               for (unsigned int i=0; i<s; i++)
                  {
                  const mininode_geometry::geometry_deferred_type *geo=&(list->get(i));

                  if (pass>=geo->pass_first && pass<=geo->pass_last)
                     {
                     mtxid();
                     mtxmult(geo->mv_matrix);
                     color(geo->color);
                     geo->node->render();
                     }
                  }

               // 2D textured geometry
               for (unsigned int i=0; i<s2; i++)
                  {
                  const mininode_geometry::geometry_deferred_type *geo=&(list_tex2D->get(i));

                  if (pass>=geo->pass_first && pass<=geo->pass_last)
                     {
                     mtxid();
                     mtxmult(geo->mv_matrix);
                     mtxtex();
                     mtxid();
                     mtxmult(geo->t_matrix);
                     mtxmodel();
                     color(geo->color);
                     bindtexmap(geo->texid2D,1,1,1);
                     geo->node->render();
                     }
                  }

               bindtexmap(0);

               // 3D textured geometry
               for (unsigned int i=0; i<s3; i++)
                  {
                  const mininode_geometry::geometry_deferred_type *geo=&(list_tex3D->get(i));

                  if (pass>=geo->pass_first && pass<=geo->pass_last)
                     {
                     mtxid();
                     mtxmult(geo->mv_matrix);
                     mtxtex();
                     mtxid();
                     mtxmult(geo->t_matrix);
                     mtxmodel();
                     color(geo->color);
                     bind3Dtexmap(geo->texid3D);
                     geo->node->render();
                     }
                  }

               bind3Dtexmap(0);
               }

            deferred_post(pass);
            }

         deferred_exit();

         mininode_geometry::clear_deferred();

         mtxpop();
         mtxtex();
         mtxpop();
         mtxmodel();
         }
      }

   virtual void deferred_init() = 0;
   virtual int deferred_pre(unsigned int pass) = 0;
   virtual void deferred_post(unsigned int pass) = 0;
   virtual void deferred_exit() = 0;

   static unsigned int deferred_first;
   static unsigned int deferred_last;
   };

//! deferred semi-transparent node
//!  enables deferred rendering of semi-transparent geometry
class mininode_deferred_semitransparent: public mininode_deferred
   {
   public:

   //! default constructor
   mininode_deferred_semitransparent()
      : mininode_deferred(minisurf::FIRST_RENDER_PHASE,minisurf::LAST_RENDER_PHASE)
      {deferred_passes=4;}

   //! destructor
   virtual ~mininode_deferred_semitransparent() {}

   //! set number of rendering passes for semi-transparent geometry
   //! 2 = over-operator: fast but with artifacts for non-uniform colors
   //! 3 = additive blending: compromise between performance and depth perception
   //! 4 = z-limited additive blending: slow but with good depth perception
   void set_passes(unsigned int passes)
      {deferred_passes=passes;}

   protected:

   unsigned int deferred_passes;

   virtual void traverse_pre()
      {
      if (deferred_level++==0) mininode_geometry::enable_deferred_semitransparen(TRUE);

      mininode_deferred::traverse_pre();
      }

   virtual void traverse_post()
      {
      if (--deferred_level==0) mininode_geometry::enable_deferred_semitransparen(FALSE);

      mininode_deferred::traverse_post();
      }

   virtual void deferred_init() {}
   virtual int deferred_pre(unsigned int pass);
   virtual void deferred_post(unsigned int pass);
   virtual void deferred_exit() {}

   static unsigned int deferred_level;
   };

typedef mininode_deferred_semitransparent mininode_root;
typedef miniref<mininode_root> mininode_rootref;

//! camera node
//!  provides camera lookat, direction, fovy and cone
//!  has bounding sphere of entire earth
class mininode_cam: public mininode_root, public minicam
   {
   public:

   //! custom constructor
   mininode_cam(miniearth *earth,
                double lat=21.39,double lon=-157.72,double height=7E6,
                double mindist=0.0,
                float fovy=0.0f,float aspect=0.0f,
                double nearp=0.0,double farp=0.0)
      : mininode_root(),
        minicam(earth,lat,lon,height,mindist,
                fovy,aspect,nearp,farp)
      {set_id(MININODE_CAM);}

   //! destructor
   virtual ~mininode_cam() {}

   //! get this camera
   virtual mininode_cam *get_camera()
      {return(this);}

   //! get bounding sphere
   virtual void get_bsphere(miniv3d &center,double &radius) const
      {
      center=miniv3d(0,0,0);
      radius=minicrs::EARTH_radius;
      }

   };

#endif
