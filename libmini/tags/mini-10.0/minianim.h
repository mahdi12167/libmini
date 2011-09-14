// (c) by Stefan Roettger

#ifndef MINIANIM_H
#define MINIANIM_H

#include "minidyna.h"
#include "minicoord.h"

class minianim : public minidyna<minicoord>
   {
   public:

   minianim() {}

   void append_sector(const minicoord &p1,const minicoord &p2,int n);

   private:

   void bisect(const minicoord &p1,const minicoord &p2,int level);
   };

#endif
