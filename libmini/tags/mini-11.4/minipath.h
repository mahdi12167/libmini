// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIPATH_H
#define MINIPATH_H

#include "minicurve.h"

class minipath : public minicurve
   {
   public:

   //! default constructor
   minipath(double start=0.0,double stop=1.0)
      : minicurve(start,stop),
      name("path"), activity("none"), description("created by libmini")
      {set_constraints(50.0,5.0);}

   //! constructor
   minipath(ministring filename,
            double start=0.0,double stop=1.0)
      : minicurve(start,stop),
      name("path"), activity("none"), description("created by libmini")
      {
      set_constraints(50.0,5.0);
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

#endif
