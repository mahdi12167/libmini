#include <mini/minibase.h>

#include <plotter/plot.h>

BOOLINT rotate=FALSE;

BOOLINT keypress(unsigned char key,float x,float y)
{
   if (tolower(key)=='x') exit(0);

   if (tolower(key)=='r') rotate=!rotate;

   return(FALSE);
}

void render(double time)
{
   static const double delta=0.01;
   static const double radius=0.01;
   static const double rounds=5;
   static const double speed=0.001;

   static double theta=0.0;

   double t;

   plot_linewidth(5);
   plot_color(0,1,0);

   plot_from(0.5,0.5);
   for (t=0.0; t<rounds*2*PI; t+=delta)
      plot_to(radius*t*sin(t+theta)+0.5,
              radius*t*cos(t+theta)+0.5);

   if (rotate) theta+=speed*time;
}

int main(int argc,char *argv[])
{
   plot_openwindow(&argc,argv,     /* main arguments */
                   512,512,        /* window size */
                   1.0f,1.0f,1.0f, /* background color */
                   render,         /* render function */
                   keypress,       /* keypress function */
                   TRUE);         /* render continuously */

   return(0);
}
