// (c) by Stefan Roettger, licensed under LGPL 2.1

#include <mini/minibase.h>

#include <mini/minimpfp.h>
#include <mini/minitime.h>

int main(int argc,char *argv[])
   {
   if (argc!=1)
      {
      printf("usage: %s\n",argv[0]);
      exit(1);
      }

   double time;

   long long int i,j;

   minifixp test1,test2,test3;
   double error1,error2,error3,error4,error5;

   // report constants:

   printf("bits=%u\n",minifixp::getbits());
   printf("limit=%g\n",minifixp::getlimit());

   // add speed test:

   time=gettime();

   const long long int addmax1=100;
   const long long int addmax2=100;

   for (i=-addmax1-1; i<addmax1; i++)
      for (j=-addmax2-1; j<addmax2; j++)
         {
         test1=minifixp(0.1*i);
         test2=minifixp(0.1*j);

         test1.add(test2,test3);

         error1=dabs(test3.get()-0.1*i-0.1*j);

         if (error1>1E-3) printf("%g+%g!=%g\n",0.1*i,0.1*j,test3.get());
         }

   time=gettime()-time;

   printf("%gk adds/s\n",(double)4*addmax1*addmax2/time/1E3);

   // mul speed test:

   time=gettime();

   const long long int mulmax1=100;
   const long long int mulmax2=100;

   for (i=-mulmax1-1; i<mulmax1; i++)
      for (j=-mulmax2-1; j<mulmax2; j++)
         {
         test1=minifixp(0.1*i);
         test2=minifixp(0.1*j);

         test1.mul(test2,test3);

         error2=dabs(test3.get()-0.1*i*0.1*j);

         if (error2>1E-3) printf("%g*%g!=%g\n",0.1*i,0.1*j,test3.get());
         }

   time=gettime()-time;

   printf("%gk muls/s\n",(double)4*mulmax1*mulmax2/time/1E3);

   // div speed test:

   time=gettime();

   const long long int divmax1=100;
   const long long int divmax2=100;

   for (i=-divmax1-1; i<divmax1; i++)
      for (j=-divmax2-1; j<divmax2+1; j++)
         if (j!=0)
            {
            test1=minifixp(0.1*i);
            test2=minifixp(0.1*j);

            test1.div(test2,test3);

            error3=dabs(test3.get()-(double)i/j);

            if (error3>1E-3) printf("%g/%g!=%g\n",0.1*i,0.1*j,test3.get());
            }

   time=gettime()-time;

   printf("%gk divs/s\n",(double)4*divmax1*divmax2/time/1E3);

   // sqrt speed test:

   time=gettime();

   const long long int sqrtmax=10000;

   for (i=0; i<sqrtmax; i++)
      {
      test1=minifixp(0.1*i);

      test2=test1.sqroot();

      error4=dabs(test2.get()-sqrt(0.1*i));

      if (error4>1E-3) printf("sqrt(%g)!=%g\n",0.1*i,test2.get());
      }

   time=gettime()-time;

   printf("%gk sqrts/s\n",(double)sqrtmax/time/1E3);

   // 1/sqrt speed test:

   time=gettime();

   const long long int invsqrtmax=10000;

   for (i=1; i<invsqrtmax+1; i++)
      {
      test1=minifixp(0.1*i);

      test2=test1.invsqroot();

      error5=dabs(test2.get()-1.0/sqrt(0.1*i));

      if (error5>1E-3) printf("1/sqrt(%g)!=%g\n",0.1*i,test2.get());
      }

   time=gettime()-time;

   printf("%gk invsqrts/s\n",(double)invsqrtmax/time/1E3);

   // precision test:

   minifixp c(271.3);

   minifixp x=c*c*c;
   minifixp y=(x-minifixp(1))*(x+minifixp(1));
   minifixp z=x*x-y;

   z=(z-minifixp(1)).abs();

   printf("precision test #1: %g(%d)\n",z.get(),z.getmsbit());

   minifixp inv=minifixp(1)/c;
   minifixp mlt=inv*c;

   mlt=(mlt-minifixp(1)).abs();

   printf("precision test #2: %g(%d)\n",mlt.get(),mlt.getmsbit());

   minifixp rx=minifixp(7);
   minifixp ry=minifixp(2);
   minifixp rz=minifixp(2002);
   minifixp r2=rx*rx+ry*ry+rz*rz;
   minifixp r=r2.sqroot();
   minifixp rxr=rx/r;
   minifixp ryr=ry/r;
   minifixp rzr=rz/r;
   minifixp len=rxr*rxr+ryr*ryr+rzr*rzr;

   len=(len-minifixp(1)).abs();

   printf("precision test #3: %g(%d)\n",len.get(),len.getmsbit());

   // validity test:

   printf("validity test: %d\n",minifixp::isvalid());

   return(0);
   }
