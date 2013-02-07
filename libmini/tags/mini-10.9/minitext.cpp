// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minibase.h"

#include "minirgb.h"
#include "miniOGL.h"

#include "minitext.h"

namespace minitext {

float CONFIGURE_ZSCALE=0.95f; // must be 1.0f for orthographic projections

void drawsymbol(const char *symbol)
   {
   float px,py,lx,ly;

   BOOLINT draw=TRUE;

   px=py=0.0f;

   while (*symbol!='\0')
      {
      lx=px;
      ly=py;

      switch (*symbol++)
         {
         case 'u': draw=FALSE; break;
         case 'd': draw=TRUE; break;
         case 'n': py+=1.0f; break;
         case 's': py-=1.0f; break;
         case 'e': px+=1.0f; break;
         case 'w': px-=1.0f; break;
         case 'N': py+=1.0f; px+=1.0f; break;
         case 'S': py-=1.0f; px-=1.0f; break;
         case 'E': px+=1.0f; py-=1.0f; break;
         case 'W': px-=1.0f; py+=1.0f; break;
         }

      if (draw)
         if (px!=lx || py!=ly) renderline(lx,ly,0.0f,px,py,0.0f);
      }
   }

void drawletter(char letter)
   {
   mtxpush();
   mtxscale(1.0f/4,1.0f/6,0.0f);

   switch (toupper(letter))
      {
      // letters occupy a 7x4 grid
      case 'A': drawsymbol("nnnnnNeEsssssunnndwww"); break;
      case 'B': drawsymbol("nnnnnneeEsSwwueedEsSww"); break;
      case 'C': drawsymbol("ueeNdSwWnnnnNeE"); break;
      case 'D': drawsymbol("nnnnnneeEssssSww"); break;
      case 'E': drawsymbol("ueeedwwwnnnnnneeeussswdww"); break;
      case 'F': drawsymbol("nnnnnneeeussswdww"); break;
      case 'G': drawsymbol("unnneedessSwWnnnnNeE"); break;
      case 'H': drawsymbol("nnnnnnueeedssssssunnndwww"); break;
      case 'I': drawsymbol("uedeeuwdnnnnnneuwdw"); break;
      case 'J': drawsymbol("undEeNnnnnnwww"); break;
      case 'K': drawsymbol("nnnnnnusssdNNNuSSSdEEE"); break;
      case 'L': drawsymbol("ueeedwwwnnnnnn"); break;
      case 'M': drawsymbol("nnnnnnEeNssssss"); break;
      case 'N': drawsymbol("nnnnnnusdEEEssunndnnnn"); break;
      case 'O': drawsymbol("uedWnnnnNeEssssSw"); break;
      case 'P': drawsymbol("nnnnnneeEsSww"); break;
      case 'Q': drawsymbol("uedWnnnnNeEssssSwuNdE"); break;
      case 'R': drawsymbol("nnnnnneeEsSwwuedEEs"); break;
      case 'S': drawsymbol("undEeNnWwWnNeE"); break;
      case 'T': drawsymbol("ueednnnnnnwwueedee"); break;
      case 'U': drawsymbol("unnnnnndsssssEeNnnnnn"); break;
      case 'V': drawsymbol("unnnnnndsssssENNnnnn"); break;
      case 'W': drawsymbol("unnnnnndssssssNeEnnnnnn"); break;
      case 'X': drawsymbol("nnNNNnuwwwdsEEEss"); break;
      case 'Y': drawsymbol("ueednnWWnnueeedsssS"); break;
      case 'Z': drawsymbol("ueeendswwwnnNNNnwwws"); break;
      case '_': drawsymbol("eee"); break;
      case '0': drawsymbol("uedWnnnnNeEssssSwuNNdWWW"); break;
      case '1': drawsymbol("ueednnnnnnSS"); break;
      case '2': drawsymbol("ueeedwwwNNNnnWwSs"); break;
      case '3': drawsymbol("undEeNnWwuedNnWwS"); break;
      case '4': drawsymbol("ueednnnnnnuwwdssseee"); break;
      case '5': drawsymbol("undEeNnWwwnnneee"); break;
      case '6': drawsymbol("unndNeEsSwWnnnnNeE"); break;
      case '7': drawsymbol("ueednnnnnnwwusssedee"); break;
      case '8': drawsymbol("uedWnNWnNeEsSwuedEsSw"); break;
      case '9': drawsymbol("undEeNnnnnWwSsEee"); break;
      case '.': drawsymbol("uednesw"); break;
      case ',': drawsymbol("uedN"); break;
      case ':': drawsymbol("unedneswunndnesw"); break;
      case ';': drawsymbol("uneedwnesSunnndnesw"); break;
      case '?': drawsymbol("ueednnnNnWwS"); break;
      case '!': drawsymbol("ueednundnnnn"); break;
      case '|': drawsymbol("ueednnnnnn"); break;
      case '-': drawsymbol("unnnedee"); break;
      case '+': drawsymbol("unnnedeeunwdss"); break;
      case '*': drawsymbol("unnedNNuwwdEE"); break;
      case '/': drawsymbol("undNNN"); break;
      case '>': drawsymbol("unedNNWW"); break;
      case '<': drawsymbol("uneeedWWNN"); break;
      case '=': drawsymbol("unnedeeunndww"); break;
      case '(': drawsymbol("ueedWnnnnN"); break;
      case ')': drawsymbol("uedNnnnnW"); break;
      case '[': drawsymbol("ueedwnnnnnne"); break;
      case ']': drawsymbol("uedennnnnnw"); break;
      case '{': drawsymbol("ueedwnnWNnne"); break;
      case '}': drawsymbol("udennNWnnw"); break;
      case '$': drawsymbol("unndEeNWwWNeEuWndssssssuwdnnnnnn"); break;
      case '%': drawsymbol("undNNNusssdswneuwwnnndnwse"); break;
      case '#': drawsymbol("unndeeeunndwwwuNdssssuednnnn"); break;
      case '&': drawsymbol("ueeedWWWnnNEssSSseNN"); break;
      case '@': drawsymbol("ueeendSwWnnnnNeEssswwnnee"); break;
      case '^': drawsymbol("uennnndNE"); break;
      case '~': drawsymbol("unnnndNEN"); break;
      case '"': drawsymbol("uennnndnueds"); break;
      case '\'': drawsymbol("uennnndn"); break;
      }

   mtxpop();
   }

void drawline(float x1,float y1,float x2,float y2,
              float hue,float sat,float val,float alpha)
   {
   float rgb[3];

   hsv2rgb(hue,sat,val,rgb);

   color(rgb[0],rgb[1],rgb[2],alpha);
   renderline(x1,y1,0.0f,x2,y2,0.0f);
   }

void drawlineRGBA(float x1,float y1,float x2,float y2,
                  float r,float g,float b,float alpha)
   {
   color(r,g,b,alpha);
   renderline(x1,y1,0.0f,x2,y2,0.0f);
   }

void drawquad(float x,float y,float width,float height,
              float hue,float sat,float val,float alpha)
   {
   float rgb[3];

   hsv2rgb(hue,sat,val,rgb);

   color(rgb[0],rgb[1],rgb[2],alpha);

   beginfans();
   beginfan();
   fanvertex(x,y,0.0f);
   fanvertex(x+width,y,0.0f);
   fanvertex(x+width,y+height,0.0f);
   fanvertex(x,y+height,0.0f);
   endfans();
   }

void drawquadRGBA(float x,float y,float width,float height,
                  float r,float g,float b,float alpha)
   {
   color(r,g,b,alpha);

   beginfans();
   beginfan();
   fanvertex(x,y,0.0f);
   fanvertex(x+width,y,0.0f);
   fanvertex(x+width,y+height,0.0f);
   fanvertex(x,y+height,0.0f);
   endfans();
   }

void drawframe(float x,float y,float width,float height,
               float hue,float sat,float val,float alpha)
   {
   float rgb1[3],rgb2[3];

   hsv2rgb(hue,sat,fmax(val-0.25f,0.0f),rgb1);
   hsv2rgb(hue,sat,fmin(val+0.25f,1.0f),rgb2);

   color(rgb1[0],rgb1[1],rgb1[2],alpha);
   renderline(x,y,0.0f,x+width,y,0.0f);
   renderline(x+width,y,0.0f,x+width,y+height,0.0f);

   color(rgb2[0],rgb2[1],rgb2[2],alpha);
   renderline(x+width,y+height,0.0f,x,y+height,0.0f);
   renderline(x,y+height,0.0f,x,y,0.0f);
   }

void drawframeRGBA(float x,float y,float width,float height,
                   float r,float g,float b,float alpha)
   {
   color(r,g,b,alpha);
   renderline(x,y,0.0f,x+width,y,0.0f);
   renderline(x+width,y,0.0f,x+width,y+height,0.0f);
   renderline(x+width,y+height,0.0f,x,y+height,0.0f);
   renderline(x,y+height,0.0f,x,y,0.0f);
   }

void drawstring(float width,
                float hue,float sat,float val,float alpha,const char *str,
                float backval,float backalpha)
   {
   const float linefeed=0.2f;

   int c,cmax,l;
   float scale;
   const char *ptr;

   float rgb[3];

   if (str==NULL) return;

   for (c=0,cmax=l=1,ptr=str; *ptr!='\0'; ptr++)
      {
      if (*ptr!='\n') c++;
      else {c=0; l++;}
      if (c>cmax) cmax=c;
      }

   scale=width/cmax;

   if (backalpha>0.0f && CONFIGURE_ZSCALE<1.0f)
      {
      drawquad(0.0f,0.0f,scale*cmax,scale*(l+(l-1)*linefeed),0.0f,0.0f,backval,backalpha);

      mtxproj();
      mtxpush();
      mtxscale(CONFIGURE_ZSCALE,CONFIGURE_ZSCALE,CONFIGURE_ZSCALE); // prevent Z-fighting
      mtxmodel();

      disableZwriting();
      }

   hsv2rgb(hue,sat,val,rgb);
   color(rgb[0],rgb[1],rgb[2],alpha);

   mtxpush();
   mtxscale(scale,scale,0.0f);
   mtxtranslate(0.0f,(l-1)*(1.0f+linefeed),0.0f);
   mtxpush();

   while (*str!='\0')
      {
      if (*str=='\n')
         {
         mtxpop();
         mtxtranslate(0.0f,-(1.0f+linefeed),0.0f);
         mtxpush();
         }
      else
         {
         if (*str>='a' && *str<='z')
            {
            mtxpush();
            mtxtranslate(0.2f,0.0f,0.0f);
            mtxscale(0.6f,0.75f,1.0f);

            drawletter(*str);

            mtxpop();
            }
         else drawletter(*str);

         mtxtranslate(1.0f,0.0f,0.0f);
         }

      str++;
      }

   mtxpop();
   mtxpop();

   if (backalpha>0.0f && CONFIGURE_ZSCALE<1.0f)
      {
      mtxproj();
      mtxpop();
      mtxmodel();

      enableZwriting();
      }
   }

// configuring
void configure_zfight(float zscale) {CONFIGURE_ZSCALE=zscale;}

}
