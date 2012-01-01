// (c) by Stefan Roettger

#ifndef MININOISE_H
#define MININOISE_H

class mininoise
   {
   public:

   mininoise(int sx=64,int sy=64,int sz=64,
             int start=4,float persist=0.5f,
             float seed=0.0f);

   ~mininoise();

   protected:

   static float *noise(int sx=64,int sy=64,int sz=64,
                       int start=4,float persist=0.5f,
                       float seed=0.0f);

   inline static float get(const float *data,
                           int sx,int sy,int sz,
                           int x,int y,int z)
      {return(data[x+(y+z*sy)*sx]);}

   inline static void set(float *data,
                          int sx,int sy,int sz,
                          int x,int y,int z,
                          float v)
      {data[x+(y+z*sy)*sx]=v;}

   inline static void add(float *data,
                          int sx,int sy,int sz,
                          int x,int y,int z,
                          float v)
      {data[x+(y+z*sy)*sx]+=v;}

   static float getrandom(float seed=-1.0f);

   inline static float interpolate(float v0,float v1,float v2,float v3,float x);

   static float interpolate(float *data,
                            int sx,int sy,int sz,
                            float c1,float c2,float c3);

   static void average(float *data,
                       int sx,int sy,int sz,
                       int x1,int y1,int z1,
                       int x2,int y2,int z2,
                       int dx,int dy,int dz);
   };

#endif
