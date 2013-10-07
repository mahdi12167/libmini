// (c) by Stefan Roettger, licensed under LGPL 2.1

#include <mini/minibase.h>

#include <mini/ministrip.h>

int main(int argc,char *argv[])
   {
   ministrip strip;

   if (argc!=3)
      {
      printf("usage: %s <geometry.geo> <geometry.ply>\n",argv[0]);
      printf(" load geometry data (libmini geometry format)\n");
      printf(" and convert it into a ply file (polygon file format)\n");
      exit(1);
      }

   if (!strip.convertGEO2PLYfile(argv[1],argv[2]))
      {
      printf("conversion failure\n");
      exit(1);
      }

   return(0);
   }
