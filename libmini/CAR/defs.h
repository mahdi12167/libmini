#ifndef DEFS_H
#define DEFS_H

#define ERROR()\
   {\
   fprintf(stderr,"fatal error in %s at line %d!\n",__FILE__,__LINE__);\
   exit(EXIT_FAILURE);\
   }

#define min(a,b) (((a)<(b))?(a):(b))
#define fmin(a,b) (((a)<(b))?(a):(b))

#define max(a,b) (((a)>(b))?(a):(b))
#define fmax(a,b) (((a)>(b))?(a):(b))

#define ftrunc(x) (trunc(x))
#define ftrc(x) ((int)trunc(x))

#define fsqr(x) ((x)*(x))
#define fsqrt(x) sqrt(x)
#define fpow(x) pow(x)

#define fsin(x) sin(x)
#define fcos(x) cos(x)
#define ftan(x) tan(x)

#define repeat      do {
#define until(expr) } while (!(expr))

#endif
