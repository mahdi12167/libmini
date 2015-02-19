// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIPATH_H
#define MINIPATH_H

#include "minicurve.h"

class minipath: public minicurve
   {
   public:

   //! default constructor
   minipath(double start=0.0,double stop=1.0,
            double max_delta=3600.0,double max_length=50000.0,double min_accuracy=50.0,double max_accel=2.0,double max_tol=2.0)
      : minicurve(start,stop),
        name("path"), activity("none"), description("created by libmini")
      {set_constraints(max_delta,max_length,min_accuracy,max_accel,max_tol);}

   //! constructor
   minipath(ministring filename,
            double start=0.0,double stop=1.0,
            double max_delta=3600.0,double max_length=50000.0,double min_accuracy=50.0,double max_accel=2.0,double max_tol=2.0)
      : minicurve(start,stop),
        name("path"), activity("none"), description("created by libmini")
      {
      set_constraints(max_delta,max_length,min_accuracy,max_accel,max_tol);
      load(filename);
      }

   //! load path from file
   BOOLINT load(ministring filename);

   //! save path to file
   void save(ministring filename);

   //! save path to file in csv format
   void save2csv(ministring filename);

   //! serialization
   ministrings to_csv();

   //! deserialization
   void from_csv(ministrings &csv);

   ministring name;
   ministring activity;
   ministring description;

   protected:

   BOOLINT read_curve_format(ministrings &curve);
   BOOLINT read_csv_format(ministrings &csv);
   BOOLINT read_gpx_format(ministrings &gpx);
   BOOLINT read_trk_format(ministrings &trk);
   };

class minipaths: public minidyna<minipath>
   {
   public:

   //! default constructor
   minipaths(double max_delta=3600.0,double max_length=50000.0,double min_accuracy=50.0,double max_accel=2.0,double max_tol=2.0);

   //! conversion of multiple paths to a combined one
   operator minipath();

   protected:

   double max_delta_;
   double max_length_;
   double min_accuracy_;
   double max_accel_;
   double max_tol_;
   };

#endif
