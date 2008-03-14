#include "minibase.h"

#include "iostream"

#include "minigeom.h"

// add include files here:
// ...

int main(int argc,char *argv[])
   {
   if (argc!=1)
      {
      printf("usage: %s\n",argv[0]);
      exit(1);
      }

   printf("running test code\n\n");

   // begin of test code section

   const int hsnum=6;

   minigeom_halfspace hs[hsnum];

   hs[0]=minigeom_halfspace(miniv3d(-1,0,0),miniv3d(1,0,0));
   hs[1]=minigeom_halfspace(miniv3d(1,0,0),miniv3d(-1,0,0));
   hs[2]=minigeom_halfspace(miniv3d(0,-1,0),miniv3d(0,1,0));
   hs[3]=minigeom_halfspace(miniv3d(0,1,0),miniv3d(0,-1,0));
   hs[4]=minigeom_halfspace(miniv3d(0,0,-1),miniv3d(0,0,1));
   hs[5]=minigeom_halfspace(miniv3d(0,0,1),miniv3d(0,0,-1));

   minigeom_polyhedron cp;

   int i,j;

   for (i=0; i<hsnum; i++)
      {
      cp.intersect(hs[i]);

      for (j=0; j<cp.getnumhalfspace(); j++)
         std::cout << cp.gethalfspace(j).getpoint() << " " << cp.gethalfspace(j).getvector() << std::endl;

      if (i<hsnum-1) std::cout << std::endl;
      }

   // add test code here:
   // ...

   // end of test code section

   printf("\nfinished test code\n");

   return(0);
   }
