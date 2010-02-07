#include <mini/minibase.h>

#include <plotter/plot.h>

void render(double time)
   {
   plot_color(1.0f,0.0f,0.0f);
   plot_line(0.0f,0.0f,1.0f,1.0f);

   plot_color(0.0f,0.0f,1.0f);
   plot_line(0.0f,1.0f,1.0f,0.0f);
   }

int main(int argc,char *argv[])
   {
   plot_openwindow(&argc,argv,
                   512,512,
                   1.0f,1.0f,1.0f,
                   render,
                   FALSE);

   return(0);
   }
