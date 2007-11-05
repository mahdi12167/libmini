// (c) by Stefan Roettger

#ifndef MINITEXT_H
#define MINITEXT_H

class minitext
   {
   public:

   static void drawline(float x1,float y1,float x2,float y2,
                        float hue,float sat,float val,float alpha);

   static void drawlineRGBA(float x1,float y1,float x2,float y2,
                            float r,float g,float b,float alpha);

   static void drawquad(float x,float y,float width,float height,
                        float hue,float sat,float val,float alpha);

   static void drawquadRGBA(float x,float y,float width,float height,
                            float r,float g,float b,float alpha);

   static void drawframe(float x,float y,float width,float height,
                         float hue,float sat,float val,float alpha);

   static void drawframeRGBA(float x,float y,float width,float height,
                             float r,float g,float b,float alpha);

   static void drawstring(float width,
                          float hue,float sat,float val,float alpha,char *str,
                          float backval=0.0f,float backalpha=0.0f);

   // configuring
   static void configure_zfight(float zscale=0.95f); // shift string towards the viewer to avoid Z-fighting with background

   protected:

   static void drawsymbol(float hue,float sat,float val,float alpha,char *symbol);
   static void drawletter(float hue,float sat,float val,float alpha,char letter);

   private:

   static float CONFIGURE_ZSCALE;
   };

#endif
