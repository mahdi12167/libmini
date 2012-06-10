// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIIO_H
#define MINIIO_H

#include <stdio.h>

namespace miniio {

//! check a file
int checkfile(const char *filename);

//! write a RAW file
void writefile(const char *filename,unsigned char *data,unsigned int bytes);

//! read a RAW file
unsigned char *readfile(const char *filename,unsigned int *bytes);

//! read from a RAW file
unsigned char *readfiled(FILE *file,unsigned int *bytes);

//! write a string as RAW file
void writestring(const char *filename,const char *cstr);

//! read a RAW file as string
char *readstring(const char *filename);

//! read a RAW file and compute signature
unsigned int signature(const char *filename);

//! compute a signature
unsigned int signature(const unsigned char *data,unsigned int bytes);

}

using namespace miniio;

#endif
