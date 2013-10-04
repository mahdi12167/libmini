// (c) by Stefan Roettger, licensed under LGPL 2.1

#include <mini/minibase.h>

#include <mini/rawbase.h>
#include <mini/rekbase.h>

int main(int argc,char *argv[])
   {
   double isovalue=0.5;

   char *outname;

   if (argc!=3 && argc!=4)
      {
      printf("usage: %s <volume.raw> <iso.txt> [<iso value>]\n",argv[0]);
      printf(" load volume data (raw or rek format) and\n");
      printf(" convert into an iso surface (internal ministrip text format)\n");
      exit(1);
      }

   if (argc==4)
      if (sscanf(argv[3],"%lg",&isovalue)!=1) exit(1);

   outname=extractRAWvolume(argv[1],argv[2],isovalue);
   if (outname==NULL) outname=extractREKvolume(argv[1],argv[2],isovalue);

   if (outname==NULL) exit(1);
   else free(outname);

   return(0);
   }
