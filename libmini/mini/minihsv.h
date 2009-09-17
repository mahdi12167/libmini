// (c) by Stefan Roettger

#ifndef MINIHSV_H
#define MINIHSV_H

namespace minihsv {

void rgb2hsv(float r,float g,float b,float hsv[3]);
void hsv2rgb(float hue,float sat,float val,float rgb[3]);

void rgb2ycbcr(float r,float g,float b,float ycbcr[3],float gamma=2.2f);
void ycbcr2rgb(float y,float cb,float cr,float rgb[3],float gamma=2.2f);

void rgb2xyz(float r,float g,float b,float xyz[3],float gamma=1.0f);
void xyz2rgb(float x,float y,float z,float rgb[3],float gamma=1.0f);

void lab2xyz(float l,float a,float b,float xyz[3]);
void xyz2lab(float x,float y,float z,float lab[3]);

void k2white(float kelvin,float white[3]);

}

using namespace minihsv;

#endif
