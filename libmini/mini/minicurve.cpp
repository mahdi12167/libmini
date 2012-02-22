// (c) by Stefan Roettger

#include "minibase.h"

#include "minimath.h"
#include "minisort.h"

#include "minicurve.h"

void minicurve::append(const minicoord &p)
   {
   minicoord c=p;

   c.convert2ecef();
   minidyna<minicoord>::append(c);

   valid=FALSE;
   }

void minicurve::append_sector(const minicoord &p1,const minicoord &p2,
                              unsigned int n)
   {
   int maxlevel;

   minicoord a=p1;
   minicoord b=p2;

   a.convert2ecef();
   b.convert2ecef();

   if (n<2) n=2;

   maxlevel=ceil(log((double)n)/log(2.0)-0.5);
   bisect(a,b,0,maxlevel-1);
   }

void minicurve::bisect(const minicoord &p1,const minicoord &p2,
                       int level,int maxlevel)
   {
   minicoord a=p1;
   minicoord b=p2;

   a.convert2llh();
   b.convert2llh();

   double h1=a.vec.z;
   double h2=b.vec.z;

   minicoord p12=(p1+p2)/2.0;
   double h12=(h1+h2)/2.0;

   p12.convert2llh();
   p12.vec.z=h12;
   p12.convert2ecef();

   if (level==0)
      if (getsize()==0) append(p1);
      else if (p1!=last()) append(p1);

   if (level<maxlevel)
      {
      bisect(p1,p12,level+1,maxlevel);
      append(p12);
      bisect(p12,p2,level+1,maxlevel);
      }

   if (level==0) append(p2);
   }

void minicurve::sort()
   {shellsort<minicoord>(*this);}

void minicurve::validate()
   {
   unsigned int i;

   double t;

   if (!valid)
      {
      sort();
      valid=TRUE;

      if (get_time_step_max()==0.0)
         for (i=0; i<getsize(); i++)
            {
            t=(double)i/(getsize()-1);
            t=curve_start+t*(curve_stop-curve_start);
            ref(i).vec.w=t;
            }
      else
         for (i=0; i<getsize()-1; i++)
            if (get(i).vec.w==get(i+1).vec.w) dispose(i+1);
      }
   }

minicoord minicurve::interpolate(double t)
   {
   double tt;

   if (!valid)
      {
      validate();
      resample(get_time_step_min());
      }

   tt=(t-get_time_start())/(get_time_stop()-get_time_start());

   return(minidyna<minicoord>::interpolate(tt));
   }

minicoord minicurve::interpolate_cubic(double t)
   {
   double tt;

   if (!valid)
      {
      validate();
      resample(get_time_step_min());
      }

   tt=(t-get_time_start())/(get_time_stop()-get_time_start());

   return(minidyna<minicoord>::interpolate_cubic(tt));
   }

double minicurve::get_time_start()
   {
   validate();
   return(first().vec.w);
   }

double minicurve::get_time_stop()
   {
   validate();
   return(last().vec.w);
   }

double minicurve::get_time_period()
   {
   validate();
   return(last().vec.w-first().vec.w);
   }

double minicurve::get_time_step_min()
   {
   unsigned int i;

   double dt,mdt;

   validate();

   if (getsize()<2) return(0.0);

   mdt=get(1).vec.w-get(0).vec.w;

   for (i=1; i<getsize()-1; i++)
      {
      dt=get(i+1).vec.w-get(i).vec.w;
      if (dt<mdt) mdt=dt;
      }

   return(mdt);
   }

double minicurve::get_time_step_max()
   {
   unsigned int i;

   double dt,mdt;

   validate();

   if (getsize()<2) return(0.0);

   mdt=get(1).vec.w-get(0).vec.w;

   for (i=1; i<getsize()-1; i++)
      {
      dt=get(i+1).vec.w-get(i).vec.w;
      if (dt>mdt) mdt=dt;
      }

   return(mdt);
   }

double minicurve::get_time_step_avg()
   {
   unsigned int i;

   double dt,sdt;

   validate();

   if (getsize()<2) return(0.0);

   sdt=get(1).vec.w-get(0).vec.w;

   for (i=1; i<getsize()-1; i++)
      {
      dt=get(i+1).vec.w-get(i).vec.w;
      sdt+=dt;
      }

   return(sdt/(getsize()-1));
   }

void minicurve::resample(double dt)
   {
   double t,rt;
   double t0,t1;

   unsigned int idx;
   double ta,tb;

   minicurve curve;

   validate();

   if (getsize()<2 || dt<=0.0) return;

   t0=get_time_start();
   t1=get_time_stop();

   t=t0;
   idx=1;

   curve.append(minidyna<minicoord>::interpolate_cubic(0.0));

   do
      {
      t+=dt;
      if (t>t1) t=t1;

      while (get(idx).vec.w<t) idx++;

      ta=get(idx-1).vec.w;
      tb=get(idx).vec.w;

      rt=(idx-1+(t-ta)/(tb-ta))/(getsize()-1);

      curve.append(minidyna<minicoord>::interpolate_cubic(rt));
      }
   while (t<t1-0.5*dt);

   *this=curve;

   valid=TRUE;
   }
