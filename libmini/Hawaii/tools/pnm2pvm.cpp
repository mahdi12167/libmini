#include "minibase.h"

#include "pnmbase.h"

int main(int argc,char *argv[])
   {
   unsigned char *image,*volume;

   int iwidth,iheight,vwidth,vheight,vdepth,
       icomponents,vcomponents;

   if (argc!=3)
      {
      printf("usage: %s <input.pnm> <output.pvm>\n",argv[0]);
      exit(1);
      }

   if ((volume=readPVMvolume(argv[2],&vwidth,&vheight,&vdepth,&vcomponents))==NULL) vdepth=0;
   else if (vwidth<1 || vheight<1 || vdepth<1 || (vcomponents!=1 && vcomponents!=3)) exit(1);

   if ((image=readPNMfile(argv[1],&iwidth,&iheight,&icomponents))==NULL) exit(1);

   if (vdepth==0)
      {
      if (iwidth<1 || iheight<1 || (icomponents!=1 && icomponents!=3)) exit(1);

      vwidth=iwidth;
      vheight=iheight;
      vcomponents=icomponents;
      }
   else
      if (iwidth!=vwidth || iheight!=vheight || icomponents!=vcomponents) exit(1);

   if (vdepth==0)
      {
      vdepth=1;
      if ((volume=(unsigned char *)malloc(vwidth*vheight*vdepth*vcomponents))==NULL) exit(1);
      }
   else
      {
      vdepth++;
      if ((volume=(unsigned char *)realloc(volume,vwidth*vheight*vdepth*vcomponents))==NULL) exit(1);
      }

   memcpy(&volume[vwidth*vheight*(vdepth-1)*vcomponents],image,iwidth*iheight*icomponents);
   free(image);

   writePVMvolume(argv[2],volume,vwidth,vheight,vdepth,vcomponents);
   free(volume);

   return(0);
   }
