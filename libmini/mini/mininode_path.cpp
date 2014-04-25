// (c) by Stefan Roettger, licensed under LGPL 2.1

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

// recreate geometry from actual view point
void mininode_geometry_path_clod::recreate(miniv3d eye,
                                           double maxsize,double maxdist)
   {
   calcD2();

   CODEERROR();
   }

// calculate the d2-values
void mininode_geometry_path_clod::calcD2()
   {
   d2_.setsize(path_.getsize(),MAXFLOAT);

   if (!path_.empty())
      calcD2(0,path_.getsize()-1);
   }

// calculate the d2-values
float mininode_geometry_path_clod::calcD2(int left,int right)
   {
   float d2=0.0f;

   if (right-left>1)
      {
      int center=(left+right)/2;

      float d2l=calcD2(left,center);
      float d2r=calcD2(center,right);

      miniv3d c=(path_.get(left).getpos()+path_.get(right).getpos())/2.0;

      d2=(path_.get(center).getpos()-c).getlength();

      d2=fmax(d2,d2l);
      d2=fmax(d2,d2r);

      d2_[center]=d2;
      }

   return(d2);
   }
