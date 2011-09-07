#include "minibase.h"

#include "minimath.h"

#include "minianim.h"

minianim::minianim()
   : minidyna<minicoord>()
   {cam=NULL;}

minianim::minianim(minicam *c)
   : minidyna<minicoord>()
   {cam=c;}

minianim::minianim(minicam *c,const minicoord &v)
  : minidyna<minicoord>(v)
   {cam=c;}

void minianim::append_sector(const minicoord &p1,const minicoord &p2,unsigned int n)
   {
   minicoord a=p1;
   minicoord b=p2;

   if (a.type!=minicoord::MINICOORD_LINEAR) a.convert2(minicoord::MINICOORD_ECEF);
   if (b.type!=minicoord::MINICOORD_LINEAR) b.convert2(minicoord::MINICOORD_ECEF);

   double ha=cam->get_elev(a);
   double hb=cam->get_elev(b);

   append(a);

   for (unsigned int i=1; i<n-1; i++)
      {
      double t=(double)i/n;

      minicoord ab=(1.0-t)*a+t*b;
      double hab=(1.0-t)*ha+t*hb;

      if (cam!=NULL)
         {
         cam->move_above(ab);
         double h=cam->get_elev(ab);
         cam->move_down(h-hab);
         }

      append(ab);
      }

   append(b);
   }

minicoord minianim::interpolate(double t)
   {
   unsigned int i;

   unsigned int n=getsize();

   if (n==0) return(minicoord());

   if (n==1) return(get(0));
   if (t<=0.0) return(get(0));
   if (t>=1.0) return(get(n-1));

   t=t*(n-1);
   i=floor(t);
   t=t-i;

   return((1.0-t)*get(i)+t*get(i+1));
   }
