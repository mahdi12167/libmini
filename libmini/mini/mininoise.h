// (c) by Stefan Roettger

#ifndef MININOISE_H
#define MININOISE_H

float *noise(int sx=64,int sy=64,int sz=64,
             int start=4,float persist=0.5f,
             float seed=0.0f);

#endif
