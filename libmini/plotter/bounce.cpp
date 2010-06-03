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
   int i;

   static double px=0.5;
   static double py=0.75;

   static double vx=0.2;
   static double vy=0.0;

   static double ax=0.0;
   static double ay=-0.5;

   static const double radius=0.02;
   static const double delta=0.02;

   static const int trail=101;
   static const int step=10;

   static double tpx[trail];
   static double tpy[trail];

   static BOOLINT init=FALSE;

   if (!init)
      {
      for (i=0; i<trail; i++)
         {
         tpx[i]=px;
         tpy[i]=py;
         }

      init=TRUE;
      }

   plot_linewidth(3);

   for (i=trail-1; i>=0; i-=step)
      {
      plot_color((double)i/trail,(double)i/trail,1);
      plot_circle(tpx[i],tpy[i],radius);
      }

   if (bounce)
      {
      vx+=ax*delta;
      vy+=ay*delta;

      px+=vx*delta;
      py+=vy*delta;

      if (px<radius || px>1-radius) vx=-vx;
      if (py<radius || py>1-radius) vy=-vy;
      }

   for (i=trail-1; i>=0; i--)
      {
      tpx[i]=tpx[i-1];
      tpy[i]=tpy[i-1];
      }

   tpx[0]=px;
   tpy[0]=py;
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
