// (c) by Stefan Roettger, licensed under LGPL 2.1

#include <mini/minibase.h>

#include <mini/miniio.h>
#include <mini/database.h>

int main(int argc,char *argv[])
   {
   unsigned char *data;
   unsigned int bytes;

   unsigned int width,height,depth;
   int components;

   int msb=1;
   int ushort=0;

   databuf buf;
   int type;

   if (argc!=6 && argc!=7)
      {
      printf("usage: %s <data.raw> width height [depth] components <data.db>\n",argv[0]);
      printf(" convert raw data into db format\n");
      printf("  | components | 1    | 2         | -2        | 65535      | -65535     | 3   | 4    |\n");
      printf("  | meaning    | byte | short-msb | short-lsb | ushort-msb | ushort-lsb | rgb | rgba |\n");
      exit(1);
      }

   if (sscanf(argv[2],"%u",&width)!=1) exit(1);
   if (sscanf(argv[3],"%u",&height)!=1) exit(1);
   if (argc==7)
      {
      if (sscanf(argv[4],"%u",&depth)!=1) exit(1);
      if (sscanf(argv[5],"%d",&components)!=1) exit(1);
      }
   else
      {
      depth=1;
      if (sscanf(argv[4],"%d",&components)!=1) exit(1);
      }

   if (width<1 || height<1 || depth<1) exit(1);
   if ((components<1 || components>4) && components!=-2 && components!=65535 && components!=-65535) exit(1);

   if (components==65535 || components==-65535)
      {
      ushort=1;
      if (components==65535) components=2;
      else components=-2;
      }

   if (components==-2)
      {
      components=2;
      msb=0;
      }

   if ((data=readfile(argv[1],&bytes))==NULL) exit(1);

   if (components==1) type=databuf::DATABUF_TYPE_BYTE;
   else if (components==2) type=databuf::DATABUF_TYPE_SHORT;
   else if (components==3) type=databuf::DATABUF_TYPE_RGB;
   else if (components==4) type=databuf::DATABUF_TYPE_RGBA;
   else exit(1);

   if (bytes<width*height*depth*components) exit(1);
   buf.copy(data,bytes,width,height,depth,1,type);
   buf.swap2(msb);
   buf.convert2(ushort);
   free(data);

   buf.savedata((argc==7)?argv[6]:argv[5]);
   buf.release();

   return(0);
   }
