// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minibase.h"

#include <time.h>
#ifndef _WIN32
#include <sys/time.h>
#else
#ifndef APIENTRY
#include <windows.h>
#endif
#endif

namespace minitime {

double gettime()
   {
#ifndef _WIN32
   struct timeval t;
   gettimeofday(&t,NULL);
   return(t.tv_sec+t.tv_usec/1.0E6);
#else
   LARGE_INTEGER freq,count;
   if (QueryPerformanceFrequency(&freq)==0) ERRORMSG();
   QueryPerformanceCounter(&count);
   return((double)count.QuadPart/freq.QuadPart);
#endif
   }

void waitfor(double secs)
   {
   if (secs<=0.0) return;
#ifndef _WIN32
   struct timespec dt,rt;
   dt.tv_sec=(int)floor(secs);
   dt.tv_nsec=(int)floor(1.0E9*(secs-floor(secs)));
   while (nanosleep(&dt,&rt)!=0) dt=rt;
#else
   double time=gettime()+secs;
   while (gettime()<time) Sleep(1);
#endif
   }

int is_leapyear(int year)
   {return(((year%4==0) && (year%100!=0)) || (year%400==0));}

unsigned int daysofmonth(unsigned int month,int year)
   {
   switch (month)
      {
      case 1: case 3: case 5: case 7: case 8: case 10: case 12: return(31);
      case 2: return(is_leapyear(year)?29:28);
      case 4: case 6: case 9: case 11: return(30); break;
      default: ERRORMSG();
      }

   return(0);
   }

unsigned int daysofyear(int year)
   {return(is_leapyear(year)?366:365);}

double utc2unixtime(int year,unsigned int month,unsigned int day,
                    unsigned int hour,unsigned int minute,unsigned int second,
                    double milliseconds)
   {
   int i;

   double s;

   s=milliseconds/1000;

   s+=second;
   s+=minute*60;
   s+=hour*60*60;
   s+=(day-1)*24*60*60;

   for (i=month; i>1; i--)
      s+=daysofmonth(i-1,year)*24*60*60;

   if (year>=1970)
      for (i=year; i>1970; i--)
         s+=daysofyear(year-1)*24*60*60;
   else
      for (i=year; i<1970; i++)
         s-=daysofyear(year)*24*60*60;

   return(s);
   }

double utc2minitime(int year,unsigned int month,unsigned int day,
                    unsigned int hour,unsigned int minute,unsigned int second,
                    double milliseconds)
   {
   return(utc2unixtime(year,month,day,
                       hour,minute,second,
                       milliseconds)-47313240);
   }

}
