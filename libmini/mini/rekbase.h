// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef REKBASE_H
#define REKBASE_H

#include "minidefs.h"

extern float REK_TARGET_RATIO;
extern long long REK_TARGET_CELLS;

// read a REK volume (Fraunhofer EZRT volume format)
//  the REK format has a 2048 byte header
//  the header contains short int values in LSB format
//   short int #1 of header: x size
//   short int #2 of header: y size
//   short int #3 of header: bits (usually 8 or 16)
//   short int #4 of header: z size
//  after the header the raw volume data is attached
//   the values of a 16-bit volume are saved in LSB order
unsigned char *readREKvolume(const char *filename,
                             long long *width,long long *height,long long *depth,unsigned int *components=NULL,
                             float *scalex=NULL,float *scaley=NULL,float *scalez=NULL);

// read REK file format header
BOOLINT readREKheader(const char *filename,
                      long long *width,long long *height,long long *depth,unsigned int *components=NULL,
                      float *scalex=NULL,float *scaley=NULL,float *scalez=NULL);

// copy a REK volume to a RAW volume
char *copyREKvolume(const char *filename,const char *output);

// copy a REK volume to a RAW volume with out-of-core cropping and non-linear quantization
char *processREKvolume(const char *filename,
                       const char *infix=NULL,
                       float ratio=REK_TARGET_RATIO, // crop volume ratio
                       long long maxcells=REK_TARGET_CELLS, // down-size threshold
                       void (*feedback)(const char *info,float percent,void *obj)=NULL,void *obj=NULL);

// read a REK volume out-of-core
unsigned char *readREKvolume_ooc(const char *filename,
                                 long long *width,long long *height,long long *depth,unsigned int *components=NULL,
                                 float *scalex=NULL,float *scaley=NULL,float *scalez=NULL,
                                 float ratio=REK_TARGET_RATIO, // crop volume ratio
                                 long long maxcells=REK_TARGET_CELLS, // down-size threshold
                                 void (*feedback)(const char *info,float percent,void *obj)=NULL,void *obj=NULL);

// extract an iso-surface from a REK volume out-of-core
char *extractREKvolume(const char *filename,
                       const char *output,
                       double isovalue=0.5, // iso value to be extracted as surface
                       void (*feedback)(const char *info,float percent,void *obj)=NULL,void *obj=NULL);

#endif
