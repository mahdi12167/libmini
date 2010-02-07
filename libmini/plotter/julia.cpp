#include <mini/minibase.h>
#include <mini/minitime.h>

#include <plotter/plot.h>

static const int julia_max_count=100;
static const int julia_cycle_count=10;

static const float julia_r=0.0f;
static const float julia_g=0.0f;
static const float julia_b=0.0f;

static const int lava_max_count=5;
static const int lava_cycle_count=6;

static const float lava_r=0.9f;
static const float lava_g=0.0f;
static const float lava_b=0.25f;

int max_count=julia_max_count;
int cycle_count=julia_cycle_count;

float solid_r=julia_r;
float solid_g=julia_g;
float solid_b=julia_b;

BOOLINT animation=TRUE;
BOOLINT lava=FALSE;

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

         if (lava)
            {
            jy+=1.0;
            jy/=1.5;
            jx*=pow(dabs(0.667+jy),0.2);
            }

         index=(double)(julia_index(jx,jy,reC,imC)%cycle_count)/(cycle_count-1);

         if (index<=0.0) plot_color(solid_r,solid_g,solid_b);
         else plot_color(index,0.0f,1.0-index);

         plot_point(x,y);
         }
   }

void render(double time)
   {
   double t;

   t=gettime();

   julia(julia_reC,julia_imC);

   t=gettime()-t;

   waitfor(1.0/25-t);

   if (animation)
      {
      double l1,l2;

      l1=0.2*sin(time)+0.25*sin(time/2)+dsqr(sin(time/5));
      l2=0.1*sin(2*time)+0.25*sin(time)+dsqr(sin(time/3));

      julia_reC=l1*sin(time/10)+l2*sin(time/3);
      julia_imC=l1*cos(time/10)+l2*cos(time/3);
      }
   }

BOOLINT keypress(unsigned char key,float x,float y)
   {
   if (key=='a') animation=!animation;
   else if (key=='l')
      {
      lava=!lava;

      if (lava)
         {
         max_count=lava_max_count;
         cycle_count=lava_cycle_count;

         solid_r=lava_r;
         solid_g=lava_g;
         solid_b=lava_b;
         }
      else
         {
         max_count=julia_max_count;
         cycle_count=julia_cycle_count;

         solid_r=julia_r;
         solid_g=julia_g;
         solid_b=julia_b;
         }
      }
   else if (key=' ')
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
                   TRUE);

   return(0);
   }
