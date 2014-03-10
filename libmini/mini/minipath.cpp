// (c) by Stefan Roettger

#include "minipath.h"

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
   {writeministring(filename.c_str(),to_string());}

BOOLINT minipath::read_curve_format(ministrings curve)
   {
   if (curve.size()!=1) return(FALSE);

   from_string(curve[0]);

   return(TRUE);
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
