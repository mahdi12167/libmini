// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minidefs.h"

#include "minirgb.h"
#include "minimath.h"

#include "miniclod.h"

// default constructor
miniclod::miniclod()
   {
   UPDATED_=FALSE;
   UPDATE_=0;

   EYE_=EYE0_=miniv3d(NAN,NAN,NAN);
   }

// destructor
miniclod::~miniclod()
   {}

// set path
void miniclod::set(const minipath &path)
   {
   path0_=path;
   UPDATED_=TRUE;
   }

// load path
void miniclod::load(ministring filename)
   {
   path0_.load(filename);
   UPDATED_=TRUE;
   }

// recreate geometry from actual view point
void miniclod::create(miniv3d eye,
                      double maxdev,double atdist,
                      double maxwidth,
                      double minv,double maxv,double sat,double val,
                      double weight,
                      int update)
   {
   UPDATE_=update;

   EYE0_=EYE_;
   EYE_=eye;

   C_=maxdev/atdist;
   D_=atdist;
   W_=maxwidth/atdist;

   MINV_=minv;
   MAXV_=maxv;
   SAT_=sat;
   VAL_=val;

   WEIGHT_=weight;

   UPDATE_=update;

   updateDX();
   calcpath();
   }

// incrementally recreate geometry from actual view point
void miniclod::create_inc(miniv3d eye)
   {
   updateDX();
   calcpath_inc(eye,UPDATE_);
   }

// update delta values
void miniclod::updateDX()
   {
   if (UPDATED_)
      {
      path_=path0_;
      path0_.clear();

      calcDC();
      calcD2();

      UPDATED_=FALSE;

      EYE0_=miniv3d(NAN,NAN,NAN);
      }
   }

// calculate the dc-values
void miniclod::calcDC()
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
float miniclod::calcD2(int left,int right,int center)
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
float miniclod::calcDM(int left,int right)
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
void miniclod::calcD2()
   {
   d2_.setsize(path_.getsize(),0.0f);
   dm_.setsize(path_.getsize(),0.0f);

   if (!path_.empty())
      calcD2(0,path_.getsize()-1);
   }

// propagate the d2-values top-down
float miniclod::calcD2(int left,int right)
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
void miniclod::addpoint(const minimeas &m,BOOLINT start)
   {
   miniv3d p;
   miniv3f n;
   double v;

   double d;
   float hue;
   miniv3f rgb;

   p=m.getpos();

   n=p;
   n.normalize();

   v=m.velocity;

   d=(p-EYE_).getlength();
   if (d<D_) d=D_;

   v=(v-MINV_)/(MAXV_-MINV_);
   hue=(1.0-v)*240.0;

   if (hue<0.0f) hue=0.0f;
   else if (hue>240.0f) hue=240.0f;

   rgb=point2rgb(m,v,hue,SAT_,VAL_);

   if (start)
      if (!POINTS_.empty())
         {
         vec3 lp=POINTS_.last().pos;
         vec3f ln=POINTS_.last().nrm;
         vec3f lc=POINTS_.last().col;

         mini3D::joint_struct point1={lp,ln,lc,0.0};
         POINTS_.push_back(point1);

         mini3D::joint_struct point2={p,n,rgb,0.0};
         POINTS_.push_back(point2);
         }

   mini3D::joint_struct point={p,n,rgb,float(W_*d)};
   POINTS_.push_back(point);
   }

// subdivide a segment
BOOLINT miniclod::subdiv(int left,int right)
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
void miniclod::calcpath()
   {
   POINTS_.clear();

   if (!path_.empty())
      {
      addpoint(path_.first());
      calcpath(0,path_.getsize()-1);
      addpoint(path_.last());
      }

   updated(POINTS_);
   }

// calculate the path subdivision bottom-up
void miniclod::calcpath(int left,int right)
   {
   if (subdiv(left,right))
      {
      int center=(left+right)/2;
      minimeas c=path_.get(center);

      calcpath(left,center);
      addpoint(c,c.start);
      calcpath(center,right);
      }
   }

// calculate the path incrementally
void miniclod::calcpath_inc(miniv3d eye,int update)
   {
   int i;

   if (!path_.empty() && update>0)
      {
      if (STACK_.empty())
         {
         EYE0_=EYE_;
         EYE_=eye;

         if (EYE_!=EYE0_)
            {
            POINTS_.clear();

            addpoint(path_.first());

            struct state_struct start={0,(int)path_.getsize()-1,FALSE};
            STACK_.push_back(start);
            }
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
            addpoint(path_.last());

            updated(POINTS_);
            }
         }
      }
   }

// calculate the path subdivision incrementally
void miniclod::calcpath_inc()
   {
   struct state_struct actual=STACK_.pop();

   int left=actual.left;
   int right=actual.right;

   if (actual.add)
      {
      minimeas a=path_.get(left);
      addpoint(a,a.start);
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

// map point measurement to rgb color
miniv3f miniclod::point2rgb(const minimeas &m,double v,
                            float hue,float sat,float val)
   {
   float rgb[3];

   hsv2rgb(hue,sat,val,rgb);

   return(miniv3f(rgb));
   }
