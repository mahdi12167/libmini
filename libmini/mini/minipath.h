// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIPATH_H
#define MINIPATH_H

#include "minicurve.h"

class minipath : public minicurve
   {
   public:

   minipath(double start=0.0,double stop=1.0)
      : minicurve(start,stop)
      {}

   BOOLINT load(ministring filename);
   void save(ministring filename);

   //! serialization
   ministrings to_csv() const;

   //! deserialization
   void from_csv(ministrings &info);

   protected:

   BOOLINT read_curve_format(ministrings curve);
   BOOLINT read_csv_format(ministrings csv);
   BOOLINT read_trk_format(ministrings trk);
   BOOLINT read_gpx_format(ministrings gpx);
   };

#endif
