// (c) by Stefan Roettger, licensed under LGPL 2.1

#include <mini/minibase.h>

#include <mini/minidds.h>
#include <mini/miniio.h>

int main(int argc,char *argv[])
   {
   unsigned char *data;
   unsigned int bytes;
   long long llbytes;

   if (argc!=2)
      {
      printf("usage: %s <volume.pvm>\n",argv[0]);
      exit(1);
      }

   if ((data=readDDSfile(argv[1],&bytes))==NULL)
      if ((data=readfile(argv[1],&llbytes))==NULL) exit(1);
      else writeDDSfile(argv[1],data,llbytes);
   else writefile(argv[1],data,bytes);

   return(0);
   }
