// (c) by Stefan Roettger

#include "minibase.h"

#include "minimath.h"
#include "minisort.h"

#include "minicurve.h"

void minicurve::append(const minimeas &p)
   {
   minimeas m=p;

   m.convert2ecef();
   minidyna<minimeas>::append(m);

   valid=FALSE;
   }

void minicurve::append(minicurve &c)
   {
   validate();
   c.validate();

   minidyna<minimeas>::append(c);

   valid=FALSE;
   }

void minicurve::merge(minicurve &c)
   {
   unsigned int i;

   minidyna<minimeas>::append(c);

   for (i=0; i<getsize(); i++) ref(i).start=0;

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

void minicurve::append_sector(const minicoord &p,
                              unsigned int n)
   {
   if (empty()) append(p);
   else append_sector(last(),p,n);
   }

void minicurve::append_point(const minicoord &p,double t,
                             unsigned int n)
   {
   minicoord p0=p;

   p0.vec.w=t;
   append_sector(p0,n);
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
   {shellsort<minimeas>(*this);}

void minicurve::validate()
   {
   unsigned int i,j;

   double t;

   if (!valid)
      {
      // check for missing time
      for (i=0; i<getsize();)
         if (isNAN(get(i).vec.w)) dispose(i);
         else i++;

      // sort by time
      sort();
      valid=TRUE;

      // check for missing time step
      if (get_time_step_max()==0.0)
         for (i=0; i<getsize(); i++)
            {
            t=(double)i/(getsize()-1);
            t=curve_start+t*(curve_stop-curve_start);
            ref(i).vec.w=t;
            }
      else
         // check for double time step
         for (i=0; i+1<getsize();)
            if (get(i).vec.w==get(i+1).vec.w) dispose(i+1);
            else i++;

      // check for missing points
      for (i=0; i<getsize();)
         if (isNAN(get(i).vec.x) || isNAN(get(i).vec.y)) dispose(i);
         else i++;

      // mark first time step
      if (!empty()) ref(0).start=TRUE;

      // check for missing height
      for (i=0; i<getsize(); i++)
         if (isNAN(get(i).vec.z))
            {
            // reduplicate height from following points
            for (j=i+1; j<getsize(); j++)
               if (!isNAN(get(j).vec.z))
                  {
                  ref(i).vec.z=get(j).vec.z;
                  break;
                  }

            // reduplicate height from previous points
            if (isNAN(get(i).vec.z))
               for (j=i; j>0;)
                  if (!isNAN(get(--j).vec.z))
                     {
                     ref(i).vec.z=get(j).vec.z;
                     break;
                     }
            }

      // initialize bbox
      bboxmin=miniv3d(MAXFLOAT,MAXFLOAT,MAXFLOAT);
      bboxmax=miniv3d(-MAXFLOAT,-MAXFLOAT,-MAXFLOAT);

      // compute bbox
      for (i=0; i<getsize(); i++)
         {
         miniv3d p=get(i).vec;

         if (p.x<bboxmin.x) bboxmin.x=p.x;
         if (p.x>bboxmax.x) bboxmax.x=p.x;

         if (p.y<bboxmin.y) bboxmin.y=p.y;
         if (p.y>bboxmax.y) bboxmax.y=p.y;

         if (p.z<bboxmin.z) bboxmin.z=p.z;
         if (p.z>bboxmax.z) bboxmax.z=p.z;
         }
      }
   }

void minicurve::set_time_mapping(double map_start,double map_stop)
   {
   curve_map_start=map_start;
   curve_map_stop=map_stop;
   }

void minicurve::set_time_repeat(double repeat_start,double repeat_stop)
   {
   curve_repeat_start=repeat_start;
   curve_repeat_stop=repeat_stop;
   }

minimeas minicurve::interpolate(double t)
   {return(interpolate_cubic(t));}

minimeas minicurve::interpolate_cubic(double t)
   {
   double tt;

   validate();

   if (curve_repeat_start<curve_repeat_stop)
      {
      t=(t-curve_repeat_start)/(curve_repeat_stop-curve_repeat_start);
      t-=floor(t);
      t=curve_repeat_start+t*(curve_repeat_stop-curve_repeat_start);
      }

   if (curve_map_start<curve_map_stop)
      {
      if (t<curve_map_start) t=curve_map_start;
      if (t>curve_map_stop) t=curve_map_stop;
      }

   tt=(t-get_time_start())/(get_time_stop()-get_time_start());

   return(minidyna<minimeas>::interpolate_cubic_directional(tt));
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

double minicurve::get_length()
   {
   unsigned int i;

   double length;

   validate();

   if (getsize()<2) return(0.0);

   length=0.0;

   for (i=0; i<getsize()-1; i++)
      {
      minimeas a=get(i);
      minimeas b=get(i+1);

      if (!b.start)
         {
         if (isNAN(a.vec.z) || isNAN(b.vec.z)) a.vec.z=b.vec.z=0.0;
         length+=(miniv3d(a.vec)-miniv3d(b.vec)).getlength();
         }
      }

   return(length);
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

   curve.append(minidyna<minimeas>::interpolate_cubic_directional(0.0));

   do
      {
      t+=dt;
      if (t>t1) t=t1;

      while (get(idx).vec.w<t) idx++;

      ta=get(idx-1).vec.w;
      tb=get(idx).vec.w;

      rt=(idx-1+(t-ta)/(tb-ta))/(getsize()-1);

      curve.append(minidyna<minimeas>::interpolate_cubic_directional(rt));
      }
   while (t<t1-0.5*dt);

   *this=curve;

   valid=TRUE;
   }

// get bounding box
void minicurve::getbbox(miniv3d &bboxmin,miniv3d &bboxmax) const
   {
   bboxmin=this->bboxmin;
   bboxmax=this->bboxmax;
   }

// serialization
ministrings minicurve::to_strings()
   {
   unsigned int i;

   ministrings curve;

   ministring info("minicurve");

   validate();

   info.append("(");

   info.append_double(curve_start);
   info.append(",");
   info.append_double(curve_stop);
   info.append(",");
   info.append_double(curve_map_start);
   info.append(",");
   info.append_double(curve_map_stop);
   info.append(",");
   info.append_double(curve_repeat_start);
   info.append(",");
   info.append_double(curve_repeat_stop);

   info.append(")");

   curve.append(info);

   for (i=0; i<getsize(); i++)
      curve.append(get(i).to_string());

   return(curve);
   }

// deserialization
void minicurve::from_strings(ministrings &infos)
   {
   unsigned int line;

   ministring info;

   if (!empty())
      {
      info=infos[0];

      if (info.startswith("minicurve"))
         {
         info=info.tail("minicurve(");

         curve_start=info.prefix(",").value();
         info=info.tail(",");
         curve_stop=info.prefix(",").value();
         info=info.tail(",");
         curve_map_start=info.prefix(",").value();
         info=info.tail(",");
         curve_map_stop=info.prefix(",").value();
         info=info.tail(",");
         curve_repeat_start=info.prefix(",").value();
         info=info.tail(",");
         curve_repeat_stop=info.prefix(")").value();
         info=info.tail(")");

         if (!info.empty()) return;

         line=1;

         while (line<infos.getsize())
            {
            minimeas meas;

            info=infos[line];
            meas.from_string(info);

            if (!info.empty()) return;

            append(meas);

            infos[line].clear();
            }

         infos.clear();

         valid=FALSE;
         }
      }
   }
