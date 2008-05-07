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

   typedef minimpfp1 minifp;

   double time1,time2;

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
         test1=minifp(0.1*i);
         test2=minifp(0.1*j);

         test1.add(test2,test3);

         if (FABS(test3.get()-0.1*i-0.1*j)>1E-3)
            printf("%g+%g!=%g\n",0.1*i,0.1*j,test3.get());
         }

   time1=minigettime()-time1;

   printf("%gkadds/s\n",(double)4*addmax1*addmax2/time1/1E3);

   // add double test:

   time2=minigettime();

   for (i=-addmax1-1; i<addmax1; i++)
      for (j=-addmax2-1; j<addmax2; j++)
         {
         test=(double)i+(double)j;
         if (test==MAXFLOAT) ERRORMSG();
         }

   time2=minigettime()-time2;

   printf("(%gkdadds/s)\n",(double)4*addmax1*addmax2/time2/1E3);

   // mul speed test:

   time1=minigettime();

   const long long int mulmax1=10000;
   const long long int mulmax2=100;

   for (i=-mulmax1-1; i<mulmax1; i++)
      for (j=-mulmax2-1; j<mulmax2; j++)
         {
         test1=minifp(0.1*i);
         test2=minifp(0.1*j);

         test1.mul(test2,test3);

         if (FABS(test3.get()-0.1*i*0.1*j)>1E-3)
            ;//!!printf("%g*%g!=%g\n",0.1*i,0.1*j,test3.get());
         }

   time1=minigettime()-time1;

   printf("%gkmuls/s\n",(double)4*mulmax1*mulmax2/time1/1E3);

   // mul double test:

   time2=minigettime();

   for (i=-mulmax1-1; i<mulmax1; i++)
      for (j=-mulmax2-1; j<mulmax2; j++)
         {
         test=(double)i*(double)j;
         if (test==MAXFLOAT) ERRORMSG();
         }

   time2=minigettime()-time2;

   printf("(%gkdmuls/s)\n",(double)4*mulmax1*mulmax2/time2/1E3);

   printf("\nfinished test code\n");

   return(0);
   }
