#include <mini/minibase.h>

#include <plotter/plot.h>

BOOLINT bounce=FALSE;

BOOLINT keypress(unsigned char key,float x,float y)
   {
   if (tolower(key)=='x') exit(0);

   if (tolower(key)=='b') bounce=!bounce;

   return(FALSE);
   }

void render(double time)
   {
   static double px=0.5;
   static double py=0.75;

   static double vx=0.2;
   static double vy=0.0;

   static double ax=0.0;
   static double ay=-0.5;

   static const double radius=0.02;
   static const double delta=0.02;

   plot_linewidth(3);
   plot_color(0,0,1);

   plot_circle(px,py,radius);

   if (bounce)
      {
      vx+=ax*delta;
      vy+=ay*delta;

      px+=vx*delta;
      py+=vy*delta;

      if (px<radius || px>1-radius) vx=-vx;
      if (py<radius || py>1-radius) vy=-vy;
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
