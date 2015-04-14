// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINICURVE_H
#define MINICURVE_H

#include "minidyna.h"
#include "minimeas.h"

class minicurve: public minidyna<minimeas>
   {
   public:

   minicurve(double start=0.0,double stop=1.0,
             int orb=minicoord::MINICOORD_ORB_NONE)
      : minidyna<minimeas>(),
        crs_orb(orb)
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
      : minidyna<minimeas>(),
        crs_orb(minicoord::MINICOORD_ORB_NONE)
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

   void set_orb(int orb);

   int get_orb() const {return(crs_orb);}

   void append(const minimeas &p);

   void append(const minicurve &c);

   void append_sector(const minicoord &p1,const minicoord &p2,
                      unsigned int n=0);

   void append_sector(const minicoord &p,
                      unsigned int n=0);

   void append_point(const minicoord &p,double t,
                     unsigned int n=0);

   void dispose(unsigned int i);

   void set_time_mapping(double map_start,double map_stop);
   void set_time_repeat(double repeat_start,double repeat_stop);

   void set_constraints(double max_delta=MAXFLOAT,
                        double max_length=MAXFLOAT,
                        double min_accuracy=MAXFLOAT,
                        double max_acceleration=MAXFLOAT,
                        double max_tolerance=MAXFLOAT)
      {
      minicurve::max_delta=max_delta;
      minicurve::max_length=max_length;
      minicurve::min_accuracy=min_accuracy;
      minicurve::max_acceleration=max_acceleration;
      minicurve::max_tolerance=max_tolerance;
      }

   void unset_constraints()
      {set_constraints();}

   void get_constraints(double &max_delta,
                        double &max_length,
                        double &min_accuracy) const
      {
      max_delta=minicurve::max_delta;
      max_length=minicurve::max_length;
      min_accuracy=minicurve::min_accuracy;
      }

   void get_constraints(double &max_delta,
                        double &max_length,
                        double &min_accuracy,
                        double &max_acceleration,
                        double &max_tolerance) const
      {
      max_delta=minicurve::max_delta;
      max_length=minicurve::max_length;
      min_accuracy=minicurve::min_accuracy;
      max_acceleration=minicurve::max_acceleration;
      max_tolerance=minicurve::max_tolerance;
      }

   void validate();
   BOOLINT is_valid() const {return(valid);}

   double get_time_start();
   double get_time_stop();
   double get_time_period();

   double get_time_step_min();
   double get_time_step_max();
   double get_time_step_avg();

   double get_length();

   minimeas interpolate(double t);
   minimeas interpolate_cubic(double t);

   void resample(double dt);

   //! get bounding box
   void getbbox(miniv3d &bboxmin,miniv3d &bboxmax);

   //! get bounding sphere
   void getbsphere(miniv3d &center,double &radius2);

   //! serialization
   ministring to_string();

   //! deserialization
   void from_string(ministring &info);

   //! serialize to string list
   ministrings to_strings();

   //! deserialize from string list
   void from_strings(ministrings &infos);

   protected:

   int crs_orb;

   BOOLINT valid;

   double curve_start,curve_stop;
   double curve_map_start,curve_map_stop;
   double curve_repeat_start,curve_repeat_stop;

   double max_delta;
   double max_length;
   double min_accuracy;
   double max_acceleration;
   double max_tolerance;

   miniv3d bboxmin,bboxmax;

   void bisect(const minicoord &p1,const minicoord &p2,
               int level,int maxlevel);

   void validate_props(unsigned int a,unsigned int b);
   void update_bbox(unsigned int a,unsigned int b);

   void sort();

   double compute_velocity(unsigned int i);

   virtual BOOLINT check_constraints(double d,double dt,
                                     miniv3d p1,miniv3d p2,double v1,double v2,
                                     double a1,double a2);

   };

#endif
