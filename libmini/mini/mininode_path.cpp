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

// load path
void mininode_geometry_path_clod::load(ministring filename)
   {
   path_.load(filename);
   calcD2();
   }

// recreate geometry from actual view point
void mininode_geometry_path_clod::recreate(miniv3d eye,
                                           double maxdiff,double atdist)
   {
   EYE_=eye;
   C_=maxdiff/atdist;

   calcpath();
   }

// calculate the distance of a point from a line segment
double mininode_geometry_path_clod::calcdist(miniv3d a,miniv3d b,miniv3d p)
   {
   miniv3d n=b-a;
   n.normalize();

   double l=(p-a)*n;
   miniv3d h=a+l*n;

   double dh=(p-h).getlength2();
   double da=(p-a).getlength2();
   double db=(p-b).getlength2();

   if (dh<da && dh<db) return(sqrt(dh));
   if (da<db) return(sqrt(da));
   else return(sqrt(db));
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

      double d=(path_.get(right).getpos()-
                path_.get(left).getpos()).getlength();

      d2=calcdist(path_.get(left).getpos(),
                  path_.get(right).getpos(),
                  path_.get(center).getpos());

      if (d>0.0) d2/=d;
      else d2=MAXFLOAT;

      d2=fmax(d2,0.5f*d2l);
      d2=fmax(d2,0.5f*d2r);

      d2_[center]=d2;

      for (i=left+1; i<right-1; i++)
         {
         double d=calcdist(path_.get(left).getpos(),
                           path_.get(right).getpos(),
                           path_.get(i).getpos());

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

      double d=(path_.get(right).getpos()-
                path_.get(left).getpos()).getlength();

      double l=calcdist(path_.get(left).getpos(),
                        path_.get(right).getpos(),
                        EYE_);

      if (d2*d>(l-md)*C_)
         {
         calcpath(left,center);
         calcpath(center,right);
         }
      }
   }
