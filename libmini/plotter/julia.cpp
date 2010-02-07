#include <mini/minibase.h>

#include <plotter/plot.h>

void julia()
   {
   int i,j;

   int width,height;

   width=get_winwidth();
   height=get_winheight();

   for (i=0; i<width; i++)
      plot_point((float)i/width,0.5);

   for (j=0; j<height; j++)
      plot_point(0.5,(float)j/height);
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
