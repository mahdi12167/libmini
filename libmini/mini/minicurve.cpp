#include "minibase.h"

#include "minimath.h"

#include "minicurve.h"

void minicurve::append_sector(const minicoord &p1,const minicoord &p2,
                              int n,double maxc)
   {
   minicoord a=p1;
   minicoord b=p2;

   a.convert2ecef();
   b.convert2ecef();

   if (n<1) n=1;

   bisect(a,b,0,ceil(log((double)n)/log(2.0)-0.5)-1);
   smooth(maxc);
   }

void minicurve::bisect(const minicoord &p1,const minicoord &p2,
                       int level,int maxlevel)
   {
   minicoord a=p1;
   minicoord b=p2;

   a.convert2ecef();
   b.convert2ecef();

   double h1=a.vec.z;
   double h2=b.vec.z;

   minicoord p12=(p1+p2)/2.0;
   double h12=(h1+h2)/2.0;

   if (p12.type!=minicoord::MINICOORD_LINEAR)
      {
      p12.convert2(minicoord::MINICOORD_LLH);
      p12.vec.z=h12;
      p12.convert2(minicoord::MINICOORD_ECEF);
      }

   if (level==0) append(p1);
   if (level<maxlevel) bisect(p1,p12,level+1,maxlevel);
   append(p12);
   if (level<maxlevel) bisect(p12,p2,level+1,maxlevel);
   if (level==0) append(p2);
   }

void minicurve::smooth(double maxc)
   {
   double t;
   double ts0,ts;

   minicurve curve;

   if (SIZE<2 || maxc<=0.0) return;

   t=0.0;
   ts0=ts=1.0/(SIZE-1);

   curve.append(interpolate_cubic(0.0));

   while (t<1.0+0.5*ts)
      {
      minicoord x1=interpolate_cubic(t);
      minicoord x2=interpolate_cubic(t+2.0*ts);

      minicoord x12=0.5*(x1+x2);
      minicoord x12c=interpolate_cubic(t+ts);

      double dx2=(x2-x1).vec.getlength2();
      double dy2=(x12c-x12).vec.getlength2();

      while (dy2>maxc*maxc*dx2)
         {
         ts*=0.5;
         continue;
         }

      curve.append(x12c);
      t+=ts;

      while (dy2<0.25*maxc*maxc*dx2)
         {
         ts*=2.0;
         dy2*=4.0;
         }

      if (ts>ts0) ts=ts0;
      }

   curve.append(interpolate_cubic(1.0));

   *this=curve;
   }
