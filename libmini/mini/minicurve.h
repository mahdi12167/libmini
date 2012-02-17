// (c) by Stefan Roettger

#ifndef MINICURVE_H
#define MINICURVE_H

#include "minidyna.h"
#include "minicoord.h"

class minicurve : public minidyna<minicoord>
   {
   public:

   minicurve() {}

   void append_sector(const minicoord &p1,const minicoord &p2,
                      unsigned int n=0);

   void sort();
   void validate();

   double get_time_start();
   double get_time_start();
   double get_time_period();

   double get_time_step_min();
   double get_time_step_max();
   double get_time_step_avg();

   private:

   void bisect(const minicoord &p1,const minicoord &p2,
               int level,int maxlevel);
   };

#endif
