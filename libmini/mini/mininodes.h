// (c) by Stefan Roettger

#ifndef MININODES_H
#define MININODES_H

#include <iostream>

#include "minibase.h"

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

//! transformation node
class mininode_transform: public mininode
   {
   public:

   static const unsigned int ID=2;

   //! default constructor
   mininode_transform(const miniv4d mtx[3])
      : mininode(ID)
      {mtxget(mtx,oglmtx);}

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

//! geometry node
class mininode_geometry: public mininode, public ministrip
   {
   public:

   static const unsigned int ID=3;

   //! default constructor
   mininode_geometry(int colcomps=0,int nrmcomps=0,int texcomps=0)
      : mininode(ID), ministrip(colcomps,nrmcomps,texcomps)
      {}

   //! destructor
   virtual ~mininode_geometry()
      {}

   protected:

   virtual void traverse_pre()
      {render();}
   };

//! color node
class mininode_color: public mininode
   {
   public:

   static const unsigned int ID=4;

   //! default constructor
   mininode_color(const miniv4d &c)
      : mininode(ID)
      {rgba=c;}

   //! destructor
   virtual ~mininode_color()
      {}

   protected:

   miniv4d rgba;

   virtual void traverse_pre()
      {color(rgba);}

   virtual void traverse_past()
      {color(rgba);}
   };

#endif
