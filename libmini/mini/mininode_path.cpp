// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minirgb.h"
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
   {
   *(mininode_geometry *)this=mininode_geometry_band_path(path_,width);
   setZscale(0.9f);
   }

// default constructor
mininode_geometry_path_clod::mininode_geometry_path_clod()
   : mininode_geometry(0,3,0)
   {
   UPDATED_=FALSE;
   UPDATE_=0;
   }

// destructor
mininode_geometry_path_clod::~mininode_geometry_path_clod()
   {}

// load path
void mininode_geometry_path_clod::load(ministring filename)
   {
   path_.load(filename);
   UPDATED_=TRUE;
   }

// recreate geometry from actual view point
void mininode_geometry_path_clod::create(double maxdev,double atdist,
                                         double maxwidth,
                                         double minv,double maxv,double sat,double val,
                                         double weight,
                                         int update)
   {
   EYE_=mininode_culling::peek_view();

   C_=maxdev/atdist;
   D_=atdist;
   W_=maxwidth/atdist;

   MINV_=minv;
   MAXV_=maxv;
   SAT_=sat;
   VAL_=val;

   WEIGHT_=weight;

   UPDATE_=update;

   if (UPDATED_)
      {
      calcDC();
      calcD2();

      UPDATED_=FALSE;
      }

   calcpath();
   }

// calculate the dc-values
void mininode_geometry_path_clod::calcDC()
   {
   unsigned int i;

   float dc;

   dc_.setsize(path_.getsize(),0.0f);

   for (i=0; i<path_.getsize(); i++)
      {
      dc=(path_.get(i).velocity-MINV_)/(MAXV_-MINV_);

      if (dc<0.0f) dc=0.0f;
      if (dc>1.0f) dc=1.0f;

      dc_[i]=dc*WEIGHT_;
      }
   }

// calculate a d2-value
float mininode_geometry_path_clod::calcD2(int left,int right,int center)
   {
   float d2,dc;

   miniv3d a=path_.get(left).getpos();
   miniv3d b=path_.get(right).getpos();
   minimeas c=path_.get(center);

   double d=(b-a).getlength();

   d2=distance2line(c.getpos(),a,b);

   // compute geometric deviation
   if (d>0.0) d2/=d;
   else d2=MAXFLOAT;

   // compute starting deviation
   if (c.start) d2=fmax(d2,1.0f);

   // compute constant deviation
   dc=fabs(dc_[center]-0.5*(dc_[left]+dc_[right]));
   if (dc>d2) d2=dc;

   return(d2);
   }

// calculate a dm-value
float mininode_geometry_path_clod::calcDM(int left,int right)
   {
   int i;

   float dm=0.0f;

   miniv3d a=path_.get(left).getpos();
   miniv3d b=path_.get(right).getpos();

   for (i=left+1; i<right-1; i++)
      {
      miniv3d c=path_.get(i).getpos();
      double d=distance2line(c,a,b);

      if (d>dm) dm=d;
      }

   return(dm);
   }

// calculate the d2-values
void mininode_geometry_path_clod::calcD2()
   {
   d2_.setsize(path_.getsize(),0.0f);
   dm_.setsize(path_.getsize(),0.0f);

   if (!path_.empty())
      calcD2(0,path_.getsize()-1);
   }

// propagate the d2-values top-down
float mininode_geometry_path_clod::calcD2(int left,int right)
   {
   float d2=0.0f;

   if (right-left>1)
      {
      int center=(left+right)/2;

      float d2l=calcD2(left,center);
      float d2r=calcD2(center,right);

      d2=calcD2(left,right,center);

      d2=fmax(d2,0.5f*d2l);
      d2=fmax(d2,0.5f*d2r);

      d2_[center]=d2;
      dm_[center]=calcDM(left,right);
      }

   return(d2);
   }

// add a point
void mininode_geometry_path_clod::addpoint(miniv3d p,double v,BOOLINT start)
   {
   miniv3d n;

   double d;
   float hue,rgb[3];

   n=p;
   n.normalize();

   d=(p-EYE_).getlength();
   if (d<D_) d=D_;

   hue=(1.0-(v-MINV_)/(MAXV_-MINV_))*240.0;

   if (hue<0.0f) hue=0.0f;
   else if (hue>240.0f) hue=240.0f;

   hsv2rgb(hue,SAT_,VAL_,rgb);

   if (start)
      if (!POS_.empty())
         {
         miniv3d lp=POS_.last();
         miniv3d ln=NRM_.last();
         miniv3d lc=COL_.last();

         POS_.push_back(lp);
         NRM_.push_back(ln);
         COL_.push_back(lc);
         WDT_.push_back(0.0);

         POS_.push_back(p);
         NRM_.push_back(n);
         COL_.push_back(miniv3d(rgb));
         WDT_.push_back(0.0);
         }

   POS_.push_back(p);
   NRM_.push_back(n);
   COL_.push_back(miniv3d(rgb));
   WDT_.push_back(W_*d);
   }

// subdivide a segment
BOOLINT mininode_geometry_path_clod::subdiv(int left,int right)
   {
   if (right-left<2) return(FALSE);

   int center=(left+right)/2;

   float d2=d2_[center];
   float dm=dm_[center];

   miniv3d a=path_.get(left).getpos();
   miniv3d b=path_.get(right).getpos();

   double d=(b-a).getlength();
   double l=distance2line(EYE_,a,b);

   return(d2*d>(l-dm)*C_);
   }

// calculate the path
void mininode_geometry_path_clod::calcpath()
   {
   if (!path_.empty())
      {
      POS_.clear();
      NRM_.clear();
      COL_.clear();
      WDT_.clear();

      addpoint(path_.first().getpos(),path_.first().velocity);
      calcpath(0,path_.getsize()-1);
      addpoint(path_.last().getpos(),path_.last().velocity);

      *(mininode_geometry *)this=mininode_geometry_band(POS_,NRM_,COL_,WDT_);
      }
   }

// calculate the path subdivision bottom-up
void mininode_geometry_path_clod::calcpath(int left,int right)
   {
   if (subdiv(left,right))
      {
      int center=(left+right)/2;
      minimeas c=path_.get(center);

      calcpath(left,center);
      addpoint(c.getpos(),c.velocity,c.start);
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
         POS_.clear();
         NRM_.clear();
         COL_.clear();
         WDT_.clear();

         EYE_=mininode_culling::peek_view();

         addpoint(path_.first().getpos(),path_.first().velocity);

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
            addpoint(path_.last().getpos(),path_.last().velocity);

            *(mininode_geometry *)this=mininode_geometry_band(POS_,NRM_,COL_,WDT_);
            setZscale(0.9f);
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
      minimeas a=path_.get(left);
      addpoint(a.getpos(),a.velocity,a.start);
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
