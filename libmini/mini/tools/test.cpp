#include "minibase.h"

// add include files here:
// ...

#include "minimath.h"

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

   minifixed4 test1(1),test2(2),test3;
   test1.add(test2,test3);
   printf("1+2=%g\n",test3.get());

   printf("\nfinished test code\n");

   return(0);
   }
