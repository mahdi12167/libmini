// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIDDS_H
#define MINIDDS_H

#include "minibase.h"

//! encode a Differential Data Stream
void DDS_encode(unsigned char *data,unsigned int bytes,
                unsigned int skip,unsigned int strip,
                unsigned char **chunk,unsigned int *size,
                unsigned int block=0);

//! decode a Differential Data Stream
void DDS_decode(unsigned char *chunk,unsigned int size,
                unsigned char **data,unsigned int *bytes,
                unsigned int block=0);

//! write a Differential Data Stream
void writeDDSfile(const char *filename,
                  unsigned char *data,unsigned int bytes,
                  unsigned int skip=0,unsigned int strip=0,
                  BOOLINT nofree=FALSE);

//! read a Differential Data Stream
unsigned char *readDDSfile(const char *filename,unsigned int *bytes);

#endif
