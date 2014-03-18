// (c) by Stefan Roettger

#include "minipath.h"

// serialization
ministrings minipath::to_csv() const
   {
   unsigned int i;

   ministrings csv;

   csv.append("\"name\",\"activity\",\"description\"");
   csv.append("\""+name+"\",\""+activity+"\",\""+description+"\"");

   csv.append("");

   csv.append("\"segment\",\"lat\",\"lon\",\"height\",\"heading\",\"accuracy\",\"velocity\",\"time\"");

   for (i=0; i<getsize(); i++)
      {
      ministring line;
      minimeas meas;

      meas=get(i);
      meas.convert2llh();

      // segment
      line.append("\"1\"");

      // lat
      line.append(",\"");
      line.append_double(meas.vec.y/3600);
      line.append("\"");

      // lon
      line.append(",\"");
      line.append_double(meas.vec.x/3600);
      line.append("\"");

      // height
      line.append(",\"");
      line.append_double(meas.vec.z);
      line.append("\"");

      // heading
      line.append(",\"0\"");

      // accuracy
      line.append(",\"0\"");

      // velocity
      line.append(",\"0\"");

      // time
      line.append(",\"");
      line.append_double(meas.vec.w);
      line.append("\"");

      csv.append(line);
      }

   return(csv);
   }

// deserialization
void minipath::from_csv(ministrings &csv)
   {
   unsigned int i,j;

   ministrings values;

   if (csv.getsize()<2) return;

   values.from_string(csv[0],",");

   if (values.getsize()<3) return;

   values.from_string(csv[1],",");

   if (values.getsize()<3) return;

   name=values[0].tail("\"").head("\"");
   activity=values[1].tail("\"").head("\"");
   description=values[2].tail("\"").head("\"");

   for (i=2; i<csv.getsize(); i++)
      if (!csv[i].empty()) break;

   values.from_string(csv[i],",");

   if (values.getsize()<8) return;

   for (i++; i<csv.getsize(); i++)
      if (!csv[i].empty())
         {
         minimeas meas;

         values.from_string(csv[i],",");

         if (values.getsize()<8) return;

         for (j=0; j<values.getsize(); j++)
            values[j]=values[j].tail("\"").head("\"");

         meas.set_llh(values[1].value(),values[2].value(),values[3].value(),values[7].value());

         append(meas);

         csv[i].clear();
         }

   csv.clear();
   }

BOOLINT minipath::load(ministring filename)
   {
   BOOLINT success;

   ministrings path;
   ministring info;

   path.load(filename);

   if (path.empty()) return(FALSE);

   info=path[0];

   if (info.startswith("minicurve")) success=read_curve_format(path);
   else if (info.startswith("\"")) success=read_csv_format(path);
   else if (info.startswith("<?xml")) success=read_gpx_format(path);
   else if (info=="[track]") success=read_trk_format(path);
   else success=FALSE;

   return(success);
   }

void minipath::save(ministring filename)
   {to_strings().save(filename);}

BOOLINT minipath::read_curve_format(ministrings &curve)
   {
   from_strings(curve);

   return(curve.empty());
   }

BOOLINT minipath::read_csv_format(ministrings &csv)
   {
   from_csv(csv);

   return(csv.empty());
   }

BOOLINT minipath::read_gpx_format(ministrings &gpx)
   {return(FALSE);}

BOOLINT minipath::read_trk_format(ministrings &trk)
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

               minimeas meas(miniv4d(lon*3600,lat*3600,elev,time),minicoord::MINICOORD_LLH);

               if (line.startswith(";"))
                  {
                  line=line.tail(";");
                  meas.set_description(line);
                  line.clear();
                  }

               append(meas);
               }

            if (!line.empty()) return(FALSE);
            }

         return(TRUE);
         }

   return(FALSE);
   }
