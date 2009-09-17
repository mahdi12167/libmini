// (c) by Stefan Roettger

#include "minibase.h"

#include "minihsv.h"

namespace minihsv {

// rgb to hsv conversion
void rgb2hsv(float r,float g,float b,float hsv[3])
   {
   float h,s,v;
   float maxv,minv,diff,rdist,gdist,bdist;

   if (r<0.0f || r>1.0f || g<0.0f || g>1.0f || b<0.0f || b>1.0f) ERRORMSG();

   maxv=fmax(r,fmax(g,b));
   minv=fmin(r,fmin(g,b));
   diff=maxv-minv;

   v=maxv;

   if (maxv!=0.0f) s=diff/maxv;
   else s=0.0f;

   if (s==0.0f) h=0.0f;
   else
      {
      rdist=(maxv-r)/diff;
      gdist=(maxv-g)/diff;
      bdist=(maxv-b)/diff;

      if (r==maxv) h=bdist-gdist;
      else if (g==maxv) h=2.0f+rdist-bdist;
      else h=4.0f+gdist-rdist;

      h*=60.0f;
      if (h<0.0f) h+=360.0f;
      }

   hsv[0]=h;
   hsv[1]=s;
   hsv[2]=v;
   }

// hsv to rgb conversion
void hsv2rgb(float hue,float sat,float val,float rgb[3])
   {
   float hue6,r,s,t;

   if (hue<0.0f || sat<0.0f || sat>1.0f || val<0.0f || val>1.0f) ERRORMSG();

   hue/=60.0f;
   hue=hue-6.0f*ftrc(hue/6.0f);
   hue6=hue-ftrc(hue);

   r=val*(1.0f-sat);
   s=val*(1.0f-sat*hue6);
   t=val*(1.0f-sat*(1.0f-hue6));

   switch (ftrc(hue))
        {
        case 0: // red -> yellow
           rgb[0] = val;
           rgb[1] = t;
           rgb[2] = r;
           break;
        case 1: // yellow -> green
           rgb[0] = s;
           rgb[1] = val;
           rgb[2] = r;
           break;
        case 2: // green -> cyan
           rgb[0] = r;
           rgb[1] = val;
           rgb[2] = t;
           break;
        case 3: // cyan -> blue
           rgb[0] = r;
           rgb[1] = s;
           rgb[2] = val;
           break;
        case 4: // blue -> magenta
           rgb[0] = t;
           rgb[1] = r;
           rgb[2] = val;
           break;
        case 5: // magenta -> red
           rgb[0] = val;
           rgb[1] = r;
           rgb[2] = s;
           break;
        }
   }

// rgb to ycbcr conversion
void rgb2ycbcr(float r,float g,float b,float ycbcr[3],float gamma)
   {
   // apply gamma correction
   if (gamma!=1.0)
      {
      r=fpow(r,gamma);
      g=fpow(g,gamma);
      b=fpow(b,gamma);
      }

   // apply conversion matrix
   ycbcr[0] =     0.299f*r +     0.587f*g +     0.114f*b;
   ycbcr[1] = -0.168736f*r + -0.331264f*g +       0.5f*b;
   ycbcr[2] =       0.5f*r + -0.418688f*g + -0.081312f*b;
   }

// ycbcr to rgb conversion
void ycbcr2rgb(float y,float cb,float cr,float rgb[3],float gamma)
   {
   float r,g,b;

   // apply conversion matrix
   r = y + 1.402f*cr;
   g = y - 0.714f*cr - 0.344f*cb;
   b = y + 1.772f*cb;

   // apply inverse gamma correction
   if (gamma!=1.0)
      {
      rgb[0]=fpow(r,1.0f/gamma);
      rgb[1]=fpow(g,1.0f/gamma);
      rgb[2]=fpow(b,1.0f/gamma);
      }
   else
      {
      rgb[0]=r;
      rgb[1]=g;
      rgb[2]=b;
      }
   }

// rgb to xyz conversion
void rgb2xyz(float r,float g,float b,float xyz[3],float gamma)
   {
   // apply gamma correction
   if (gamma!=1.0)
      {
      r=fpow(r,gamma);
      g=fpow(g,gamma);
      b=fpow(b,gamma);
      }
   else
      {
      if (r>0.04045f) r=fpow((r+0.055f)/1.055f,2.4f); else r/=12.92f;
      if (g>0.04045f) g=fpow((g+0.055f)/1.055f,2.4f); else g/=12.92f;
      if (b>0.04045f) b=fpow((b+0.055f)/1.055f,2.4f); else b/=12.92f;
      }

   // assume sRGB
   xyz[0] = 0.4124564f*r + 0.3575761f*g + 0.1804375f*b;
   xyz[1] = 0.2126729f*r + 0.7151522f*g + 0.0721750f*b;
   xyz[2] = 0.0193339f*r + 0.1191920f*g + 0.9503041f*b;
   }

// xyz to rgb conversion
void xyz2rgb(float x,float y,float z,float rgb[3],float gamma)
   {
   float r,g,b;

   // assume sRGB
   r =  3.2404542f*x + -1.5371385f*y + -0.4985314f*z;
   g = -0.9692660f*x +  1.8760108f*y +  0.0415560f*z;
   b =  0.0556434f*x + -0.2040259f*y +  1.0572252f*z;

   // apply inverse gamma correction
   if (gamma!=1.0)
      {
      rgb[0]=fpow(r,1.0f/gamma);
      rgb[1]=fpow(g,1.0f/gamma);
      rgb[2]=fpow(b,1.0f/gamma);
      }
   else
      {
      if (r>0.0031308f) rgb[0]=1.055f*fpow(r,1.0f/2.4f)-0.055f; else rgb[0]=12.92f*r;
      if (g>0.0031308f) rgb[1]=1.055f*fpow(g,1.0f/2.4f)-0.055f; else rgb[1]=12.92f*g;
      if (b>0.0031308f) rgb[2]=1.055f*fpow(b,1.0f/2.4f)-0.055f; else rgb[2]=12.92f*b;
      }
   }

// xyz to lab conversion
void xyz2lab(float x,float y,float z,float lab[3])
   {
   float white[3];

   float xr,yr,zr;
   float xn,yn,zn;

   // choose white point
   k2white(6500.0f,white); //assume sRGB

   // denormalize white point
   xr=white[0]/white[1];
   yr=1.0f;
   zr=white[2]/white[1];

   // normalize xyz components
   xn=x/xr;
   yn=y/yr;
   zn=z/zr;

   // equalize xyz components
   if (xn>216.0f/24389.0f) xn=fpow(xn,1.0f/3); else xn=(24389.0f/27.0f*xn+16.0f)/116.0f;
   if (yn>216.0f/24389.0f) yn=fpow(yn,1.0f/3); else yn=(24389.0f/27.0f*yn+16.0f)/116.0f;
   if (zn>216.0f/24389.0f) zn=fpow(zn,1.0f/3); else zn=(24389.0f/27.0f*zn+16.0f)/116.0f;

   // compute lab components
   lab[0]=1.16f*yn-0.16f; // brightness range=0..1
   lab[1]=5.0f*(xn-yn); // a<0=green -> red=a>0
   lab[2]=2.0f*(yn-zn); // b<0=blue -> yellow=b>0
   }

// lab to xyz conversion
void lab2xyz(float l,float a,float b,float xyz[3])
   {
   float white[3];

   float xr,yr,zr;
   float xn,yn,zn;

   // choose white point
   k2white(6500.0f,white); //assume sRGB

   // denormalize white point
   xr=white[0]/white[1];
   yr=1.0f;
   zr=white[2]/white[1];

   // compute xyz components
   yn=(l+0.16f)/1.16f;
   xn=a/5.0f+yn;
   zn=yn-b/2.0f;

   // deequalize xyz components
   if (xn*xn*xn>216.0f/24389.0f) xn*=xn*xn; else xn=(116.0f*xn-16.0f)*(27.0f/24389.0f);
   if (yn*yn*yn>216.0f/24389.0f) yn*=yn*yn; else yn=(116.0f*yn-16.0f)*(27.0f/24389.0f);
   if (zn*zn*zn>216.0f/24389.0f) zn*=zn*zn; else zn=(116.0f*zn-16.0f)*(27.0f/24389.0f);

   // denormalize xyz components
   xyz[0]=xn*xr;
   xyz[1]=yn*yr;
   xyz[2]=zn*zr;
   }

// chromatic adaption
void xyz2xyz(float kin,float kout,float x,float y,float z,float xyz[3])
   {
   float white1[3],white2[3];

   float xt,yt,zt;

   // choose white points
   k2white(kin,white1);
   k2white(kout,white2);

   // denormalize white points
   white1[0]/=white1[1];
   white1[2]/=white1[1];
   white1[1]=1.0f;
   white2[0]/=white2[1];
   white2[2]/=white2[1];
   white2[1]=1.0f;

   // transform forward (Bradford)
   xt =  0.8951f*x +  0.2664f*y + -0.1614f*z;
   yt = -0.7502f*x +  1.7135f*y +  0.0367f*z;
   zt =  0.0389f*x + -0.0685f*y +  1.0296f*z;

   // adapt chroma
   xt*=white2[0]/white1[0];
   yt*=white2[1]/white1[1];
   zt*=white2[2]/white1[2];

   // transform backward
   xyz[0] =  0.986993f*xt + -0.147054f*yt + 0.159963f*zt;
   xyz[1] =  0.432305f*xt +  0.518360f*yt + 0.049291f*zt;
   xyz[2] = -0.008529f*xt +  0.040043f*yt + 0.968487f*zt;
   }

// convert kelvin to white point
void k2white(float kelvin,float white[3])
   {
   double c;
   float xr,yr;

   // white point at 5000K/D50 (wide-gamut RGB)
   static const float xr5000K=0.3457f;
   static const float yr5000K=0.3585;

   // day light at 5500K/D55
   static const float xr5500K=0.3324f;
   static const float yr5500K=0.3474f;

   // white point at 6500K/D65 (sRGB)
   static const float xr6500K=0.312713f;
   static const float yr6500K=0.329016f;

   if (kelvin<5000.0f) kelvin=5000.0f;
   if (kelvin>6500.0f) kelvin=6500.0f;

   if (kelvin<5500.0f)
      {
      c=(kelvin-5000.0f)/500.0f;
      xr=xr5000K+c*(xr5500K-xr5000K);
      yr=yr5000K+c*(yr5500K-yr5000K);
      }
   else
      {
      c=(kelvin-5500.0f)/1000.0f;
      xr=xr5500K+c*(xr6500K-xr5500K);
      yr=yr5500K+c*(yr6500K-yr5500K);
      }

   white[0]=xr;
   white[1]=yr;
   white[2]=1.0f-xr-yr;
   }

}
