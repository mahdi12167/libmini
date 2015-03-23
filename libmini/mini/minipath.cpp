// (c) by Stefan Roettger

#include "minixml.h"
#include "minisort.h"

#include "minipath.h"

// serialization
ministrings minipath::to_csv()
   {
   unsigned int i;

   ministrings csv;

   unsigned int segment=0;
   unsigned int point=1;

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

      // inclination
      line.append(",\"");
      line.append_double(meas.inclination);
      line.append("\"");

      // power
      line.append(",\"");
      line.append_double(meas.power);
      line.append("\"");

      // frequency
      line.append(",\"");
      line.append_double(meas.frequency);
      line.append("\"");

      // heartbeat
      line.append(",\"");
      line.append_double(meas.heartbeat);
      line.append("\"");

      // description and metadata
      if (!meas.get_description().empty() || !meas.get_metadata().empty())
         {
         line.append(",\"");
         line.append(meas.get_description());
         line.append("\"");

         if (!meas.get_metadata().empty())
            {
            line.append(",\"");
            line.append(meas.get_metadata());
            line.append("\"");
            }
         }

      csv.append(line);
      }

   return(csv);
   }

// deserialization
void minipath::from_csv(ministrings &csv)
   {
   unsigned int i;

   unsigned int line;

   ministrings values;

   unsigned int segment=0,nextseg;

   line=0;

   while (line+2<=csv.getsize())
      {
      values.clear();
      values.from_string(csv[line],",");

      line++;

      if (values.getsize()==3)
         {
         values.clear();
         values.from_string(csv[line],",");

         if (values.getsize()==3)
            {
            name=values[0].tail("\"").head("\"");
            activity=values[1].tail("\"").head("\"");
            description=values[2].tail("\"").head("\"");

            line++;
            }
         }
      else if (values.getsize()>=9)
         while (line<csv.getsize())
            {
            if (csv[line].empty()) break;

            minimeas meas;

            values.clear();
            values.from_string(csv[line],",");

            if (values.getsize()<9) break;

            for (i=0; i<values.getsize(); i++)
               values[i]=values[i].tail("\"").head("\"");

            // lat/lon/h /w time
            meas.set_llh(values[2].value(),values[3].value(),values[4].value(),utc2unixtime(values[8]),crs_orb);

            // physical properties
            meas.accuracy=values[6].value();
            meas.velocity=values[7].value();
            meas.heading=values[5].value();

            // optional inclination
            if (values.getsize()>=10)
               meas.inclination=values[9].value();

            // optional health properties
            if (values.getsize()>=11)
               meas.power=values[10].value();
            if (values.getsize()>=12)
               meas.frequency=values[11].value();
            if (values.getsize()>=13)
               meas.heartbeat=values[12].value();

            // optional description
            if (values.getsize()>=14)
               meas.set_description(values[13]);

            // optional metadata
            if (values.getsize()>=15)
               meas.set_metadata(values[14]);

            nextseg=values[0].value_uint();
            if (nextseg!=segment) meas.start=TRUE;
            segment=nextseg;

            append(meas);

            csv[line].clear();

            line++;
            }
      else
         line++;

      for (line++; line<csv.getsize(); line++)
         if (!csv[line].empty()) break;
      }

   csv.clear();

   validate();
   }

// serialization
std::string minipath::to_stdstring()
   {return(to_csv().to_string("\n").c_str());}

