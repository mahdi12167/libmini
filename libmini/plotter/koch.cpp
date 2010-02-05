#include <mini/minibase.h>

#include <plotter/plot.h>

static const float sqrt34=sqrt(0.75f);

void koch(int n,
          float x1,float y1,
          float x2,float y2);

void step(int n,
          float *x,float *y,
          float dx,float dy)
   {
   float sx,sy;

   sx=*x+dx;
   sy=*y+dy;

   koch(n-1,*x,*y,sx,sy);

   *x=sx;
   *y=sy;
   }

void koch(int n,
          float x1,float y1,
          float x2,float y2)
   {
   float dx,dy;
   float rx,ry;

   dx=(x2-x1)/3;
   dy=(y2-y1)/3;

   rx=-dy*sqrt34;
   ry=dx*sqrt34;

   if (n>0)
      {
      step(n,&x1,&y1,dx,dy);
      step(n,&x1,&y1,dx/2+rx,dy/2+ry);
      step(n,&x1,&y1,dx/2-rx,dy/2-ry);
      step(n,&x1,&y1,dx,dy);
      }
   else
      plot_line(x1,y1,x2,y2);
   }

void render(double time)
   {
   int n;

   n=((int)time)%6;

   plot_color(1.0f,1.0f,1.0f);

   koch(n,
        0.8f,0.333f,
        0.2f,0.333f);

   koch(n,
        0.2f,0.333f,
        0.5f,0.333f+0.6*sqrt34);

   koch(n,
        0.5f,0.333f+0.6*sqrt34,
        0.8f,0.333f);
   }

int main(int argc,char *argv[])
   {
   plot_openwindow(&argc,argv,
                   512,512,
                   0.0f,0.0f,0.0f,
                   render);

   return(0);
   }
