#include "minibase.h"

#include "minimath.h"
#include "minisort.h"

#include "minicurve.h"

void minicurve::append_sector(const minicoord &p1,const minicoord &p2,
                              int n,double maxl)
   {
   minicoord a=p1;
   minicoord b=p2;

   a.convert2ecef();
   b.convert2ecef();

   if (n<2) n=2;

   bisect(a,b,0,ceil(log((double)n)/log(2.0)-0.5)-1);
   sample(maxl);
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

void minicurve::sample(double maxl)
   {
   unsigned int i;

   minicurve curve;
   unsigned int n;

   double l;
   double r,minr;

   if (getsize()<2 || maxl<=0.0) return;

   minr=1.0;

   for (i=0; i<getsize()-1; i++)
      {
      l=(get(i+1)-get(i)).vec.getlength();

      r=maxl/l;
      if (r<minr) minr=r;
      }

   n=ceil((getsize()-1)/minr)+1;

   if (n>getsize())
      {
      for (i=0; i<n; i++)
         curve.append(interpolate_cubic((double)i/(n-1)));

      *this=curve;
      }
   }