// deserialization
void minipath::from_stdstring(const std::string &csv)
   {
   ministrings strs;
   strs.from_string(csv.c_str(),"\n");
   from_csv(strs);
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
   unsigned int trk,trkseg,trkpt;

   if (gpx.empty()) return(FALSE);

   if (gpx[0].startswith("<?xml"))
      {
      minixml xml;
      xml.from_strings(gpx);

      if (xml.get("gpx.version").empty()) return(FALSE);

      if (!xml.get("gpx.description").empty())
         description=xml.get("gpx.description");

      for (trk=1; ; trk++)
         {
         ministring prefix="gpx.trk";
         if (trk>1) prefix+="#"+ministring(trk);

         if (xml.get(prefix+".trkseg.trkpt.lat").empty() ||
             xml.get(prefix+".trkseg.trkpt.lon").empty() ||
             xml.get(prefix+".trkseg.trkpt.time").empty()) break;

         for (trkseg=1; ; trkseg++)
            {
            ministring infix=".trkseg";
            if (trkseg>1) infix+="#"+ministring(trkseg);

            if (xml.get(prefix+infix+".trkpt.lat").empty() ||
                xml.get(prefix+infix+".trkpt.lon").empty() ||
                xml.get(prefix+infix+".trkpt.time").empty()) break;

            for (trkpt=1; ; trkpt++)
               {
               ministring infix2=".trkpt";
               if (trkpt>1) infix2+="#"+ministring(trkpt);

               ministring lat=xml.get(prefix+infix+infix2+".lat");
               ministring lon=xml.get(prefix+infix+infix2+".lon");
               ministring ele=xml.get(prefix+infix+infix2+".ele");
               ministring time=xml.get(prefix+infix+infix2+".time");
               ministring hdop=xml.get(prefix+infix+infix2+".hdop");
               ministring vdop=xml.get(prefix+infix+infix2+".vdop");
               ministring pdop=xml.get(prefix+infix+infix2+".pdop");
               ministring speed=xml.get(prefix+infix+infix2+".speed");
               ministring degrees=xml.get(prefix+infix+infix2+".degrees");
               ministring description=xml.get(prefix+infix+infix2+".description");

               if (lat.empty() || lon.empty() || time.empty()) break;

               minimeas meas;

               meas.set_llh(lat.value(),lon.value(),ele.value(),utc2unixtime(time),crs_orb);

               meas.accuracy=pdop.value();
               if (isNAN(meas.accuracy))
                  {
                  meas.accuracy=hdop.value();
                  if (isNAN(meas.accuracy))
                     meas.accuracy=vdop.value();
                  }

               meas.velocity=speed.value();
               meas.heading=degrees.value();

               if (!description.empty())
                  meas.set_description(description);

               append(meas);
               }
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

                  minicoord coord(miniv4d(lon*3600,lat*3600,elev,time),
                                  minicoord::MINICOORD_LLH,0,minicoord::MINICOORD_DATUM_NONE,crs_orb);

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

// default constructor
minipaths::minipaths(double max_delta,double max_length,double min_accuracy,
                     int orb)
   : minidyna<minipath *>(),
     max_delta_(max_delta),
     max_length_(max_length),
     min_accuracy_(min_accuracy),
     orb_(orb)
   {sorted_=TRUE;}

// copy constructor
minipaths::minipaths(const minipaths &paths)
   {
   paths.get_constraints(max_delta_,max_length_,min_accuracy_);
   orb_=paths.get_orb();

   for (unsigned int i=0; i<size(); i++)
      delete get(i);

   clear();

   for (unsigned int i=0; i<paths.size(); i++)
      append(*paths.get(i));

   sorted_=paths.sorted_;
   }

// destructor
minipaths::~minipaths()
   {
   for (unsigned int i=0; i<size(); i++)
      delete get(i);
   }

//! assignment operator
minipaths& minipaths::operator = (const minipaths &paths)
{
   paths.get_constraints(max_delta_,max_length_,min_accuracy_);
   orb_=paths.get_orb();

   for (unsigned int i=0; i<size(); i++)
      delete get(i);

   clear();

   for (unsigned int i=0; i<paths.size(); i++)
      append(*paths.get(i));

   sorted_=paths.sorted_;

   return(*this);
}

// append path
void minipaths::append(const minipath &path)
   {
   push_back(new minipath(path));
   sorted_=FALSE;
   }

// sort paths by starting time
void minipaths::sort()
   {
   if (!sorted_)
      {
      mergesort<minipath>(*this);
      sorted_=TRUE;
      }
   }

// conversion operator
minipaths::operator minipath()
   {
   minipath paths;

   paths.set_constraints(max_delta_,max_length_,min_accuracy_);
   paths.set_orb(orb_);

   sort();

   for (unsigned int i=0; i<size(); i++)
      {
      minipath path=*get(i);
      paths.append(path);
      }

   return(paths);
   }

// serialization
std::vector<std::string> minipaths::to_stdstrings()
   {
   std::vector<std::string> csvs;

   sort();

   for (unsigned int i=0; i<size(); i++)
      {
      minipath path=*get(i);
      csvs.push_back(path.to_stdstring());
      }

   return(csvs);
   }

// deserialization
void minipaths::from_stdstrings(const std::vector<std::string> &csvs)
   {
   for (unsigned int i=0; i<csvs.size(); i++)
      {
      minipath path;
      path.from_stdstring(csvs[i]);
      append(path);
      }
   }
