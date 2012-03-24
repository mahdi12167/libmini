// (c) by Stefan Roettger, licensed under GPL 2+

#include <mini/minibase.h>

#include <plotter/plot.h>

void fern(int n)
   {
   int i;

   double x,y;
   double xn,yn;

   double r,g,b;

   double z;

   static const double z1=0.01;
   static const double z2=0.07;
   static const double z3=0.07;
   static const double z4=0.85;

   x=0.0;
   y=0.0;

   r=0.0;
   g=0.0;
   b=0.0;

   for (i=0; i<n; i++)
      {
      z=drand48();

      if ((z-=z1)<0.0)
         {
         xn=0.0;
         yn=0.16*y;

         r=r*z1+0.75*(1.0-z1);
         g=g*z1+0.5*(1.0-z1);
         b=b*z1+0.1*(1.0-z1);
         }
      else if ((z-=z2)<0.0)
         {
         xn=0.2*x-0.26*y;
         yn=0.23*x+0.22*y+1.6;

         r=r*z2+0.25*(1.0-z2);
         g=g*z2+1.0*(1.0-z2);
         b=b*z2+0.75*(1.0-z2);
         }
      else if ((z-=z3)<0.0)
         {
         xn=-0.15*x+0.28*y;
         yn=0.26*x+0.24*y+0.44;

         r=r*z3+0.5*(1.0-z3);
         g=g*z3+1.0*(1.0-z3);
         b=b*z3+0.5*(1.0-z3);
         }
      else
         {
         xn=0.85*x+0.04*y;
         yn=-0.04*x+0.85*y+1.6;

         r=r*z4+0.1*(1.0-z4);
         g=g*z4+0.5*(1.0-z4);
         b=b*z4+0.1*(1.0-z4);
         }

      x=xn;
      y=yn;

      plot_color(r,g,b);
      plot_point(0.075*x+0.5,0.075*y+0.1);
      }
   }

void render(double time)
   {fern(200000);}

int main(int argc,char *argv[])
   {
   plot_openwindow(&argc,argv,
                   512,512,
                   1.0f,1.0f,1.0f,
                   render,
                   NULL,
                   FALSE);

   return(0);
   }
