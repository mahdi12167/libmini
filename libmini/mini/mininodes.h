// (c) by Stefan Roettger

#ifndef MININODES_H
#define MININODES_H

#include <iostream>

#include "minibase.h"
#include "minimath.h"

#include "minimath.h"
#include "minicoord.h"
#include "mininode.h"

#include "miniOGL.h"
#include "minicam.h"

//! camera node
class mininode_cam: public mininode, public minicam
   {
   public:

   static const unsigned int ID=1;

   //! default constructor
   mininode_cam(miniearth *earth,
                double lat=21.39,double lon=-157.72,double height=7E6,
                double mindist=0.0)
      : mininode(ID), minicam(earth,lat,lon,height,mindist)
      {}

   //! destructor
   virtual ~mininode_cam()
      {}
   };

//! color node
class mininode_color: public mininode
   {
   public:

   static const unsigned int ID=2;

   //! default constructor
   mininode_color(const miniv4d &c)
      : mininode(ID)
      {rgba=c;}

   mininode_color(const miniv3d &c)
      : mininode(ID)
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

//! transformation node
class mininode_transform: public mininode
   {
   public:

   static const unsigned int ID=3;

   //! default constructor
   mininode_transform(const miniv4d mtx[3]=NULL)
      : mininode(ID)
      {if (mtx!=NULL) mtxget(mtx,oglmtx);}

   //! destructor
   virtual ~mininode_transform()
      {}

   protected:

   double oglmtx[16];

   virtual void traverse_pre()
      {mtxpush(); mtxmult(oglmtx);}

   virtual void traverse_post()
      {mtxpop();}
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

//! geometry node
class mininode_geometry: public mininode, public ministrip
   {
   public:

   static const unsigned int ID=4;

   //! default constructor
   mininode_geometry(int colcomps=0,int nrmcomps=0,int texcomps=0)
      : mininode(ID), ministrip(colcomps,nrmcomps,texcomps)
      {}

   //! destructor
   virtual ~mininode_geometry()
      {}

   void addcoord(minicoord c)
      {
      if (c.type!=minicoord::MINICOORD_LINEAR) c.convert2(minicoord::MINICOORD_ECEF);
      addvtx(c.vec);
      }

   protected:

   virtual void traverse_pre()
      {render(1);}
   };

//! tube geometry node
class mininode_geometry_tube: public mininode_geometry
   {
   public:

   //! default constructors
   mininode_geometry_tube(double radius,double height,int tessel=16);
   mininode_geometry_tube(const miniv3d &p1,const miniv3d &p2,double radius,int tessel=16);

   //! multi-tube constructor
   mininode_geometry_tube(const minidyna<miniv3d> &p,double radius,
                          BOOLINT start_cap=TRUE,BOOLINT end_cap=TRUE,
                          int tessel=16);

   protected:

   miniv3d create_tube(const miniv3d &start,const miniv3d &end,
                       const miniv3d &start_dir,const miniv3d &end_dir,
                       const miniv3d &start_right,
                       double radius,
                       BOOLINT start_cap=TRUE,BOOLINT end_cap=TRUE,
                       int tessel=16);

   miniv3d project(const miniv3d &p,const miniv3d &d,
                   const miniv3d &o,const miniv3d &n) const;
   };

#endif
