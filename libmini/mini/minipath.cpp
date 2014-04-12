// (c) by Stefan Roettger

#include "minixml.h"

#include "minipath.h"

// serialization
ministrings minipath::to_csv()
   {
   unsigned int i;

   ministrings csv;

   unsigned int segment=0;
   unsigned int point=0;

   validate();

   csv.append("\"name\",\"activity\",\"description\"");
   csv.append("\""+name+"\",\""+activity+"\",\""+description+"\"");

   csv.append("");

   csv.append("\"segment\",\"point\",\"lat\",\"lon\",\"height\",\"heading\",\"accuracy\",\"velocity\",\"time\"");

   for (i=0; i<getsize(); i++)
      {
      ministring line;
      minimeas meas;

      meas=get(i);
      meas.convert2llh();

      if (meas.start)
         {
         segment++;
         point=1;
         }

      // segment
      line.append("\"");
      line.append_uint(segment);
      line.append("\"");

      // point
      line.append(",\"");
      line.append_uint(point++);
      line.append("\"");

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
      line.append(",\"");
      line.append_double(meas.heading);
      line.append("\"");

      // accuracy
      line.append(",\"");
      line.append_double(meas.accuracy);
      line.append("\"");

      // velocity
      line.append(",\"");
      line.append_double(meas.velocity);
      line.append("\"");

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

   unsigned int segment=0,nextseg;

   if (csv.getsize()<2) return;

   values.from_string(csv[0],",");

   if (values.getsize()<3) return;

   values.clear();
   values.from_string(csv[1],",");

   if (values.getsize()<3) return;

   name=values[0].tail("\"").head("\"");
   activity=values[1].tail("\"").head("\"");
   description=values[2].tail("\"").head("\"");

   for (i=2; i<csv.getsize(); i++)
      if (!csv[i].empty()) break;

   values.clear();
   values.from_string(csv[i],",");

   if (values.getsize()<9) return;

   for (i++; i<csv.getsize(); i++)
      if (!csv[i].empty())
         {
         minimeas meas;

         values.clear();
         values.from_string(csv[i],",");

         if (values.getsize()<9) return;

         for (j=0; j<values.getsize(); j++)
            values[j]=values[j].tail("\"").head("\"");

         meas.set_llh(values[2].value(),values[3].value(),values[4].value(),utc2unixtime(values[8]));

         meas.accuracy=values[6].value();
         meas.velocity=values[7].value();
         meas.heading=values[5].value();

         nextseg=values[0].value_uint();
         if (nextseg!=segment) meas.start=TRUE;
         segment=nextseg;

         append(meas);

         csv[i].clear();
         }

   csv.clear();
   }

// load path from file
BOOLINT minipath::load(ministring filename)
   {
   BOOLINT success;

   ministrings path;
   ministring info;

   path.load(filename);

   if (path.empty()) return(FALSE);

   path.remove_leading_white_space();
   path.remove_trailing_white_space();

   info=path[0];

   if (info.startswith("minicurve")) success=read_curve_format(path);
   else if (info.startswith("\"")) success=read_csv_format(path);
   else if (info.startswith("<?xml")) success=read_gpx_format(path);
   else if (info=="[track]") success=read_trk_format(path);
   else success=FALSE;

   validate();

   return(success);
   }

// save path to file
void minipath::save(ministring filename)
   {to_strings().save(filename);}

// save path to file in csv format
void minipath::save2csv(ministring filename)
   {to_csv().save(filename);}

// read internal format
BOOLINT minipath::read_curve_format(ministrings &curve)
   {
   from_strings(curve);

   return(curve.empty());
   }

// read csv format
BOOLINT minipath::read_csv_format(ministrings &csv)
   {
   from_csv(csv);

   return(csv.empty());
   }

// read gpx format
BOOLINT minipath::read_gpx_format(ministrings &gpx)
   {
   unsigned int trkseg,trkpt;

   if (gpx.empty()) return(FALSE);

   if (gpx[0].startswith("<?xml"))
      {
      minixml xml;
      xml.from_strings(gpx);

      if (xml.get("gpx.version").empty()) return(FALSE);

      for (trkseg=1; ; trkseg++)
         {
         ministring prefix="gpx.trk.trkseg";
         if (trkseg>1) prefix+="#"+ministring(trkseg);

         if (xml.get(prefix+".trk.trkpt.lat").empty() ||
             xml.get(prefix+".trk.trkpt.lon").empty()) break;

         for (trkpt=1; ; trkpt++)
            {
            ministring infix=".trk.trkpt";
            if (trkpt>1) infix+="#"+ministring(trkpt);

            ministring lat=xml.get(prefix+infix+".lat");
            ministring lon=xml.get(prefix+infix+".lon");
            ministring ele=xml.get(prefix+infix+".ele");
            ministring time=xml.get(prefix+infix+".time");

            if (!lat.empty() && !lon.empty())
               {
               minimeas meas;

               meas.set_llh(lat.value(),lon.value(),ele.value(),utc2unixtime(time));
               if (trkpt==1) meas.start=TRUE;

               append(meas);
               }
            else break;
            }
         }

      return(gpx.empty());
      }

   return(FALSE);
   }

// read trk format
BOOLINT minipath::read_trk_format(ministrings &trk)
   {
   unsigned int i,j;

   if (trk.empty()) return(FALSE);

   if (trk[0]=="[track]")
      for (i=1; i<trk.getsize(); i++)
         if (trk[i]=="--start--")
            {
            for (j=i+1; j<trk.getsize(); j++)
               {
               ministring line=trk[j];

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

                  minicoord coord(miniv4d(lon*3600,lat*3600,elev,time),minicoord::MINICOORD_LLH);

                  BOOLINT start=FALSE;
                  ministring desc;

                  if (line.startswith(";"))
                     {
                     line=line.tail(";");

                     if (line=="#CMDNewSegment") start=TRUE;
                     else desc=line;

                     line.clear();
                     }

                  minimeas meas(coord,NAN,NAN,NAN,start);
                  meas.set_description(desc);

                  append(meas);
                  }

               if (!line.empty()) return(FALSE);
               }

            return(TRUE);
            }

   return(FALSE);
   }
