// (c) by Stefan Roettger

#ifndef MINISHADER_H
#define MINISHADER_H

#include "minicache.h"

class minishader
   {
   public:

   static void setVISshader(minicache *cache,
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

   static void setVISbathymap(unsigned char *bathymap,
                              int bathywidth,int bathyheight,int bathycomps);

   static void setNPRshader(minicache *cache,
                            float scale,float exaggeration,
                            float fogstart,float fogend,
                            float fogdensity,
                            float fogcolor[3],
                            float bathystart,float bathyend,
                            float contours,
                            float fadefactor,
                            float sealevel,
                            float seacolor[3],float seatrans,
                            float seagray);

   static void setNPRbathymap(unsigned char *bathymap,
                              int bathywidth,int bathyheight,int bathycomps);

   static void setcontourmode(int mode);
   static void setdetailtexmode(int mode,float alpha=1.0f,int mask=0);
   static void setseamode(int mode);
   static void setfademode(int mode,float fadestart,float fadeend);

   static void unsetshaders(minicache *cache);

   static void initbathymap_linear(unsigned char *map,int width,
                                   float hue1,float hue2,
                                   float sat1,float sat2,
                                   float val1,float val2,
                                   float mid);

   static void initbathymap_contour(unsigned char *map,int width,
                                    float hue1,float hue2,float alpha,
                                    float start,float end,float contours,
                                    float hue,float sat,float val);

   private:

   static unsigned char *VISBATHYMAP;
   static int VISBATHYWIDTH,VISBATHYHEIGHT,VISBATHYCOMPS;
   static int VISBATHYMOD;

   static unsigned char *NPRBATHYMAP;
   static int NPRBATHYWIDTH,NPRBATHYHEIGHT,NPRBATHYCOMPS;
   static int NPRBATHYMOD;

   static int CONTOURMODE;

   static int DETAILTEXMODE;
   static float DETAILTEXALPHA;
   static int DETAILTEXMASK;

   static int SEAMODE;

   static int FADEMODE;
   static float FADESTART;
   static float FADEEND;

   static float mapt(float t);
   };

#endif
