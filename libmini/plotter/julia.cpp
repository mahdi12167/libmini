#include <mini/minibase.h>
#include <mini/minicomplex.h>
#include <mini/minitime.h>

#include <plotter/plot.h>

static const float fps=25.0f;

static const int julia_max_count=50;
static const int julia_cycle_count=10;

static const float julia_r=0.0f;
static const float julia_g=0.0f;
static const float julia_b=0.0f;

static const int lava_max_count=5;
static const int lava_cycle_count=6;

static const float lava_r=1.0f;
static const float lava_g=0.25f;
static const float lava_b=0.0f;

int max_count=julia_max_count;
int cycle_count=julia_cycle_count;

float solid_r=julia_r;
float solid_g=julia_g;
float solid_b=julia_b;

BOOLINT animation=TRUE;
BOOLINT transition=FALSE;
BOOLINT lava=FALSE;

double julia_reC=-0.158513;
double julia_imC=0.659491;

double julia_time=0.0;

static const int julia_fmax=4;
double julia_fnum=0.0;

int julia_index(minicomplex z,minicomplex c,
                minicomplex (*f)(minicomplex z,minicomplex c),
                int max_count)
   {
   int i;

   for (i=1; i<=max_count; i++)
      {
      z=f(z,c);
      if (z.norm()>4) return(i);
      }

   return(0);
   }

void julia(minicomplex c,
           int max_count,
           minicomplex (*f)(minicomplex z,minicomplex c),
           minicomplex (*warp)(minicomplex z),
           void (*color)(int index,int max_count,float *r,float *g,float*b))
   {
   int i,j;

   int width,height;

   double x,y;
   minicomplex z;

   int index;
   float r,g,b;

   width=get_winwidth();
   height=get_winheight();

   for (i=0; i<width; i++)
      for (j=0; j<height; j++)
         {
         x=(i+0.5)/width;
         y=(j+0.5)/height;

         z=minicomplex(4.0*x-2.0,4.0*y-2.0);

         if (warp!=NULL) z=warp(z);

         index=julia_index(z,c,f,max_count);
         color(index,max_count,&r,&g,&b);

         plot_color(r,g,b);
         plot_point(x,y);
         }
   }

minicomplex julia_fn(int num,
                     minicomplex z,minicomplex c)
   {
   switch (num%julia_fmax)
      {
      default:
      case 0: return(z*z+c);
      case 1: return(0.5*z*z*z*z+z+c);
      case 2: return(2.0*(z*z*z*z*z-z*z)/(z*z*z+c));
      case 3: return((z*z*z*z*z*z*(z+1)-c)/(z*z*z*z*(z+1)+c));
      }
   }

minicomplex interpolate_fn(double num,
                           minicomplex z,minicomplex c)
   {
   int n;
   double w;

   n=ftrc(num);
   w=num-n;

   if (w==0.0) return(julia_fn(n,z,c));
   else return((1.0-w)*julia_fn(n,z,c)+w*julia_fn(n+1,z,c));
   }

minicomplex julia_f(minicomplex z,minicomplex c)
   {return(interpolate_fn(julia_fnum,z,c));}

void julia_color(int index,int max_count,
                 float *r,float *g,float *b)
   {
   double w;

   w=(double)(index%cycle_count)/(cycle_count-1);

   if (w<=0.0)
      {
      *r=solid_r;
      *g=solid_g;
      *b=solid_b;
      }
   else
      {
      *r=w;
      *g=0.0;
      *b=1.0-w;
      }
   }

minicomplex lava_warp(minicomplex z)
   {
   z.y+=1.0;
   z.y/=1.5;
   z.x*=pow(dabs(0.667+z.y),0.2);

   return(z);
   }

void render(double time)
   {
   double t;

   t=gettime();

   julia(minicomplex(julia_reC,julia_imC),
         max_count,
         julia_f,
         (lava)?lava_warp:NULL,
         julia_color);

   t=gettime()-t;

   waitfor(1.0/fps-t);

   if (animation)
      {
      double l1,l2;

      l1=0.2*sin(julia_time)+0.25*sin(julia_time/2)+dsqr(sin(julia_time/5));
      l2=0.1*sin(2*julia_time)+0.25*sin(julia_time)+dsqr(sin(julia_time/3));

      julia_reC=l1*sin(julia_time/10)+l2*sin(julia_time/3);
      julia_imC=l1*cos(julia_time/10)+l2*cos(julia_time/3);

      julia_time+=1.0/fps;
      if (transition) julia_fnum+=1.0/fps/180;
      }
   }

BOOLINT keypress(unsigned char key,float x,float y)
   {
   switch (key)
      {
      case 'a': animation=!animation; break;
      case 't': transition=!transition; break;
      case 'l':
         lava=!lava;

         if (lava)
            {
            animation=TRUE;

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

         break;
      case '>':
         julia_fnum=dtrc(julia_fnum+1)%julia_fmax;
         return(TRUE);
      case '<':
         julia_fnum=dtrc(julia_fnum+julia_fmax-1)%julia_fmax;
         return(TRUE);
      case ' ':
         animation=FALSE;

         julia_reC=2.0*x-1.0;
         julia_imC=-2.0*y+1.0;

         printf("actual julia parameter: (%g,%g)\n",julia_reC,julia_imC);

         return(TRUE);

      default: break;
      }

   return(FALSE);
   }

int main(int argc,char *argv[])
   {
   plot_openwindow(&argc,argv,
                   256,256,
                   1.0f,1.0f,1.0f,
                   render,
                   keypress,
                   TRUE);

   return(0);
   }
