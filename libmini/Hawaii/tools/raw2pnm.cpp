#include "minibase.h"

#include "pnmbase.h"

int main(int argc,char *argv[])
   {
   int i;

   unsigned char *data;
   int bytes;

   unsigned char *image;
   int width,height,components;

   if (argc!=6)
      {
      printf("usage: %s <data.raw> width height components <image.pnm>\n",argv[0]);
      exit(1);
      }

   if (sscanf(argv[2],"%d",&width)!=1) exit(1);
   if (sscanf(argv[3],"%d",&height)!=1) exit(1);
   if (sscanf(argv[4],"%d",&components)!=1) exit(1);

   if (width<1 || height<1) exit(1);
   if (components<=0 && components!=-2) exit(1);

   if ((data=readfile(argv[1],&bytes))==NULL) exit(1);

   if (components==-2)
      {
      for (i=0; i<bytes-1; i+=2)
         {
         unsigned char tmp=data[i];
         data[i]=data[i+1];
         data[i+1]=tmp;
         }

      components=2;
      }

   if (bytes<width*height*components) exit(1);
   writePNMimage(argv[5],data,width,height,components);
   free(data);

   return(0);
   }
