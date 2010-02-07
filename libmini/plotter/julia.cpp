#include <mini/minibase.h>

#include <plotter/plot.h>

void julia_color(double x,double y)
   {
   plot_color(x,0.0,y);
   }

void julia()
   {
   int i,j;

   int width,height;
   float x,y;

   width=get_winwidth();
   height=get_winheight();

   for (i=0; i<width; i++)
      for (j=0; j<height; j++)
         {
         x=(float)i/width;
         y=(float)j/height;

         julia_color(x,y);
         plot_point(x,y);
         }
   }

void render(double time)
   {julia();}

int main(int argc,char *argv[])
   {
   plot_openwindow(&argc,argv,
                   512,512,
                   1.0f,1.0f,1.0f,
                   render,
                   FALSE);

   return(0);
   }
