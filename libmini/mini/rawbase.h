// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef RAWBASE_H
#define RAWBASE_H

#include "minidefs.h"

#include "stdio.h"

extern float RAW_TARGET_RATIO;
extern long long RAW_TARGET_CELLS;

extern unsigned short int RAW_INTEL;

#define RAW_ISINTEL (*((unsigned char *)(&RAW_INTEL)+1)==0)

// read a RAW volume
//  the RAW file format is encoded into the filename
//   as in name.size_cellformat_cellsize.raw
//    e.g. name.256x256_u2m.raw
//    e.g. name.256x256x256_2_100x100x50.raw
//    e.g. name.256x256x256x100.raw
//   cell format modifiers:
//    1 = 8 bit
//    2 = 16 bit
//    3 = rgb
//    4 = rgba
//    6 = rgb 16 bit
//    8 = rgba 16 bit
//    f = 32 bit float
//    u = unsigned
//    s = signed
//    m = msb
//    l = lsb
//   default modifiers = u1m
unsigned char *readRAWvolume(const char *filename,
                             long long *width,long long *height,long long *depth,long long *steps,
                             unsigned int *components=NULL,unsigned int *bits=NULL,BOOLINT *sign=NULL,BOOLINT *msb=NULL,
                             float *scalex=NULL,float *scaley=NULL,float *scalez=NULL);

// analyze RAW file format
BOOLINT readRAWinfo(char *filename,
                    long long *width,long long *height,long long *depth,long long *steps,
                    unsigned int *components=NULL,unsigned int *bits=NULL,BOOLINT *sign=NULL,BOOLINT *msb=NULL,
                    float *scalex=NULL,float *scaley=NULL,float *scalez=NULL);

// define RAW file format
char *makeRAWinfo(long long width,long long height,long long depth=1,long long steps=1,
                  unsigned int components=1,unsigned int bits=8,BOOLINT sign=FALSE,BOOLINT msb=TRUE,
                  float scalex=1.0f,float scaley=1.0f,float scalez=1.0f);

// write a RAW volume
char *writeRAWvolume(const char *filename, // /wo suffix .raw
                     unsigned char *volume,
                     long long width,long long height,long long depth=1,long long steps=1,
                     unsigned int components=1,unsigned int bits=8,BOOLINT sign=FALSE,BOOLINT msb=TRUE,
                     float scalex=1.0f,float scaley=1.0f,float scalez=1.0f);

// copy a RAW volume
char *copyRAWvolume(FILE *file, // source file desc
                    const char *output, // destination file name /wo suffix .raw
                    long long width,long long height,long long depth=1,long long steps=1,
                    unsigned int components=1,unsigned int bits=8,BOOLINT sign=FALSE,BOOLINT msb=TRUE,
                    float scalex=1.0f,float scaley=1.0f,float scalez=1.0f);

// copy a RAW volume
char *copyRAWvolume(const char *filename, // source file
                    const char *output); // destination file name /wo suffix .raw

// copy a RAW volume with out-of-core linear quantization
char *copyRAWvolume_linear(FILE *file, // source file desc
                           const char *output, // destination file name /wo suffix .raw
                           long long width,long long height,long long depth=1,long long steps=1,
                           unsigned int components=1,unsigned int bits=8,BOOLINT sign=FALSE,BOOLINT msb=TRUE,
                           float scalex=1.0f,float scaley=1.0f,float scalez=1.0f,
                           void (*feedback)(const char *info,float percent,void *obj)=NULL,void *obj=NULL);

// copy a RAW volume with out-of-core linear quantization
char *copyRAWvolume_linear(const char *filename, // source file
                           const char *output, // destination file name /wo suffix .raw
                           void (*feedback)(const char *info,float percent,void *obj)=NULL,void *obj=NULL);

// copy a RAW volume with out-of-core non-linear quantization
char *copyRAWvolume_nonlinear(FILE *file, // source file desc
                              const char *output, // destination file name /wo suffix .raw
                              long long width,long long height,long long depth=1,long long steps=1,
                              unsigned int components=1,unsigned int bits=8,BOOLINT sign=FALSE,BOOLINT msb=TRUE,
                              float scalex=1.0f,float scaley=1.0f,float scalez=1.0f,
                              void (*feedback)(const char *info,float percent,void *obj)=NULL,void *obj=NULL);

// copy a RAW volume with out-of-core non-linear quantization
char *copyRAWvolume_nonlinear(const char *filename, // source file
                              const char *output, // destination file name /wo suffix .raw
                              void (*feedback)(const char *info,float percent,void *obj)=NULL,void *obj=NULL);

