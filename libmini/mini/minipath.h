// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIPATH_H
#define MINIPATH_H

#include <string>
#include <vector>

#include "minicurve.h"

class minipath: public minicurve
   {
   public:

   //! default constructor
   minipath(double start=0.0,double stop=1.0,
            double max_delta=3600.0,double max_length=50000.0,double min_accuracy=50.0,double max_accel=2.0,double max_tol=2.0,
            int orb=minicoord::MINICOORD_ORB_NONE)
      : minicurve(start,stop),
        name("path"), activity("none"), description("created by libmini")
      {
      set_constraints(max_delta,max_length,min_accuracy,max_accel,max_tol);
      set_orb(orb);
      }

   //! constructor
   minipath(ministring filename,
            double start=0.0,double stop=1.0,
            double max_delta=3600.0,double max_length=50000.0,double min_accuracy=50.0,double max_accel=2.0,double max_tol=2.0,
            int orb=minicoord::MINICOORD_ORB_NONE)
      : minicurve(start,stop),
        name("path"), activity("none"), description("created by libmini")
      {
      set_constraints(max_delta,max_length,min_accuracy,max_accel,max_tol);
      set_orb(orb);

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

   //! serialization
   std::string to_stdstring();

   //! deserialization
   void from_stdstring(const std::string &csv);

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
   minipaths(double max_delta=3600.0,double max_length=50000.0,double min_accuracy=50.0,
             int orb=minicoord::MINICOORD_ORB_NONE);

   //! get constraints
   void get_constraints(double &max_delta,
                        double &max_length,
                        double &min_accuracy)
      {
      max_delta=max_delta_;
      max_length=max_length_;
      min_accuracy=min_accuracy_;
      }

   //! get orbital
   int get_orb() {return(orb_);}

   //! append path
   void append(const minipath &path) {push_back(path);}

   //! conversion of multiple paths to a combined one
   operator minipath() const;

   //! serialization
   std::vector<std::string> to_stdstrings();

   //! deserialization
   void from_stdstrings(const std::vector<std::string> &csvs);

   protected:

   double max_delta_;
   double max_length_;
   double min_accuracy_;

   int orb_;
   };

#endif
