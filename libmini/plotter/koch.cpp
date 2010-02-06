#include <mini/minibase.h>

#include <plotter/plot.h>

static const float sqrt34=sqrt(0.75f);

void koch(int n,
          float dx,float dy)
   {
   float dx3,dy3;
   float rx34,ry34;

   dx3=dx/3;
   dy3=dy/3;

   rx34=-dy3*sqrt34;
   ry34=dx3*sqrt34;

   if (n>0)
      {
      koch(n-1,dx3,dy3);
      koch(n-1,dx3/2+rx34,dy3/2+ry34);
      koch(n-1,dx3/2-rx34,dy3/2-ry34);
      koch(n-1,dx3,dy3);
      }
   else
      plot_delta(dx,dy);
   }

void render(double time)
   {
   int n;

   n=((int)time)%6;

   plot_color(1.0f,1.0f,1.0f);

   plot_from(0.8f,0.333f);

   koch(n,-0.6f,0.0f);
   koch(n,0.3f,0.6f*sqrt34);
   koch(n, 0.3f,-0.6f*sqrt34);
   }

int main(int argc,char *argv[])
   {
   plot_openwindow(&argc,argv,
                   512,512,
                   0.0f,0.0f,0.0f,
                   render,
                   TRUE);

   return(0);
   }
