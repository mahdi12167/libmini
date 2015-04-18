// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "miniclod.h"

// default constructor
miniCLOD::miniCLOD()
   {
   UPDATED_=FALSE;
   UPDATE_=0;
   UPDATING_=FALSE;
   }

// destructor
miniCLOD::~miniCLOD() {}

// set path
void miniCLOD::set(const minipath &path)
   {
   block_on();

   if (path!=path0_)
      {
      path0_=path;
      UPDATED_=TRUE;
      }

   block_off();
   }

// set paths
void miniCLOD::set(const minipaths &paths)
   {
   block_on();

   paths0_=minipaths(paths);
   path0_.clear();
   UPDATED_=TRUE;

   block_off();
   }

// append to paths
void miniCLOD::append(minipath path)
   {
   path.validate();

   block_on();

   paths0_.append(path);
   path0_.clear();
   UPDATED_=TRUE;

   block_off();
   }

// read path
void miniCLOD::read(const std::string &csv,
                    double max_delta,
                    double max_length,
                    double min_accuracy,
                    int orb)
   {
   minipath path;

   path.set_constraints(max_delta,max_length,min_accuracy);
   path.set_orb(orb);

   path.from_stdstring(csv);

   block_on();

   if (path!=path0_)
      {
      path0_=path;
      UPDATED_=TRUE;
      }

   block_off();
   }

// read paths
void miniCLOD::read(const std::vector<std::string> &csvs,
                    double max_delta,
                    double max_length,
                    double min_accuracy,
                    int orb)
   {
   minipaths paths(max_delta,max_length,min_accuracy,orb);
   paths.from_stdstrings(csvs);

   block_on();

   paths0_=paths;
   path0_.clear();
   UPDATED_=TRUE;

   block_off();
   }

// append to paths
void miniCLOD::append(const std::string &csv,
                      double max_delta,
                      double max_length,
                      double min_accuracy,
                      int orb)
   {
   minipath path;

   path.set_constraints(max_delta,max_length,min_accuracy);
   path.set_orb(orb);

   path.from_stdstring(csv);
   path.validate();

   block_on();

   paths0_.append(path);
   path0_.clear();
   UPDATED_=TRUE;

   block_off();
   }

// load path
void miniCLOD::load(ministring filename,
                    double max_delta,
                    double max_length,
                    double min_accuracy,
                    int orb)
   {
   minipath path;

   path.set_constraints(max_delta,max_length,min_accuracy);
   path.set_orb(orb);

   path.load(filename);

   block_on();

   if (path!=path0_)
      {
      path0_=path;
      UPDATED_=TRUE;
      }

   block_off();
   }

// clear path
void miniCLOD::clear()
   {
   block_on();

   if (!path0_.empty() || !paths0_.empty())
      {
      path0_.clear();
      paths0_.clear();
      UPDATED_=TRUE;
      }

   block_off();
   }

// get actual path
minipath miniCLOD::getpath()
   {
   minipath path;

   block_on();

   paths0_.validate();
   path=paths0_;
   path0_.validate();
   path.append(path0_);
   path.validate();

   block_off();

   return(path);
   }

// recreate geometry from actual view point
void miniCLOD::create(vec3 eye,
                      double maxdev,double atdist,
                      double maxwidth,
                      double minv,double maxv,
                      double sat,double val,double alpha,
                      double weight,double start,
                      int update)
   {
   block_on();

   if (!UPDATING_)
      {
      UPDATING_=TRUE;
      block_off();

      C_=maxdev/atdist;
      D_=atdist;
      W_=maxwidth/atdist;

      MINV_=minv;
      MAXV_=maxv;

      SAT_=sat;
      VAL_=val;
      ALPHA_=alpha;

      WEIGHT_=weight;
      START_=start;

      UPDATE_=update;

      calcpath(eye);

      UPDATING_=FALSE;
      }
   else
      block_off();
   }

// incrementally recreate geometry from actual view point
void miniCLOD::create_inc(vec3 eye)
   {
   block_on();

   if (!UPDATING_)
      {
      UPDATING_=TRUE;
      block_off();

      calcpath_inc(eye,UPDATE_);

      UPDATING_=FALSE;
      }
   else
      block_off();
   }

// update delta values
BOOLINT miniCLOD::updateDX()
   {
   block_on();

   if (UPDATED_)
      {
      paths0_.validate();
      path_=paths0_;
      path0_.validate();
      path_.append(path0_);
      path_.validate();

      UPDATED_=FALSE;
      block_off();

      calcDC();
      calcD2();

      return(TRUE);
      }
   else
      block_off();

   return(FALSE);
   }

// calculate the dc-values
void miniCLOD::calcDC()
   {
   unsigned int i;

   float dc;

   dc_.resize(path_.size(),0.0f);

   for (i=0; i<path_.size(); i++)
      {
      dc=(path_.get(i).velocity-MINV_)/(MAXV_-MINV_);

      if (dc<0.0f) dc=0.0f;
      if (dc>1.0f) dc=1.0f;

      dc_[i]=dc*WEIGHT_;
      }
   }

// calculate a d2-value
float miniCLOD::calcD2(int left,int right,int center)
   {
   float d2,dc;

   vec3 a=path_.get(left).getpos();
   vec3 b=path_.get(right).getpos();
   minimeas c=path_.get(center);

   double d=(b-a).getlength();

   d2=distance2line(c.getpos(),a,b);

   // compute geometric deviation
   if (d>0.0) d2/=d;
   else d2=MAXFLOAT;

   // compute starting deviation
   if (c.start) d2=fmax(d2,START_);

   // compute constant deviation
   dc=fabs(dc_[center]-0.5*(dc_[left]+dc_[right]));
   if (dc>d2) d2=dc;

   return(d2);
   }

