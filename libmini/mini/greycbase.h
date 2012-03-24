// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef GREYCBASE_H
#define GREYCBASE_H

namespace greycbase {

void denoiseGREYCimage(unsigned char *image,int width,int height,int components,
                       float p=0.8f,float a=0.4f);

}

#endif
