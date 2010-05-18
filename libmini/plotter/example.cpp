#include <mini/minibase.h>

#include <plotter/plot.h>

BOOLINT keypress(unsigned char key,float x,float y)
   {
   /* insert keyboard actions here */

   if (tolower(key)=='x') exit(0);

   return(FALSE); /* do not update window */
   }

void render(double time)
   {
   /* insert plot code here */

   plot_color(1.0f,0.0f,0.0f);
   plot_line(0.0f,0.0f,1.0f,1.0f);

   plot_color(0.0f,0.0f,1.0f);
   plot_line(0.0f,1.0f,1.0f,0.0f);
   }

int main(int argc,char *argv[])
   {
   plot_openwindow(&argc,argv,     /* main arguments */
                   512,512,        /* window size */
                   1.0f,1.0f,1.0f, /* background color */
                   render,         /* render function */
                   keypress,       /* keypress function */
                   FALSE);         /* render continuously */

   return(0);
   }
