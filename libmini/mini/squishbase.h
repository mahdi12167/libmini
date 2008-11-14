// (c) by Stefan Roettger

#ifndef SQUISHBASE_H
#define SQUISHBASE_H

namespace squishbase {

// S3TC compression modes
enum {SQUISHMODE_FAST,SQUISHMODE_GOOD,SQUISHMODE_SLOW};

// compress to S3TC
void compressS3TC(int isrgbadata,unsigned char *rawdata,unsigned int bytes,
                  unsigned char **s3tcdata,unsigned int *s3tcbytes,int width,int height,
                  int mode=SQUISHMODE_FAST); // compress as fast as possible by default

// decompress from S3TC
void decompressS3TC(int isrgbadata,unsigned char *s3tcdata,unsigned int bytes,
                    unsigned char **rawdata,unsigned int *rawbytes,int width,int height);

// S3TC auto-compression hook
void autocompress(int isrgbadata,unsigned char *rawdata,unsigned int bytes,
                  unsigned char **s3tcdata,unsigned int *s3tcbytes,int width,int height,
                  void *data);

// S3TC auto-decompression hook
void autodecompress(int isrgbadata,unsigned char *s3tcdata,unsigned int bytes,
                    unsigned char **rawdata,unsigned int *rawbytes,int width,int height,
                    void *data);

// set squish mode for auto-compression
void setautocompressmode(int mode=SQUISHMODE_FAST);

}

#endif
