// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINICURVE_H
#define MINICURVE_H

#include "minidyna.h"
#include "minimeas.h"

class minicurve : public minidyna<minimeas>
   {
   public:

   minicurve(double start=0.0,double stop=1.0)
      : minidyna<minimeas>()
      {
      curve_start=start;
      curve_stop=stop;

      curve_map_start=curve_map_stop=0.0;
      curve_repeat_start=curve_repeat_stop=0.0;

      valid=FALSE;
      }

   minicurve(const minicoord &p1,const minicoord &p2,
             unsigned int n=0)
      : minidyna<minimeas>()
      {
      curve_start=0.0;
      curve_stop=1.0;

      curve_map_start=curve_map_stop=0.0;
      curve_repeat_start=curve_repeat_stop=0.0;

      valid=FALSE;

      append_sector(p1,p2,n);
      }

   void append(const minimeas &p);

   void append_sector(const minicoord &p1,const minicoord &p2,
                      unsigned int n=0);

   void append_sector(const minicoord &p,
                      unsigned int n=0);

   void append_point(const minicoord &p,double t,
                     unsigned int n=0);

   void set_time_mapping(double map_start,double map_stop);
   void set_time_repeat(double repeat_start,double repeat_stop);

   minimeas interpolate(double t);
   minimeas interpolate_cubic(double t);

   double get_time_start();
   double get_time_stop();
   double get_time_period();

   double get_time_step_min();
   double get_time_step_max();
   double get_time_step_avg();

   void resample(double dt);

   //! serialization
   ministrings to_strings() const;

   //! deserialization
   void from_strings(ministrings &info);

   private:

   BOOLINT valid;

   double curve_start,curve_stop;
   double curve_map_start,curve_map_stop;
   double curve_repeat_start,curve_repeat_stop;

   void bisect(const minicoord &p1,const minicoord &p2,
               int level,int maxlevel);

   void sort();
   void validate();
   };

#endif
