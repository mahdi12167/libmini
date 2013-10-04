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

   printf("loading geometry\n");

   if (!strip.load(argv[1]))
      {
      printf("load failure\n");
      exit(1);
      }

   printf("saving geometry\n");

   strip.writePLYfile(argv[2]);

   return(0);
   }
