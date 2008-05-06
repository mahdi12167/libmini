#include "minibase.h"

// add include files here:
// ...

#include "minimpfp.h"
#include "minitime.h"

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

   typedef minimpfp4 minifp;

   double time;

   unsigned int i,j;

   minifp test1,test2,test3;

   printf("bits=%u\n",minifp::getbits());
   printf("limit=%g\n",minifp::getlimit());

   time=minigettime();

   const unsigned int addmax1=100000;
   const unsigned int addmax2=10;

   for (i=0; i<addmax1; i++)
      for (j=0; j<addmax2; j++)
         {
         test1=minifp(i);
         test2=minifp(j);

         test1.add(test2,test3);

         if (test3.get()!=(double)i+(double)j)
            printf("%g+%g!=%g\n",(double)i,(double)j,test3.get());
         }

   printf("%gkadds/s\n",(double)addmax1*addmax2/(minigettime()-time)/1000);

   time=minigettime();

   const unsigned int mulmax1=10000;
   const unsigned int mulmax2=100;

   for (i=0; i<mulmax1; i++)
      for (j=0; j<mulmax2; j++)
         {
         test1=minifp(i);
         test2=minifp(j);

         test1.mul(test2,test3);

         if (test3.get()!=(double)i*(double)j)
            printf("%g*%g!=%g\n",(double)i,(double)j,test3.get());
         }

   printf("%gkmuls/s\n",(double)mulmax1*mulmax2/(minigettime()-time)/1000);

   printf("\nfinished test code\n");

   return(0);
   }
