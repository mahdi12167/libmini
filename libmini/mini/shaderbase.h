// (c) by Stefan Roettger

#ifndef SHADERBASE_H
#define SHADERBASE_H

#include "minicache.h"

namespace shaderbase {

void setVISshader(minicache *cache,
                  float scale,float exaggeration,
                  float fogstart,float fogend,
                  float fogdensity,
                  float fogcolor[3],
                  float bathystart,float bathyend,
                  float contours,
                  float sealevel,float seabottom,
                  float seacolor[3],
                  float seatrans,float bottomtrans,
                  float bottomcolor[3],
                  float seamodulate);

void setVISbathymap(unsigned char *bathymap,
                    int bathywidth,int bathyheight,int bathycomps);

void setNPRshader(minicache *cache,
                  float scale,float exaggeration,
                  float fogstart,float fogend,
                  float fogdensity,
                  float fogcolor[3],
                  float bathystart,float bathyend,
                  float contours,
                  float fadefactor,
                  float sealevel,
                  float seacolor[3],float seatrans,
                  float seagrey);

void setNPRbathymap(unsigned char *bathymap,
                    int bathywidth,int bathyheight,int bathycomps);

void unsetshaders(minicache *cache);

}

using namespace shaderbase;

#endif
