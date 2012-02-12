// (c) by Stefan Roettger

#ifndef MINICURVE_H
#define MINICURVE_H

#include "minidyna.h"
#include "minicoord.h"

class minicurve : public minidyna<minicoord>
   {
   public:

   minicurve() {}

   void append_sector(const minicoord &p1,const minicoord &p2,int n);

   private:

   void bisect(const minicoord &p1,const minicoord &p2,int level);
   };

#endif