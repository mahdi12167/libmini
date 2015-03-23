// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "miniclod.h"

// default constructor
miniCLOD::miniCLOD()
   {
   EYE_=EYE0_=vec3(NAN,NAN,NAN);

   UPDATED_=RECREATE_=FALSE;
   UPDATE_=0;
   }

// destructor
miniCLOD::~miniCLOD() {}

// set path
void miniCLOD::set(const minipath &path)
   {
   if (path!=path0_)
      {
      path0_=path;
      UPDATED_=TRUE;
      }
   }

// set paths
void miniCLOD::set(const minipaths &paths)
   {
   paths0_=minipaths(paths);
   paths0_.validate();
   path0_.clear();
   UPDATED_=TRUE;
   }

// append to paths
void miniCLOD::append(const minipath &path)
   {
   paths0_.append(path);
   paths0_.validate();
   path0_.clear();
   UPDATED_=TRUE;
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

   if (path!=path0_)
      {
      path0_=path;
      UPDATED_=TRUE;
      }
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

   paths0_=paths;
   paths0_.validate();
   path0_.clear();
   UPDATED_=TRUE;
   }

// append to paths
void miniCLOD::append(const std::string &csv)
   {
   minipath path;

   double max_delta;
   double max_length;
   double min_accuracy;

   paths0_.get_constraints(max_delta,max_length,min_accuracy);
   path.set_constraints(max_length,max_length,min_accuracy);
   path.set_orb(paths0_.get_orb());

   path.from_stdstring(csv);
   paths0_.append(path);
   paths0_.validate();
   path0_.clear();
   UPDATED_=TRUE;
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

   if (path!=path0_)
      {
      path0_=path;
      UPDATED_=TRUE;
      }
   }

// clear path
void miniCLOD::clear()
   {
   if (!path_.empty())
      {
      path0_.clear();
      paths0_.clear();
      UPDATED_=TRUE;
      }
   }

// recreate geometry from actual view point
void miniCLOD::create(vec3 eye,
                      double maxdev,double atdist,
                      double maxwidth,
                      double minv,double maxv,double sat,double val,
                      double weight,
                      int update)
   {
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
void miniCLOD::create_inc(vec3 eye)
   {
   updateDX();
   calcpath_inc(eye,UPDATE_);
   }

// update delta values
void miniCLOD::updateDX()
   {
   if (UPDATED_)
      {
      if (paths0_.size()>0)
         {
         path_=paths0_;
         path0_.validate();
         path_.append(path0_);
         path0_.clear();
         }
      else
         {
         path_=path0_;
         path_.validate();
         path0_.clear();
         }

      calcDC();
      calcD2();

      UPDATED_=FALSE;
      RECREATE_=TRUE;
      }
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
   if (c.start) d2=fmax(d2,1.0f);

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

   rgb=point2rgb(m,v,hue,SAT_,VAL_);

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
void miniCLOD::calcpath()
   {
   POINTS_.clear();

   if (!path_.empty())
      {
      addpoint(path_.front());
      calcpath(0,path_.size()-1);
      addpoint(path_.back());
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
         EYE0_=EYE_;
         EYE_=eye;

         if (EYE_!=EYE0_ || RECREATE_)
            {
            POINTS_.clear();

            if (!path_.empty())
               {
               addpoint(path_.front());

               struct state_struct start={0,(int)path_.size()-1,FALSE};
               STACK_.push_back(start);
               }
            else updated(POINTS_);

            RECREATE_=FALSE;
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
            addpoint(path_.back());

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
vec3f miniCLOD::point2rgb(const minimeas &m,double v,
                          float hue,float sat,float val)
   {return(hsv2rgb(hue,sat,val));}
