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
void mininode_geometry_path_clod::create(double maxdiff,double atdist,
                                         int update)
   {
   EYE_=mininode_culling::peek_view();
   C_=maxdiff/atdist;
   UPDATE_=update;

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

// add a point
void mininode_geometry_path_clod::addpoint(miniv3d p)
   {
   BAND_.push_back(p);
   p.normalize();
   NRM_.push_back(p);
   WIDTH_.push_back(10.0); //!! adapt to distance
   }

// subdivide a segment
BOOLINT mininode_geometry_path_clod::subdiv(int left,int right)
   {
   if (right-left<2) return(FALSE);

   int center=(left+right)/2;

   float d2=d2_[center];
   float md=md_[center];

   miniv3d a=path_.get(left).getpos();
   miniv3d b=path_.get(right).getpos();

   double d=(b-a).getlength();
   double l=distance2line(EYE_,a,b);

   return(d2*d>(l-md)*C_);
   }

// calculate the path
void mininode_geometry_path_clod::calcpath()
   {
   if (!path_.empty())
      {
      BAND_.clear();
      NRM_.clear();
      WIDTH_.clear();

      addpoint(path_.first().getpos());
      calcpath(0,path_.getsize()-1);
      addpoint(path_.last().getpos());

      *(mininode_geometry *)this=mininode_geometry_band(BAND_,NRM_,WIDTH_);
      }
   }

// calculate the path subdivision bottom-up
void mininode_geometry_path_clod::calcpath(int left,int right)
   {
   if (subdiv(left,right))
      {
      int center=(left+right)/2;
      miniv3d c=path_.get(center).getpos();

      calcpath(left,center);
      addpoint(c);
      calcpath(center,right);
      }
   }

// calculate the path incrementally
void mininode_geometry_path_clod::calcpath_inc(int update)
   {
   int i;

   if (!path_.empty())
      {
      if (STACK_.empty())
         {
         BAND_.clear();
         NRM_.clear();
         WIDTH_.clear();

         addpoint(path_.first().getpos());

         struct state_struct start={0,path_.getsize()-1,FALSE};
         STACK_.push_back(start);
         }
      else
         {
         for (i=0; i<update; i++)
            {
            calcpath_inc();
            if (STACK_.empty()) break;
            }

         if (STACK_.empty())
            {
            addpoint(path_.last().getpos());

            *(mininode_geometry *)this=mininode_geometry_band(BAND_,NRM_,WIDTH_);
            }
         }
      }
   }

// calculate the path subdivision incrementally
void mininode_geometry_path_clod::calcpath_inc()
   {
   struct state_struct actual=STACK_.pop();

   int left=actual.left;
   int right=actual.right;

   if (actual.add)
      {
      miniv3d a=path_.get(left).getpos();
      addpoint(a);
      }

   if (subdiv(left,right))
      {
      int center=(left+right)/2;

      struct state_struct rseg={center,right,TRUE};
      STACK_.push_back(rseg);

      struct state_struct lseg={left,center,FALSE};
      STACK_.push_back(lseg);
      }
   }