// copy a RAW volume with out-of-core cropping
char *cropRAWvolume(FILE *file, // source file desc
                    const char *output, // destination file name /wo suffix .raw
                    long long width,long long height,long long depth=1,long long steps=1,
                    unsigned int components=1,unsigned int bits=8,BOOLINT sign=FALSE,BOOLINT msb=TRUE,
                    float scalex=1.0f,float scaley=1.0f,float scalez=1.0f,
                    float ratio=RAW_TARGET_RATIO, // crop volume ratio
                    void (*feedback)(const char *info,float percent,void *obj)=NULL,void *obj=NULL);

// copy a RAW volume with out-of-core cropping
char *cropRAWvolume(const char *filename, // source file
                    const char *output, // destination file name /wo suffix .raw
                    float ratio=RAW_TARGET_RATIO, // crop volume ratio
                    void (*feedback)(const char *info,float percent,void *obj)=NULL,void *obj=NULL);

// copy a RAW volume with out-of-core down-sizing
char *downsizeRAWvolume(FILE *file, // source file desc
                        const char *output, // destination file name /wo suffix .raw
                        long long width,long long height,long long depth=1,long long steps=1,
                        unsigned int components=1,unsigned int bits=8,BOOLINT sign=FALSE,BOOLINT msb=TRUE,
                        float scalex=1.0f,float scaley=1.0f,float scalez=1.0f,
                        void (*feedback)(const char *info,float percent,void *obj)=NULL,void *obj=NULL);

// copy a RAW volume with out-of-core down-sizing
char *downsizeRAWvolume(const char *filename, // source file
                        const char *output, // destination file name /wo suffix .raw
                        void (*feedback)(const char *info,float percent,void *obj)=NULL,void *obj=NULL);

// process a RAW volume with out-of-core cropping and non-linear quantization
char *processRAWvolume(FILE *file, // source file desc
                       const char *output, // destination file name
                       long long width,long long height,long long depth=1,long long steps=1,
                       unsigned int components=1,unsigned int bits=8,BOOLINT sign=FALSE,BOOLINT msb=TRUE,
                       float scalex=1.0f,float scaley=1.0f,float scalez=1.0f,
                       const char *infix=NULL, // optional infix for destination
                       float ratio=RAW_TARGET_RATIO, // crop volume ratio
                       long long maxcells=RAW_TARGET_CELLS, // down-size threshold
                       void (*feedback)(const char *info,float percent,void *obj)=NULL,void *obj=NULL); // feedback callback

// process a RAW volume with out-of-core cropping and non-linear quantization
char *processRAWvolume(const char *filename, // source file
                       const char *infix=NULL, // optional infix for destination
                       float ratio=RAW_TARGET_RATIO, // crop volume ratio
                       long long maxcells=RAW_TARGET_CELLS, // down-size threshold
                       void (*feedback)(const char *info,float percent,void *obj)=NULL,void *obj=NULL); // feedback callback

// read a RAW volume out-of-core
unsigned char *readRAWvolume_ooc(const char *filename,
                                 long long *width,long long *height,long long *depth,unsigned int *components,
                                 float *scalex,float *scaley,float *scalez,
                                 float ratio, // crop volume ratio
                                 long long maxcells, // down-size threshold
                                 void (*feedback)(const char *info,float percent,void *obj),void *obj);

// swap the hi and lo byte of 16 bit data
void swapRAWbytes(unsigned char *data,long long bytes);

// convert from signed short to unsigned short
void convRAWbytes(unsigned char *data,long long bytes);

// convert from float to unsigned short
void convRAWfloat(unsigned char *data,long long bytes);

// quantize 16 bit data to 8 bit using a non-linear mapping
unsigned char *quantizeRAW(unsigned char *volume,
                           long long width,long long height,long long depth,
                           BOOLINT msb=TRUE,
                           BOOLINT linear=FALSE,BOOLINT nofree=FALSE);

// extract an iso-surface from a RAW volume out-of-core
char *extractRAWvolume(FILE *file, // source file desc
                       const char *output, // destination file name /wo suffix .raw
                       long long width,long long height,long long depth=1,long long steps=1,
                       unsigned int components=1,unsigned int bits=8,BOOLINT sign=FALSE,BOOLINT msb=TRUE,
                       float scalex=1.0f,float scaley=1.0f,float scalez=1.0f,
                       double isovalue=0.5, // iso value to be extracted as surface
                       void (*feedback)(const char *info,float percent,void *obj)=NULL,void *obj=NULL); // feedback callback

// extract an iso-surface from a RAW volume out-of-core
char *extractRAWvolume(const char *filename, // source file
                       const char *output, // destination file name /wo suffix .raw
                       double isovalue=0.5, // iso value to be extracted as surface
                       void (*feedback)(const char *info,float percent,void *obj)=NULL,void *obj=NULL); // feedback callback

#endif
