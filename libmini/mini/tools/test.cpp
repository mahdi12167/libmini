#include "minibase.h"

// add include files here:
// ...

#include "minimpfp.h"
#include "minitime.h"

// end of include files

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

   double time;

   long long int i,j;

   minimf test1,test2,test3;

   // constants:

   printf("bits=%u\n",minimf::getbits());
   printf("limit=%g\n",minimf::getlimit());

   // add speed test:

   time=minigettime();

   const long long int addmax1=100;
   const long long int addmax2=100;

   for (i=-addmax1-1; i<addmax1; i++)
      for (j=-addmax2-1; j<addmax2; j++)
         {
         test1=minimf(0.1*i);
         test2=minimf(0.1*j);

         test1.add(test2,test3);

         if (FABS(test3.get()-0.1*i-0.1*j)>1E-6)
            printf("%g+%g!=%g\n",0.1*i,0.1*j,test3.get());
         }

   time=minigettime()-time;

   printf("%gk adds/s\n",(double)4*addmax1*addmax2/time/1E3);

   // mul speed test:

   time=minigettime();

   const long long int mulmax1=100;
   const long long int mulmax2=100;

   for (i=-mulmax1-1; i<mulmax1; i++)
      for (j=-mulmax2-1; j<mulmax2; j++)
         {
         test1=minimf(0.1*i);
         test2=minimf(0.1*j);

         test1.mul(test2,test3);

         if (FABS(test3.get()-0.1*i*0.1*j)>1E-6)
            printf("%g*%g!=%g\n",0.1*i,0.1*j,test3.get());
         }

   time=minigettime()-time;

   printf("%gk muls/s\n",(double)4*mulmax1*mulmax2/time/1E3);

   // div speed test:

   //!!
   double v=1.00009;
   minimf t(v),r;
   r=t.inv2(t);
   printf("test: %.18f -> %.18f %.18f\n",1.0/v,t.get(),r.get());
   exit(0);

   time=minigettime();

   const long long int divmax1=100;
   const long long int divmax2=100;

   for (i=-divmax1-1; i<divmax1; i++)
      for (j=-divmax2-1; j<divmax2+1; j++)
         if (j!=0)
            {
            test1=minimf(0.1*i);
            test2=minimf(0.1*j);

            test1.div(test2,test3);

            if (FABS(test3.get()-(double)i/j)>1E-6)
               printf("%g/%g!=%g\n",0.1*i,0.1*j,test3.get());
            }

   time=minigettime()-time;

   printf("%gk divs/s\n",(double)4*divmax1*divmax2/time/1E3);

   // sqrt speed test:

   time=minigettime();

   const long long int sqrtmax=10000;

   if (0) //!!
   for (i=0; i<sqrtmax; i++)
      {
      test1=minimf(0.1*i);

      test2=test1.sqroot();

      if (FABS(test2.get()-sqrt(0.1*i))>1E-6)
         printf("sqrt(%g)!=%g\n",0.1*i,test2.get());
      }

   time=minigettime()-time;

   printf("%gk sqrts/s\n",(double)sqrtmax/time/1E3);

   // inv sqrt speed test:

   time=minigettime();

   const long long int invsqrtmax=10000;

   if (0) //!!
   for (i=1; i<invsqrtmax+1; i++)
      {
      test1=minimf(0.1*i);

      test2=test1.invsqroot();

      if (FABS(test2.get()-1.0/sqrt(0.1*i))>1E-6)
         printf("invsqrt(%g)!=%g\n",0.1*i,test2.get());
      }

   time=minigettime()-time;

   printf("%gk invsqrts/s\n",(double)invsqrtmax/time/1E3);

   // precision test:

   minimf c(255.271);

   minimf x=c*c*c*c*c*c*c;
   minimf y=(x-1)*(x+1);
   minimf z=x*x-y;

   z.sub(minimf::one(),z);

   printf("precision test value #1: %g\n",z.get());

   minimf inv=minimf(1.0)/c;
   minimf mlt=inv*c;

   mlt.sub(minimf::one(),mlt);

   printf("precision test value #2: %g\n",mlt.get());

   // end of test code

   printf("\nfinished test code\n");

   return(0);
   }
