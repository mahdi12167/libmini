// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIPATH_H
#define MINIPATH_H

#include "minicurve.h"

class minipath : public minicurve
   {
   public:

   minipath(double start=0.0,double stop=1.0)
      : minicurve(start,stop),
      name("path"), activity("none"), description("created by libmini")
      {}

   BOOLINT load(ministring filename);
   void save(ministring filename);

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
