// (c) by Stefan Roettger

#ifndef MINIBASE_H
#define MINIBASE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <float.h>
#include <math.h>

namespace minibase {

extern void (*minierrorhandler)(const char *file,int line,int fatal);

#define WARNMSG() minierrormsg(__FILE__,__LINE__)
#define ERRORMSG() minierrormsg(__FILE__,__LINE__,1)

inline void minierrormsg(const char *file,int line,int fatal=0)
   {
   if (minierrorhandler==0)
      {
      if (fatal==0) fprintf(stderr,"warning");
      else fprintf(stderr,"fatal error");
      fprintf(stderr," in <%s> at line %d!\n",file,line);
      }
   else minierrorhandler(file,line,fatal);
   if (fatal!=0) exit(EXIT_FAILURE);
   }

#ifndef NULL
#   define NULL (0)
#endif

#define BOOLINT char

#ifndef TRUE
#   define TRUE (1)
#endif
#ifndef FALSE
#   define FALSE (0)
#endif

#ifndef PI
#   define PI (3.14159265358979323846)
#endif
#ifndef RAD
#   define RAD (PI/180.0)
#endif

#ifndef MAXFLOAT
#   define MAXFLOAT (FLT_MAX)
#endif

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
#define FTRC(x) (int)floor(x)

inline float fABS(const float x) {return((x<0.0)?-x:x);}
inline double FABS(const double x) {return((x<0.0)?-x:x);}
#define fabs(x) fABS(x)

#ifndef min
inline int min(const int a,const int b) {return((a<b)?a:b);}
#endif
inline float fMIN(const float a,const float b) {return((a<b)?a:b);}
inline double FMIN(const double a,const double b) {return((a<b)?a:b);}
#define fmin(a,b) fMIN(a,b)

#ifndef max
inline int max(const int a,const int b) {return((a>b)?a:b);}
#endif
inline float fMAX(const float a,const float b) {return((a>b)?a:b);}
inline double FMAX(const double a,const double b) {return((a>b)?a:b);}
#define fmax(a,b) fMAX(a,b)

inline int sqr(const int x) {return(x*x);}
inline float fsqr(const float x) {return(x*x);}
inline double FSQR(const double x) {return(x*x);}

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

#ifdef _MSC_VER
#define strdup _strdup
#define snprintf _snprintf
#endif

inline char *strcct(const char *str1,const char *str2)
   {
   char *str;

   if (str1==NULL && str2==NULL) return(NULL);

   if (str1==NULL) return(strdup(str2));
   if (str2==NULL) return(strdup(str1));

   if ((str=(char *)malloc(strlen(str1)+strlen(str2)+1))==NULL) ERRORMSG();

   memcpy(str,str1,strlen(str1));
   memcpy(str+strlen(str1),str2,strlen(str2)+1);

   return(str);
   }

}

using namespace minibase;

#endif
