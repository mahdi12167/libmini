// (c) by Stefan Roettger

#ifndef SQUISHBASE_H
#define SQUISHBASE_H

namespace squishbase {

enum MODE_ENUM {MODE_FAST,MODE_GOOD,MODE_SLOW};

void compressS3TC(int isrgbadata,unsigned char *rawdata,unsigned int bytes,
                  unsigned char **s3tcdata,unsigned int *s3tcbytes,
                  int width,int height);

void setmode(MODE_ENUM mode=MODE_FAST);

}

//using namespace squishbase;

#endif