// calculate a dm-value
float miniCLOD::calcDM(int left,int right)
   {
   int i;

   float dm=0.0f;

   vec3 a=path_.get(left).getpos();
   vec3 b=path_.get(right).getpos();

   for (i=left+1; i<right-1; i++)
      {
      vec3 c=path_.get(i).getpos();
      double d=distance2line(c,a,b);

      if (d>dm) dm=d;
      }

   return(dm);
   }

// calculate the d2-values
void miniCLOD::calcD2()
   {
   d2_.resize(path_.size(),0.0f);
   dm_.resize(path_.size(),0.0f);

   if (!path_.empty())
      calcD2(0,path_.size()-1);
   }

// propagate the d2-values top-down
float miniCLOD::calcD2(int left,int right)
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
void miniCLOD::addpoint(const minimeas &m,BOOLINT start)
   {
   vec3 p,n;
   double v;

   double d;
   float hue;
   vec4f rgb;

   p=m.getpos();
   n=p.normalize();

   v=m.velocity;

   d=(p-EYE_).getlength();
   if (d<D_) d=D_;

   v=(v-MINV_)/(MAXV_-MINV_);
   hue=(1.0-v)*240.0;

   if (hue<0.0f) hue=0.0f;
   else if (hue>240.0f) hue=240.0f;

   rgb=point2rgb(m,v,hue,SAT_,VAL_,ALPHA_);

   if (start)
      if (!POINTS_.empty())
         {
         vec3 lp=POINTS_.back().pos;
         vec3f ln=POINTS_.back().nrm;
         vec4f lc=POINTS_.back().col;

         mini3D::joint_struct point1={lp,ln,lc,0.0};
         POINTS_.push_back(point1);

         mini3D::joint_struct point2={p,n,rgb,0.0};
         POINTS_.push_back(point2);
         }

   mini3D::joint_struct point={p,n,rgb,float(W_*d)};
   POINTS_.push_back(point);
   }

// subdivide a segment
BOOLINT miniCLOD::subdiv(int left,int right)
   {
   if (right-left<2) return(FALSE);

   int center=(left+right)/2;

   float d2=d2_[center];
   float dm=dm_[center];

   vec3 a=path_.get(left).getpos();
   vec3 b=path_.get(right).getpos();

   double d=(b-a).getlength();
   double l=distance2line(EYE_,a,b);

   return(d2*d>(l-dm)*C_);
   }

// calculate the path
void miniCLOD::calcpath(vec3 eye)
   {
   updateDX();

   EYE_=eye;

   POINTS_.clear();
   STACK_.clear();

   if (!path_.empty())
      {
      int last=path_.size()-1;

      addpoint(path_.get(0));
      calcpath(0,last);
      if (!path_.get(last).start) addpoint(path_.get(last));
      }

   updated(POINTS_);
   }

// calculate the path subdivision bottom-up
void miniCLOD::calcpath(int left,int right)
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
void miniCLOD::calcpath_inc(vec3 eye,int update)
   {
   int i;

   if (update>0)
      {
      if (STACK_.empty())
         {
         if (updateDX() || eye!=EYE_)
            {
            EYE_=eye;

            POINTS_.clear();

            if (!path_.empty())
               {
               int last=path_.size()-1;

               addpoint(path_.get(0));

               struct state_struct start={0,last,FALSE};
               STACK_.push_back(start);
               }
            else updated(POINTS_);
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
            int last=path_.size()-1;

            if (!path_.get(last).start) addpoint(path_.get(last));

            updated(POINTS_);
            }
         }
      }
   }

// calculate the path subdivision incrementally
void miniCLOD::calcpath_inc()
   {
   struct state_struct actual=STACK_.back();

   STACK_.pop_back();

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

// calculate the distance of a point p from a line segment between vectors a and b
double miniCLOD::distance2line(vec3 p,vec3 a,vec3 b)
   {
   vec3 n=(b-a).normalize();

   double l=(p-a).dot(n);
   vec3 h=a+l*n;

   double dh=(p-h).getlength2();
   double da=(p-a).getlength2();
   double db=(p-b).getlength2();

   if (dh<da && dh<db) return(sqrt(dh));
   if (da<db) return(sqrt(da));
   else return(sqrt(db));
   }

// hsv to rgb conversion
vec3f miniCLOD::hsv2rgb(float hue,float sat,float val)
   {
   vec3f rgb;

   float hue6,r,s,t;

   if (hue<0.0f || sat<0.0f || sat>1.0f || val<0.0f || val>1.0f) ERRORMSG();

   hue/=60.0f;
   hue=hue-6.0f*ftrc(hue/6.0f);
   hue6=hue-ftrc(hue);

   r=val*(1.0f-sat);
   s=val*(1.0f-sat*hue6);
   t=val*(1.0f-sat*(1.0f-hue6));

   switch (ftrc(hue))
        {
        case 0: // red -> yellow
           rgb = vec3f(val,t,r);
           break;
        case 1: // yellow -> green
           rgb = vec3f(s,val,r);
           break;
        case 2: // green -> cyan
           rgb = vec3f(r,val,t);
           break;
        case 3: // cyan -> blue
           rgb = vec3f(r,s,val);
           break;
        case 4: // blue -> magenta
           rgb = vec3f(t,r,val);
           break;
        case 5: // magenta -> red
           rgb = vec3f(val,r,s);
           break;
        }

   return(rgb);
   }

// map point measurement to rgb color
vec4f miniCLOD::point2rgb(const minimeas &m,double v,
                          float hue,float sat,float val,float alpha)
   {return(vec4f(hsv2rgb(hue,sat,val),alpha));}
