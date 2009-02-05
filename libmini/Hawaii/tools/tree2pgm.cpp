#include "minibase.h"

#include "pnmbase.h"

int main(int argc,char *argv[])
   {
   int i;

   unsigned char *image1;
   int width1,height1,components1;

   unsigned char *image2;
   int width2,height2,components2;

   float red,green,blue,nir;
   float redmax,greenmax,bluemax,nirmax;
   float ndvi,trees,treemin,treemax;

   if (argc!=4)
      {
      printf("usage: %s <rgb.ppm> <nir.pgm> <ndvi.pgm>\n",argv[0]);
      exit(1);
      }

   if ((image1=readPNMfile(argv[1],&width1,&height1,&components1))==NULL) exit(1);
   if ((image2=readPNMfile(argv[2],&width2,&height2,&components2))==NULL) exit(1);
   if (width1!=width2 || height1!=height2 || components1!=3 || components2!=1) exit(1);

   redmax=greenmax=bluemax=nirmax=1.0f/255.0f;

   for (i=0; i<width1*height1; i++)
      {
      red=image1[3*i]/255.0f;
      green=image1[3*i+1]/255.0f;
      blue=image1[3*i+2]/255.0f;
      nir=image2[i]/255.0f;

      if (red>redmax) redmax=red;
      if (green>greenmax) greenmax=green;
      if (blue>bluemax) bluemax=blue;
      if (nir>nirmax) nirmax=nir;
      }

   redmax*=255.0f;
   greenmax*=255.0f;
   bluemax*=255.0f;
   nirmax*=255.0f;

   treemin=1.0f;
   treemax=2.0f;

   for (i=0; i<width1*height1; i++)
      {
      red=image1[3*i]/redmax;
      green=image1[3*i+1]/greenmax;
      blue=image1[3*i+2]/bluemax;
      nir=image2[i]/nirmax;

      if (red==0.0f || nir==0.0f) ndvi=0.0f;
      else ndvi=(nir-red)/(nir+red);

      ndvi+=0.3f;

      if (ndvi<0.0f) ndvi=0.0f;
      else if (ndvi>1.0f) ndvi=1.0f;

      trees=ndvi*((1.0f-red)+green+(1.0f-blue));

      if (trees<treemin) treemin=trees;
      if (trees>treemax) treemax=trees;
      }

   for (i=0; i<width1*height1; i++)
      {
      red=image1[3*i]/redmax;
      green=image1[3*i+1]/greenmax;
      blue=image1[3*i+2]/bluemax;
      nir=image2[i]/nirmax;

      if (red==0.0f || nir==0.0f) ndvi=0.0f;
      else ndvi=(nir-red)/(nir+red);

      ndvi+=0.3f;

      if (ndvi<0.0f) ndvi=0.0f;
      else if (ndvi>1.0f) ndvi=1.0f;

      trees=ndvi*((1.0f-red)+green+(1.0f-blue));
      trees=(trees-treemin)/(treemax-treemin);

      image2[i]=ftrc(255.0f*trees+0.5f);
      }

   writePNMimage(argv[3],image2,width2,height2,1);

   return(0);
   }
