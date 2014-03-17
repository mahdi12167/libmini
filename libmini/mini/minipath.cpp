// (c) by Stefan Roettger

#include "minipath.h"

// serialization
ministrings minipath::to_csv() const
   {
   ministrings csv;

   return(csv); //!!
   }

// deserialization
void minipath::from_csv(ministrings &info)
   {
   }

BOOLINT minipath::load(ministring filename)
   {
   BOOLINT success;

   ministrings path;

   path.load(filename);

   if (path.empty()) return(FALSE);

   if (path[0].startswith("minicurve")) success=read_curve_format(path);
   else if (path[0]=="[track]") success=read_trk_format(path);
   else success=FALSE;

   return(success);
   }

void minipath::save(ministring filename)
   {to_strings().save(filename);}

BOOLINT minipath::read_curve_format(ministrings curve)
   {
   from_strings(curve);

   return(curve.empty());
   }

BOOLINT minipath::read_csv_format(ministrings csv)
   {
   return(FALSE); //!!
   }

BOOLINT minipath::read_trk_format(ministrings trk)
   {
   if (trk.size()>4)
      if (trk[0]=="[track]" && trk[3]=="--start--")
         {
         for (unsigned int i=4; i<trk.size(); i++)
            {
            ministring line=trk[i];

            if (line.startswith("("))
               {
               double lat,lon,elev,time;

               line=line.tail("(");
               lat=line.prefix(",").value();
               line=line.tail(",");
               lon=line.prefix(",").value();
               line=line.tail(",");
               elev=line.prefix(",").value();
               line=line.tail(",");
               time=line.prefix(",").value();
               line=line.tail(")");

               append(minicoord(miniv4d(lon*3600,lat*3600,elev,time),minicoord::MINICOORD_LLH));
               }

            if (!line.empty()) return(FALSE);
            }

         return(TRUE);
         }

   return(FALSE);
   }

BOOLINT minipath::read_gpx_format(ministrings gpx)
   {
   return(FALSE); //!!
   }
