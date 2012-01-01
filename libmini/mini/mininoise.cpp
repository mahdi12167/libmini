// (c) by Stefan Roettger

#include "minibase.h"

#include "mininoise.h"

mininoise::mininoise(int sx,int sy,int sz,
                     int start,float persist,
                     float seed)
   {}

mininoise::~mininoise()
   {}

float mininoise::getrandom(float seed)
   {
   static const long long maxbits=50;
   static const long long maxnum=1ull<<maxbits;

   static long long number=0;

   if (seed>=0.0f && seed<=1.0f) number=ftrc(seed*(maxnum-1)+0.5f);

   number=271*(number+331);
   number=(number<<(maxbits/3))+(number>>(2*maxbits/3));
   number&=maxnum-1;

   return((float)number/(maxnum-1));
   }

float mininoise::interpolate(float v0,float v1,float v2,float v3,float x)
   {
   float p,q,r;

   p=v3-v2+v1-v0;
   q=v0-v1-p;
   r=v2-v0;

   return(((p*x+q)*x+r)*x+v1);
   }

float mininoise::interpolate(float *data,
                             int sx,int sy,int sz,
                             float c1,float c2,float c3)
   {
   int k1,k2,k3;
   float w1,w2,w3;

   float v0,v1,v2,v3,
         v4,v5,v6,v7,
         v8,v9,v10,v11,
         v12,v13,v14,v15;

   k1=ftrc(c1*(sx-1));
   w1=c1*(sx-1)-k1;

   if (k1<0)
      {
      k1=0;
      w1=0.0f;
      }

   if (k1>sx-2)
      {
      k1=sx-2;
      w1=1.0f;
      }

   k2=ftrc(c2*(sy-1));
   w2=c2*(sy-1)-k2;

   if (k2<0)
      {
      k2=0;
      w2=0.0f;
      }

   if (k2>sy-2)
      {
      k2=sy-2;
      w2=1.0f;
      }

   k3=ftrc(c3*(sz-1));
   w3=c3*(sz-1)-k3;

   if (k3<0)
      {
      k3=0;
      w3=0.0f;
      }

   if (k3>sz-2)
      {
      k3=sz-2;
      w3=1.0f;
      }

   v1=interpolate((k1>0)?data[k1-1+(k2+k3*sy)*sx]:data[k1+(k2+k3*sy)*sx],
                  data[k1+(k2+k3*sy)*sx],data[k1+1+(k2+k3*sy)*sx],
                  (k1<sx-2)?data[k1+2+(k2+k3*sy)*sx]:data[k1+1+(k2+k3*sy)*sx],w1);

   if (k2>0)
      v0=interpolate((k1>0)?data[k1-1+(k2-1+k3*sy)*sx]:data[k1+(k2-1+k3*sy)*sx],
                     data[k1+(k2-1+k3*sy)*sx],data[k1+1+(k2-1+k3*sy)*sx],
                     (k1<sx-2)?data[k1+2+(k2-1+k3*sy)*sx]:data[k1+1+(k2-1+k3*sy)*sx],w1);
   else v0=v1;

   v2=interpolate((k1>0)?data[k1-1+(k2+1+k3*sy)*sx]:data[k1+(k2+1+k3*sy)*sx],
                  data[k1+(k2+1+k3*sy)*sx],data[k1+1+(k2+1+k3*sy)*sx],
                  (k1<sx-2)?data[k1+2+(k2+1+k3*sy)*sx]:data[k1+1+(k2+1+k3*sy)*sx],w1);

   if (k2<sy-2)
      v3=interpolate((k1>0)?data[k1-1+(k2+2+k3*sy)*sx]:data[k1+(k2+2+k3*sy)*sx],
                     data[k1+(k2+2+k3*sy)*sx],data[k1+1+(k2+2+k3*sy)*sx],
                     (k1<sx-2)?data[k1+2+(k2+2+k3*sy)*sx]:data[k1+1+(k2+2+k3*sy)*sx],w1);
   else v3=v2;

   v5=interpolate((k1>0)?data[k1-1+(k2+(k3+1)*sy)*sx]:data[k1+(k2+(k3+1)*sy)*sx],
                  data[k1+(k2+(k3+1)*sy)*sx],data[k1+1+(k2+(k3+1)*sy)*sx],
                  (k1<sx-2)?data[k1+2+(k2+(k3+1)*sy)*sx]:data[k1+1+(k2+(k3+1)*sy)*sx],w1);

   if (k2>0)
      v4=interpolate((k1>0)?data[k1-1+(k2-1+(k3+1)*sy)*sx]:data[k1+(k2-1+(k3+1)*sy)*sx],
                     data[k1+(k2-1+(k3+1)*sy)*sx],data[k1+1+(k2-1+(k3+1)*sy)*sx],
                     (k1<sx-2)?data[k1+2+(k2-1+(k3+1)*sy)*sx]:data[k1+1+(k2-1+(k3+1)*sy)*sx],w1);
   else v4=v5;

   v6=interpolate((k1>0)?data[k1-1+(k2+1+(k3+1)*sy)*sx]:data[k1+(k2+1+(k3+1)*sy)*sx],
                  data[k1+(k2+1+(k3+1)*sy)*sx],data[k1+1+(k2+1+(k3+1)*sy)*sx],
                  (k1<sx-2)?data[k1+2+(k2+1+(k3+1)*sy)*sx]:data[k1+1+(k2+1+(k3+1)*sy)*sx],w1);

   if (k2<sy-2)
      v7=interpolate((k1>0)?data[k1-1+(k2+2+(k3+1)*sy)*sx]:data[k1+(k2+2+(k3+1)*sy)*sx],
                     data[k1+(k2+2+(k3+1)*sy)*sx],data[k1+1+(k2+2+(k3+1)*sy)*sx],
                     (k1<sx-2)?data[k1+2+(k2+2+(k3+1)*sy)*sx]:data[k1+1+(k2+2+(k3+1)*sy)*sx],w1);
   else v7=v6;

   if (k3>0)
      {
      v9=interpolate((k1>0)?data[k1-1+(k2+(k3-1)*sy)*sx]:data[k1+(k2+(k3-1)*sy)*sx],
                     data[k1+(k2+(k3-1)*sy)*sx],data[k1+1+(k2+(k3-1)*sy)*sx],
                     (k1<sx-2)?data[k1+2+(k2+(k3-1)*sy)*sx]:data[k1+1+(k2+(k3-1)*sy)*sx],w1);

      if (k2>0)
         v8=interpolate((k1>0)?data[k1-1+(k2-1+(k3-1)*sy)*sx]:data[k1+(k2-1+(k3-1)*sy)*sx],
                        data[k1+(k2-1+(k3-1)*sy)*sx],data[k1+1+(k2-1+(k3-1)*sy)*sx],
                        (k1<sx-2)?data[k1+2+(k2-1+(k3-1)*sy)*sx]:data[k1+1+(k2-1+(k3-1)*sy)*sx],w1);
      else v8=v9;

      v10=interpolate((k1>0)?data[k1-1+(k2+1+(k3-1)*sy)*sx]:data[k1+(k2+1+(k3-1)*sy)*sx],
                      data[k1+(k2+1+(k3-1)*sy)*sx],data[k1+1+(k2+1+(k3-1)*sy)*sx],
                      (k1<sx-2)?data[k1+2+(k2+1+(k3-1)*sy)*sx]:data[k1+1+(k2+1+(k3-1)*sy)*sx],w1);

      if (k2<sy-2)
         v11=interpolate((k1>0)?data[k1-1+(k2+2+(k3-1)*sy)*sx]:data[k1+(k2+2+(k3-1)*sy)*sx],
                         data[k1+(k2+2+(k3-1)*sy)*sx],data[k1+1+(k2+2+(k3-1)*sy)*sx],
                         (k1<sx-2)?data[k1+2+(k2+2+(k3-1)*sy)*sx]:data[k1+1+(k2+2+(k3-1)*sy)*sx],w1);
      else v11=v10;
      }
   else
      {
      v8=v0;
      v9=v1;
      v10=v2;
      v11=v3;
      }

   if (k3<sz-2)
      {
      v13=interpolate((k1>0)?data[k1-1+(k2+(k3+2)*sy)*sx]:data[k1+(k2+(k3+2)*sy)*sx],
                      data[k1+(k2+(k3+2)*sy)*sx],data[k1+1+(k2+(k3+2)*sy)*sx],
                      (k1<sx-2)?data[k1+2+(k2+(k3+2)*sy)*sx]:data[k1+1+(k2+(k3+2)*sy)*sx],w1);

      if (k2>0)
         v12=interpolate((k1>0)?data[k1-1+(k2-1+(k3+2)*sy)*sx]:data[k1+(k2-1+(k3+2)*sy)*sx],
                         data[k1+(k2-1+(k3+2)*sy)*sx],data[k1+1+(k2-1+(k3+2)*sy)*sx],
                         (k1<sx-2)?data[k1+2+(k2-1+(k3+2)*sy)*sx]:data[k1+1+(k2-1+(k3+2)*sy)*sx],w1);
      else v12=v13;

      v14=interpolate((k1>0)?data[k1-1+(k2+1+(k3+2)*sy)*sx]:data[k1+(k2+1+(k3+2)*sy)*sx],
                      data[k1+(k2+1+(k3+2)*sy)*sx],data[k1+1+(k2+1+(k3+2)*sy)*sx],
                      (k1<sx-2)?data[k1+2+(k2+1+(k3+2)*sy)*sx]:data[k1+1+(k2+1+(k3+2)*sy)*sx],w1);

      if (k2<sy-2)
         v15=interpolate((k1>0)?data[k1-1+(k2+2+(k3+2)*sy)*sx]:data[k1+(k2+2+(k3+2)*sy)*sx],
                         data[k1+(k2+2+(k3+2)*sy)*sx],data[k1+1+(k2+2+(k3+2)*sy)*sx],
                         (k1<sx-2)?data[k1+2+(k2+2+(k3+2)*sy)*sx]:data[k1+1+(k2+2+(k3+2)*sy)*sx],w1);
      else v15=v14;
      }
   else
      {
      v12=v4;
      v13=v5;
      v14=v6;
      v15=v7;
      }

   return(interpolate(interpolate(v8,v9,v10,v11,w2),
                      interpolate(v0,v1,v2,v3,w2),
                      interpolate(v4,v5,v6,v7,w2),
                      interpolate(v12,v13,v14,v15,w2),w3));
   }

