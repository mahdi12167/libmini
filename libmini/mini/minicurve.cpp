#include "minibase.h"

#include "minimath.h"

#include "minicurve.h"

void minicurve::append_sector(const minicoord &p1,const minicoord &p2,int n)
   {
   minicoord a=p1;
   minicoord b=p2;

   if (a.type!=minicoord::MINICOORD_LINEAR) a.convert2(minicoord::MINICOORD_ECEF);
   if (b.type!=minicoord::MINICOORD_LINEAR) b.convert2(minicoord::MINICOORD_ECEF);

   if (n<1) n=1;

   append(a);
   bisect(a,b,ceil(log((double)n)/log(2.0)));
   }

void minicurve::bisect(const minicoord &p1,const minicoord &p2,int level)
   {
   minicoord a=p1;
   minicoord b=p2;

   if (a.type!=minicoord::MINICOORD_LINEAR) a.convert2(minicoord::MINICOORD_LLH);
   if (b.type!=minicoord::MINICOORD_LINEAR) b.convert2(minicoord::MINICOORD_LLH);

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

   if (level>0)
      {
      bisect(p1,p12,level-1);
      bisect(p12,p2,level-1);
      }
   else
      {
      append(p12);
      append(p2);
      }
   }
