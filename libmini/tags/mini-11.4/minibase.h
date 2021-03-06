// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIBASE_H
#define MINIBASE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <time.h> // for seed

#include "minidefs.h"

namespace minibase {

enum MINI_ERROR
   {
   MINI_ERROR_NONFATAL=0,
   MINI_ERROR_FATAL=1,
   MINI_ERROR_MEM=2,
   MINI_ERROR_IO=3,
   MINI_ERROR_CODE=4
   };

extern void (*minierrorhandler)(const char *file,int line,int fatal,const char *msg);

#define ERRORMSG() minierrormsg(__FILE__,__LINE__,MINI_ERROR_FATAL)
#define MEMERROR() minierrormsg(__FILE__,__LINE__,MINI_ERROR_MEM)
#define IOERROR() minierrormsg(__FILE__,__LINE__,MINI_ERROR_IO)
#define CODEERROR() minierrormsg(__FILE__,__LINE__,MINI_ERROR_CODE)
#define WARNMSG(msg) minierrormsg(__FILE__,__LINE__,MINI_ERROR_NONFATAL,msg)

#ifdef LIBMINI_DEBUG
#   define ERRORCHK(cond) {if (cond) minierrormsg(__FILE__,__LINE__,MINI_ERROR_FATAL);}
#else
#   define ERRORCHK(cond) {/*empty*/}
#endif

inline void minierrormsg(const char *file,int line,int fatal,const char *msg=NULL)
   {
   if (minierrorhandler==0)
      {
      if (fatal==MINI_ERROR_NONFATAL) fprintf(stderr,"warning");
      else if (fatal==MINI_ERROR_MEM) fprintf(stderr,"insufficient memory");
      else if (fatal==MINI_ERROR_IO) fprintf(stderr,"io error");
      else if (fatal==MINI_ERROR_CODE) fprintf(stderr,"unimplemented code");
      else fprintf(stderr,"fatal error");
      fprintf(stderr," in <%s> at line %d!\n",file,line);
      if (msg!=NULL) fprintf(stderr,"description: %s\n",msg);
      }
   else minierrorhandler(file,line,fatal,msg);
   if (fatal!=MINI_ERROR_NONFATAL) exit(1);
   }

#define mini_stringify(s) #s

#ifdef __APPLE__
#   define ffloor(x) floorf(x)
#   define fceil(x) ceilf(x)
#else
#   ifndef __sgi
#      define ffloor(x) (float)floor(x)
#      define fceil(x) (float)ceil(x)
#   endif
#endif

#define ftrc(x) (int)ffloor(x)
#define dtrc(x) (int)floor(x)

inline float FABS(const float x) {return((x<0.0)?-x:x);}
inline double dabs(const double x) {return((x<0.0)?-x:x);}
#define fabs(x) FABS(x)

#ifndef min
inline int min(const int a,const int b) {return((a<b)?a:b);}
#endif
inline float FMIN(const float a,const float b) {return((a<b)?a:b);}
inline double dmin(const double a,const double b) {return((a<b)?a:b);}
#define fmin(a,b) FMIN(a,b)

#ifndef max
inline int max(const int a,const int b) {return((a>b)?a:b);}
#endif
inline float FMAX(const float a,const float b) {return((a>b)?a:b);}
inline double dmax(const double a,const double b) {return((a>b)?a:b);}
#define fmax(a,b) FMAX(a,b)

inline int sqr(const int x) {return(x*x);}
inline float fsqr(const float x) {return(x*x);}
inline double dsqr(const double x) {return(x*x);}

#ifdef __APPLE__
#   define fsqrt(x) sqrtf(x)
#   define fsin(x) sinf(x)
#   define fcos(x) cosf(x)
#   define ftan(x) tanf(x)
#   define fasin(x) asinf(x)
#   define facos(x) acosf(x)
#   define fatan(x) atanf(x)
#   define fatan2(y,x) atan2f(y,x)
#   define fexp(x) expf(x)
#   define flog(x) logf(x)
#   define fpow(x,y) powf(x,y)
#else
#   ifndef __sgi
#      define fsqrt(x) (float)sqrt(x)
#      define fsin(x) (float)sin(x)
#      define fcos(x) (float)cos(x)
#      define ftan(x) (float)tan(x)
#      define fasin(x) (float)asin(x)
#      define facos(x) (float)acos(x)
#      define fatan(x) (float)atan(x)
#      define fatan2(y,x) (float)atan2(y,x)
#      define fexp(x) (float)exp(x)
#      define flog(x) (float)log((float)(x))
#      define fpow(x,y) (float)pow((float)(x),(float)(y))
#   endif
#endif

inline float fround2(const float v,const unsigned int n=6) {return(ffloor(v*fpow(10.0f,(int)n)+0.5f)/fpow(10.0f,(int)n));}
inline double round2(const double v,const unsigned int n=6) {return(floor(v*pow(10.0,(int)n)+0.5)/pow(10.0,(int)n));}

inline int isNAN(const double v) {return(v!=v);}

#ifdef _WIN32
#define drand48() ((double)rand()/RAND_MAX)
#define srand48(s) srand(s)
#endif

#define minirand() drand48()
#define miniseed() srand48((unsigned int)time(NULL))

#ifdef _MSC_VER
#define strdup _strdup
#define snprintf _snprintf
#endif

inline char *strdup2(const char *str1,const char *str2)
   {
   char *str;

   if (str1==NULL && str2==NULL) return(NULL);

   if (str1==NULL) return(strdup(str2));
   if (str2==NULL) return(strdup(str1));

   if ((str=(char *)malloc(strlen(str1)+strlen(str2)+1))==NULL) MEMERROR();

   memcpy(str,str1,strlen(str1));
   memcpy(str+strlen(str1),str2,strlen(str2)+1);

   return(str);
   }

#ifdef _MSC_VER
inline int strcasecmp(const char *str1,const char *str2)
   {
   const char *ptr1,*ptr2;
   for (ptr1=str1,ptr2=str2; tolower(*ptr1)==tolower(*ptr2) && *ptr1!='\0' && *ptr2!='\0'; ptr1++,ptr2++);
   return(*ptr1-*ptr2);
   }
#endif

#if defined(_MSC_VER) || defined(__MINGW32__)
inline char *strcasestr(const char *str1,const char *str2)
   {
   unsigned int i,j;

   unsigned int len1,len2;

   len1=strlen(str1);
   len2=strlen(str2);

   for (i=0; i+len2<=len1; i++)
      {
      for (j=0; j<len2; j++)
         if (tolower(str2[j])!=tolower(str1[i+j])) break;

      if (j==len2) return((char *)&str1[i]);
      }

   return(NULL);
   }
#endif

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

#ifdef _MSC_VER
#include <direct.h>
#define mkdir(dir,perm) _mkdir(dir)
#endif

}

using namespace minibase;

#endif
