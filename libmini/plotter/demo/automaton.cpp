// (c) by Stefan Roettger, licensed under GPL 2+

#include <mini/minibase.h>

#include <plotter/plot.h>

static const int size=1000;
static const int b=2;

unsigned int code=3536951686;

BOOLINT keypress(unsigned char key,float x,float y)
   {
   if (tolower(key)=='x') exit(0);

   if (key==' ')
      {
      code=(1<<(1<<(2*b+1))-1)*drand48()+0.5;
      printf("code=%u\n",code);
      return(TRUE);
      }

   return(FALSE);
   }

void render(double time)
   {
   int i,j,k;

   int width,height;

   char state[size]={0};
   char state2[size]={0};

   width=get_winwidth();
   height=get_winheight();

   if (width>size) width=size;

   for (i=0; i<size; i++) state[i]=drand48()<0.5;

   for (i=0; i<height; i++)
      {
      for (j=0; j<size; j++)
         {
         unsigned int s0=0,s;

         for (k=-b; k<=b; k++)
            {
            if (j+k>=0 && j+k<size) s=state[j+k];
            else s=0;

            s0|=(s<<(k+b));
            }

         state2[j]=(code&(1<<s0))!=0;
         }

      for (j=0; j<size; j++) state[j]=state2[j];

      plot_color(0,0,0);
      for (j=0; j<width; j++)
         {
         double x=(j+0.5)/width;
         double y=1.0-(i+0.5)/height;

         if (state[j])
            plot_point(x,y);
         }
      }
   }

int main(int argc,char *argv[])
   {
   plot_openwindow(&argc,argv,
                   512,512,
                   1.0f,1.0f,1.0f,
                   render,
                   keypress,
                   NULL,
                   FALSE);

   return(0);
   }
