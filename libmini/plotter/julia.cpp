#include <mini/minibase.h>

#include <plotter/plot.h>

static const int max_count=500;
static const int cycle_count=10;

double julia_reC=-0.158513;
double julia_imC=0.659491;

int julia_index(double reZ,double imZ,
                double reC,double imC)
   {
   int i;

   double reZn,imZn;

   for (i=1; i<=max_count; i++)
      {
      reZn=reZ*reZ-imZ*imZ+reC;
      imZn=2*reZ*imZ+imC;

      reZ=reZn;
      imZ=imZn;

      if (reZ*reZ+imZ*imZ>4) return(i);
      }

   return(0);
   }

void julia(double reC,double imC)
   {
   int i,j;

   int width,height;
   double x,y,jx,jy;

   double index;

   width=get_winwidth();
   height=get_winheight();

   for (i=0; i<width; i++)
      for (j=0; j<height; j++)
         {
         x=(double)i/width;
         y=(double)j/height;

         jx=4.0*x-2.0;
         jy=4.0*y-2.0;

         index=(double)(julia_index(jx,jy,reC,imC)%cycle_count)/(cycle_count-1);

         if (index==0) plot_color(0.0f,0.0f,0.0f);
         else plot_color(index,0.0f,1.0-index);

         plot_point(x,y);
         }
   }

void render(double time)
   {julia(julia_reC,julia_imC);}

BOOLINT keypress(unsigned char key,float x,float y)
   {
   if (key=' ')
      {
      julia_reC=2.0*x-1.0;
      julia_imC=-2.0*y+1.0;

      printf("actual julia parameter: (%g,%g)\n",julia_reC,julia_imC);

      return(TRUE);
      }

   return(FALSE);
   }

int main(int argc,char *argv[])
   {
   plot_openwindow(&argc,argv,
                   512,512,
                   1.0f,1.0f,1.0f,
                   render,
                   keypress,
                   FALSE);

   return(0);
   }
