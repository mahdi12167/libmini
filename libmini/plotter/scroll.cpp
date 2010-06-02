#include <mini/minibase.h>

#include <plotter/plot.h>

BOOLINT scroll=FALSE;

BOOLINT keypress(unsigned char key,float x,float y)
   {
   if (tolower(key)=='x') exit(0);

   if (tolower(key)=='s') scroll=!scroll;

   return(FALSE);
   }

void render(double time)
   {
   int i,j;

   int width,height;

   double x,y;
   float r,g,b;

   double l;

   static double alpha=0.0,beta=0.0,gamma=0.0;

   width=get_winwidth();
   height=get_winheight();

   for (i=0; i<width; i++)
      for (j=0; j<height; j++)
         {
         x=(i+0.5)/width;
         y=(j+0.5)/height;

         l=fsqr(x-0.5)+fsqr(y-0.5);

         r=sin(300*l+alpha-PI);
         g=sin(300*l+beta);
         b=sin(300*l+gamma+PI);

         plot_color(r,g,b);
         plot_point(x,y);
         }

   if (scroll)
      {
      alpha+=0.15;
      beta+=0.3;
      gamma+=0.6;
      }
   }

int main(int argc,char *argv[])
   {
   plot_openwindow(&argc,argv,
                   512,512,
                   1.0f,1.0f,1.0f,
                   render,
                   keypress,
                   TRUE);

   return(0);
   }
