// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MININOISE_H
#define MININOISE_H

class mininoise
   {
   public:

   //! default constructor
   mininoise(int sx=64,int sy=64,int sz=64,
             int start=4,float persist=0.5f,
             float seed=0.0f);

   //! default destructor
   ~mininoise();

   //! cubic interpolation of perlin noise
   float interpolate(float c1,float c2=0.0f,float c3=0.0f);

   private:

   int sizex,sizey,sizez;
   float *data;

   inline static float get(const float *data,
                           int sx,int sy,int sz,
                           int x,int y,int z)
      {
      if (x<0 || x>=sx || y<0 || y>=sy || z<0 || z>=sz) ERRORMSG();
      return(data[x+(y+z*sy)*sx]);
      }

   inline static void set(float *data,
                          int sx,int sy,int sz,
                          int x,int y,int z,
                          float v)
      {
      if (x<0 || x>=sx || y<0 || y>=sy || z<0 || z>=sz) ERRORMSG();
      data[x+(y+z*sy)*sx]=v;
      }

   inline static void add(float *data,
                          int sx,int sy,int sz,
                          int x,int y,int z,
                          float v)
      {
      if (x<0 || x>=sx || y<0 || y>=sy || z<0 || z>=sz) ERRORMSG();
      data[x+(y+z*sy)*sx]+=v;
      }

   inline static float interpolate(float v0,float v1,float v2,float v3,float x);

   inline static float interpolatex(const float *data,
                                    int sx,int sy,int sz,
                                    int k1,int k2,int k3,
                                    float wx);

   inline static float interpolatey(const float *data,
                                    int sx,int sy,int sz,
                                    int k1,int k2,int k3,
                                    float wx,float wy);

   inline static float interpolatez(const float *data,
                                    int sx,int sy,int sz,
                                    int k1,int k2,int k3,
                                    float wx,float wy,float wz);

   static float interpolate(const float *data,
                            int sx,int sy,int sz,
                            float c1,float c2,float c3);

   static void average(float *data,
                       int sx,int sy,int sz,
                       int x1,int y1,int z1,
                       int x2,int y2,int z2,
                       int dx,int dy,int dz);

   static float *noise(int sx=64,int sy=64,int sz=64,
                       int start=4,float persist=0.5f,
                       float seed=0.0f);

   static float getrandom(float seed=-1.0f);
   };

#endif
