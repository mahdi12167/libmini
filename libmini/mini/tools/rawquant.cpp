#include <mini/minibase.h>

#include <mini/rawbase.h>

static const float ratio=0.5f;
static const unsigned long long maxcells=250;

int main(int argc,char *argv[])
   {
   unsigned long long cell_limit=maxcells;

   if (argc!=2 && argc!=3)
      {
      printf("usage: %s <input.raw> [<volume cell limit in mb>]\n",argv[0]);
      exit(1);
      }

   if (argc==3)
      if (sscanf(argv[2],"%llu",&cell_limit)!=1) exit(1);

   cell_limit*=1000000;

   char *output;

   output=processRAWvolume(argv[1],ratio,maxcells);

   if (output)
      {
      printf("wrote %s\n",output);
      free(output);
      }
   else exit(1);

   return(0);
   }
