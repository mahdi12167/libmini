#include <mini/minibase.h>

#include <plotter/plot.h>

void fern(int n)
   {
   int i;

   double x,y;
   double xn,yn;

   double z;

   x=0.0;
   y=0.0;

   for (i=0; i<n; i++)
      {
      z=drand48();

      if (z<0.01)
         {
         xn=0.0;
         yn=0.16*y;

         plot_color(0.75f,0.5f,0.0f);
         }
      else if (z<0.08)
         {
         xn=0.2*x-0.26*y;
         yn=0.23*x+0.22*y+1.6;

         plot_color(0.25f,1.0f,0.75f);
         }
      else if (z<0.15)
         {
         xn=-0.15*x+0.28*y;
         yn=0.26*x+0.24*y+0.44;

         plot_color(0.5f,1.0f,0.5f);
         }
      else
         {
         xn=0.85*x+0.04*y;
         yn=-0.04*x+0.85*y+1.6;

         plot_color(0.25f,1.0f,0.25f);
         }

      x=xn;
      y=yn;

      plot_point(0.075*x+0.5,0.075*y+0.1);
      }
   }

void render(double time)
   {fern(50000);}

int main(int argc,char *argv[])
   {
   plot_openwindow(&argc,argv,
                   512,512,
                   0.0f,0.0f,0.0f,
                   render,
                   FALSE);

   return(0);
   }