void mininoise::average(float *data,
                        int sx,int sy,int sz,
                        int x1,int y1,int z1,
                        int x2,int y2,int z2,
                        int dx,int dy,int dz)
   {
   int x,y,z;

   float v,v1,v2;

   for (x=0; x<dx; x++)
      for (y=0; y<dy; y++)
         for (z=0; z<dz; z++)
            {
            v1=get(data,sx,sy,sz,x1+x,y1+y,z1+z);
            v2=get(data,sx,sy,sz,x2+x,y2+y,z2+z);
            v=(v1+v2)/2.0f;
            set(data,sx,sy,sz,x1+x,y1+y,z1+z,v);
            set(data,sx,sy,sz,x2+x,y2+y,z2+z,v);
            }
   }

float *mininoise::noise(int sx,int sy,int sz,
                        int start,float persist,
                        float seed)
   {
   int i;
   int x,y,z;

   int size;
   int dx,dy,dz;

   float scaling;

   float *noise,*octave;

   float v,minv,maxv;

   if ((sx&(sx-1))!=0 || sx<2) ERRORMSG();
   if ((sy&(sy-1))!=0 || sy<2) ERRORMSG();
   if ((sz&(sz-1))!=0 || sz<2) ERRORMSG();

   if ((noise=(float *)malloc(sx*sy*sz*sizeof(float)))==NULL) ERRORMSG();

   for (x=0; x<sx; x++)
      for (y=0; y<sy; y++)
         for (z=0; z<sz; z++) set(noise,sx,sy,sz,x,y,z,0.0f);

   if ((octave=(float *)malloc(sx*sy*sz*sizeof(float)))==NULL) ERRORMSG();

   size=max(sx,max(sy,sz));

   getrandom(seed);

   scaling=1.0f;

   for (i=start; i<=size; i*=2)
      {
      dx=max(i*sx/size,1);
      dy=max(i*sy/size,1);
      dz=max(i*sz/size,1);

      for (x=0; x<dx; x++)
         for (y=0; y<dy; y++)
            for (z=0; z<dz; z++)
               set(octave,dx,dy,dz,x,y,z,
                   getrandom()*scaling);

      // seamless noise via octave face averaging
      average(octave,dx,dy,dz,0,0,0,dx-1,0,0,1,dy,dz);
      average(octave,dx,dy,dz,0,0,0,0,dy-1,0,dx,1,dz);
      average(octave,dx,dy,dz,0,0,0,0,0,dz-1,dx,dy,1);

      for (x=0; x<sx; x++)
         for (y=0; y<sy; y++)
            for (z=0; z<sz; z++)
               add(noise,sx,sy,sz,x,y,z,
                   interpolate(octave,dx,dy,dz,
                               (float)x/(sx-1),(float)y/(sy-1),(float)z/(sz-1)));

      scaling*=persist;
      }

   free(octave);

   minv=MAXFLOAT;
   maxv=-MAXFLOAT;

   for (x=0; x<sx; x++)
      for (y=0; y<sy; y++)
         for (z=0; z<sz; z++)
            {
            v=get(noise,sx,sy,sz,x,y,z);
            if (v<minv) minv=v;
            if (v>maxv) maxv=v;
            }

   if (minv==maxv) maxv++;

   // normalization
   for (x=0; x<sx; x++)
      for (y=0; y<sy; y++)
         for (z=0; z<sz; z++)
            set(noise,sx,sy,sz,x,y,z,
                (get(noise,sx,sy,sz,x,y,z)-minv)/(maxv-minv));

   return(noise);
   }
