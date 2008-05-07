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

   double time1,time2,time3;

   long long int i,j;

   minifp test1,test2,test3;

   double test;

   // constants:

   printf("bits=%u\n",minifp::getbits());
   printf("limit=%g\n",minifp::getlimit());

   // add speed test:

   time1=minigettime();

   const long long int addmax1=100000;
   const long long int addmax2=10;

   for (i=-addmax1-1; i<addmax1; i++)
      for (j=-addmax2-1; j<addmax2; j++)
         {
         test1=minifp(i);
         test2=minifp(j);

         test1.add(test2,test3);

         if (test3.get()!=(double)i+(double)j)
            printf("%g+%g!=%g\n",(double)i,(double)j,test3.get());
         }

   time1=minigettime()-time1;

   // add idle test:

   test1=minifp(1);
   test2=minifp(-2);

   test1.add(test2,test3);

   time2=minigettime();

   for (i=-addmax1-1; i<addmax1; i++)
      for (j=-addmax2-1; j<addmax2; j++)
         if (test3.get()!=-1)
            printf("%g+%g!=%g\n",(double)1,(double)-2,test3.get());

   time2=minigettime()-time2;

   printf("%gkadds/s\n",(double)4*addmax1*addmax2/(time1-time2)/1E3);

   // add double test:

   time3=minigettime();

   for (i=-addmax1-1; i<addmax1; i++)
      for (j=-addmax2-1; j<addmax2; j++)
         {
         test=(double)i+(double)j;
         if (test==MAXFLOAT) ERRORMSG();
         }

   time3=minigettime()-time3;

   printf("(%gkdadds/s)\n",(double)4*addmax1*addmax2/time3/1E3);

   // mul speed test:

   time1=minigettime();

   const long long int mulmax1=10000;
   const long long int mulmax2=100;

   for (i=-mulmax1-1; i<mulmax1; i++)
      for (j=-mulmax2-1; j<mulmax2; j++)
         {
         test1=minifp(i);
         test2=minifp(j);

         test1.mul(test2,test3);

         if (test3.get()!=(double)i*(double)j)
            printf("%g*%g!=%g\n",(double)i,(double)j,test3.get());
         }

   time1=minigettime()-time1;

   // mul idle test:

   test1=minifp(1);
   test2=minifp(-2);

   test1.mul(test2,test3);

   time2=minigettime();

   for (i=-mulmax1-1; i<mulmax1; i++)
      for (j=-mulmax2-1; j<mulmax2; j++)
         if (test3.get()!=-2)
            printf("%g*%g!=%g\n",(double)-1,(double)-2,test3.get());

   time2=minigettime()-time2;

   printf("%gkmuls/s\n",(double)4*mulmax1*mulmax2/(time1-time2)/1E3);

   // mul double test:

   time3=minigettime();

   for (i=-mulmax1-1; i<mulmax1; i++)
      for (j=-mulmax2-1; j<mulmax2; j++)
         {
         test=(double)i*(double)j;
         if (test==MAXFLOAT) ERRORMSG();
         }

   time3=minigettime()-time3;

   printf("(%gkdmuls/s)\n",(double)4*mulmax1*mulmax2/time3/1E3);

   printf("\nfinished test code\n");

   return(0);
   }
