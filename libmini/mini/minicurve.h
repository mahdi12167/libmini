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

      set_constraints();

      bboxmin=miniv3d(MAXFLOAT,MAXFLOAT,MAXFLOAT);
      bboxmax=miniv3d(-MAXFLOAT,-MAXFLOAT,-MAXFLOAT);

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

      set_constraints();

      bboxmin=miniv3d(MAXFLOAT,MAXFLOAT,MAXFLOAT);
      bboxmax=miniv3d(-MAXFLOAT,-MAXFLOAT,-MAXFLOAT);

      valid=FALSE;

      append_sector(p1,p2,n);
      }

   virtual ~minicurve() {}

   void append(const minimeas &p);

   void append(minicurve &c);
   void merge(minicurve &c);

   void append_sector(const minicoord &p1,const minicoord &p2,
                      unsigned int n=0);

   void append_sector(const minicoord &p,
                      unsigned int n=0);

   void append_point(const minicoord &p,double t,
                     unsigned int n=0);

   void set_time_mapping(double map_start,double map_stop);
   void set_time_repeat(double repeat_start,double repeat_stop);

   void set_constraints(double min_accuracy=50.0,
                        double max_acceleration=5.0,
                        double max_tolerance=2.0)
      {
      minicurve::min_accuracy=min_accuracy;
      minicurve::max_acceleration=max_acceleration;
      minicurve::max_tolerance=max_tolerance;
      }

   minimeas interpolate(double t);
   minimeas interpolate_cubic(double t);

   double get_time_start();
   double get_time_stop();
   double get_time_period();

   double get_time_step_min();
   double get_time_step_max();
   double get_time_step_avg();

   double get_length();

   void resample(double dt);

   //! get bounding box
   void getbbox(miniv3d &bboxmin,miniv3d &bboxmax);

   //! get bounding sphere
   void getbsphere(miniv3d &center,double &radius2);

   //! serialization
   ministrings to_strings();

   //! deserialization
   void from_strings(ministrings &infos);

   protected:

   BOOLINT valid;

   double curve_start,curve_stop;
   double curve_map_start,curve_map_stop;
   double curve_repeat_start,curve_repeat_stop;

   double min_accuracy;
   double max_acceleration;
   double max_tolerance;

   miniv3d bboxmin,bboxmax;

   void bisect(const minicoord &p1,const minicoord &p2,
               int level,int maxlevel);

   void sort();
   void validate();

   double compute_velocity(unsigned int i);

   virtual BOOLINT check_constraints(double d,double dt,
                                     miniv3d p1,miniv3d p2,double v1,double v2,
                                     double a1,double a2);

   };

#endif
