// (c) by Stefan Roettger

#ifndef MINIANIM_H
#define MINIANIM_H

#include "minidyna.h"
#include "minicoord.h"
#include "minicam.h"

class minianim : public minidyna<minicoord>
   {
   public:

   minianim();
   minianim(minicam *c);
   minianim(minicam *c,const minicoord &v);

   void append_sector(const minicoord &p1,const minicoord &p2,unsigned int n);

   private:

   minicam *cam;
   };

#endif
