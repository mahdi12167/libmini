// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minimath.h"

#include "mininode_path.h"

// default constructor
mininode_geometry_path::mininode_geometry_path()
   : mininode_geometry(0,3,0)
   {}

// constructor
mininode_geometry_path::mininode_geometry_path(ministring filename,double width)
   : mininode_geometry(0,3,0)
   {
   path_.load(filename);
   recreate(width);
   }

// destructor
mininode_geometry_path::~mininode_geometry_path()
   {}

// recreate geometry with a particular band width
void mininode_geometry_path::recreate(double width)
   {*(mininode_geometry *)this=mininode_geometry_band_path(path_,width);}

// default constructor
mininode_geometry_path_clod::mininode_geometry_path_clod()
   : mininode_geometry(0,3,0)
   {}

// destructor
mininode_geometry_path_clod::~mininode_geometry_path_clod()
   {}

// load path
void mininode_geometry_path_clod::load(ministring filename)
   {
   path_.load(filename);
   calcD2();
   }

// recreate geometry from actual view point
void mininode_geometry_path_clod::recreate(minicoord eye,
                                           double maxdiff,double atdist)
   {
   eye.convert2ecef();
   EYE_=eye.getpos();

   C_=maxdiff/atdist;

   calcpath();
   }

// calculate the d2-values
void mininode_geometry_path_clod::calcD2()
   {
   d2_.setsize(path_.getsize(),MAXFLOAT);
   md_.setsize(path_.getsize(),0.0f);

   if (!path_.empty())
      calcD2(0,path_.getsize()-1);
   }

// propagate the d2-values top-down
float mininode_geometry_path_clod::calcD2(int left,int right)
   {
   int i;

   double d2=0.0;
   double md=0.0;

   if (right-left>1)
      {
      int center=(left+right)/2;

      float d2l=calcD2(left,center);
      float d2r=calcD2(center,right);

      miniv3d a=path_.get(left).getpos();
      miniv3d b=path_.get(right).getpos();
      miniv3d c=path_.get(center).getpos();

      double d=(b-a).getlength();

      d2=distance2line(c,a,b);

      if (d>0.0) d2/=d;
      else d2=MAXFLOAT;

      d2=fmax(d2,0.5f*d2l);
      d2=fmax(d2,0.5f*d2r);

      d2_[center]=d2;

      for (i=left+1; i<right-1; i++)
         {
         miniv3d c=path_.get(i).getpos();
         double d=distance2line(c,a,b);

         if (d>md) md=d;
         }

      md_[i]=md;
      }

   return(d2);
   }

// calculate the path
void mininode_geometry_path_clod::calcpath()
   {
   if (!path_.empty())
      calcpath(0,path_.getsize()-1);
   }

// calculate the path subdivision bottom-up
void mininode_geometry_path_clod::calcpath(int left,int right)
   {
   if (right-left>1)
      {
      int center=(left+right)/2;

      float d2=d2_[center];
      float md=md_[center];

      miniv3d a=path_.get(left).getpos();
      miniv3d b=path_.get(right).getpos();

      double d=(b-a).getlength();
      double l=distance2line(EYE_,a,b);

      if (d2*d>(l-md)*C_)
         {
         calcpath(left,center);
         calcpath(center,right);
         }
      }
   }
