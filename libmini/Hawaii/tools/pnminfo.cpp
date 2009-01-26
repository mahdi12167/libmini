#include "minibase.h"

#include "pnmbase.h"

int main(int argc,char *argv[])
   {
   unsigned char *image;

   int width,height,
       components;

   char *comment;
   int bytes;

   float coord[8],cellsize[2],scaling;
   int missing;

   if (argc!=2 && argc!=3)
      {
      printf("usage: %s <image.pnm> [<comment.txt>]\n",argv[0]);
      exit(1);
      }

   image=readPNMfile(argv[1],
                     &width,&height,&components,
                     &comment);

   if (image==NULL) exit(1);

   if (argc==2)
      if (getPNMparamsLL(comment,
                         coord,cellsize,
                         &scaling,&missing)!=0)
         {
         printf("SW=%12f/%12f arc-seconds\n",coord[0],coord[1]);
         printf("NW=%12f/%12f arc-seconds\n",coord[2],coord[3]);
         printf("NE=%12f/%12f arc-seconds\n",coord[4],coord[5]);
         printf("SE=%12f/%12f arc-seconds\n",coord[6],coord[7]);
         printf("spacing=%g/%g arc-seconds\n",cellsize[0],cellsize[1]);
         printf("scale=%g meters\n",scaling);
         printf("missing=%d\n",missing);
         }
      else printf("no DEM info found\n");
   else
      {
      free(comment);
      if ((comment=(char *)readfile(argv[2],&bytes))==NULL) exit(1);

      if ((comment=(char *)realloc(comment,bytes+1))==NULL) exit(1);
      comment[bytes]='\0';

      writePNMimage(argv[1],image,width,height,components,comment);
      }

   free(comment);
   free(image);

   return(0);
   }
