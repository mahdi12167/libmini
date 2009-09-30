// (c) by Stefan Roettger

#ifndef MINIRGB_H
#define MINIRGB_H

namespace minirgb {

//! HSV (hue scaled to 0..360)
void rgb2hsv(float r,float g,float b,float hsv[3]);
void hsv2rgb(float hue,float sat,float val,float rgb[3]);

//! YCbCr
void rgb2ycbcr(float r,float g,float b,float ycbcr[3],float gamma=2.2f);
void ycbcr2rgb(float y,float cb,float cr,float rgb[3],float gamma=2.2f);

//! XYZ (scaled to 0..1, assumes sRGB)
void rgb2xyz(float r,float g,float b,float xyz[3],float gamma=1.0f);
void xyz2rgb(float x,float y,float z,float rgb[3],float gamma=1.0f);

// Lab (scaled to 0..1)
void xyz2lab(float x,float y,float z,float lab[3]);
void lab2xyz(float l,float a,float b,float xyz[3]);

//! chromatic adaption
void xyz2xyz(float kin,float kout,float x,float y,float z,float xyz[3]);

//! white point (normalized vector)
void k2white(float kelvin,float white[3]);

}

using namespace minirgb;

#endif
