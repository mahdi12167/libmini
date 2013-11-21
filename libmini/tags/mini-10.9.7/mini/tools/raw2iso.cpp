// (c) by Stefan Roettger, licensed under LGPL 2.1

#include <mini/minibase.h>

#include <mini/rawbase.h>
#include <mini/rekbase.h>

void feedback(const char *info,float percent,void *obj)
   {
   static float last=0.0f;

   if (fabs(percent-last)>=0.01f)
      {
      printf("%s: %d%%\n",info,(int)(100.0f*percent+0.5f));
      last=percent;
      }
   }

int main(int argc,char *argv[])
   {
   double isovalue=0.5;

   char *outname;

   if (argc!=3 && argc!=4)
      {
      printf("usage: %s <volume.raw> <iso.geo> [<iso value>]\n",argv[0]);
      printf(" load volume data (raw and rek volume format)\n");
      printf(" and convert it into an iso surface (libmini geometry format)\n");
      exit(1);
      }

   if (argc==4)
      if (sscanf(argv[3],"%lg",&isovalue)!=1) exit(1);

   outname=extractRAWvolume(argv[1],argv[2],isovalue,feedback);
   if (outname==NULL) outname=extractREKvolume(argv[1],argv[2],isovalue,feedback);

   if (outname==NULL) exit(1);
   else free(outname);

   return(0);
   }
