#include "minibase.h"

// add include files here:
// ...

#include "minimesh.h"

int main(int argc,char *argv[])
   {
   if (argc!=1)
      {
      printf("usage: %s\n",argv[0]);
      exit(1);
      }

   printf("running test code\n\n");

   // add test code here:
   // ...

   minihedron h1=minihedron(miniv3d(0,0,0),miniv3d(1,0,0),miniv3d(0,1,0),miniv3d(0,0,1),minival(1,miniv3d(0,0,0),miniv3d(1,0,0),miniv3d(0,1,0),miniv3d(0,0,1)));
   minihedron h2=minihedron(miniv3d(-1,0,0),miniv3d(1,-1,1),miniv3d(1,1,1),miniv3d(0,0,2),minival(2,miniv3d(-1,0,0),miniv3d(1,-1,1),miniv3d(1,1,1),miniv3d(0,0,2)));

   minimesh mesh;

   mesh.append(h1);
   mesh.append(h2);

   minibsptree bspt;

   bspt.insert(mesh);

   printf("\nfinished test code\n");

   return(0);
   }
