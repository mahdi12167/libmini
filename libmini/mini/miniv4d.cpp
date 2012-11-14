// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "miniv4d.h"

#include "miniv4f.h"
#include "miniv3d.h"
#include "miniv3f.h"

#include "ministring.h"

// conversion constructors
miniv4d::miniv4d(const miniv4f &v) {x=(double)v.x; y=(double)v.y; z=(double)v.z; w=(double)v.w;}
miniv4d::miniv4d(const miniv3d &v) {x=v.x; y=v.y; z=v.z; w=0.0;}
miniv4d::miniv4d(const miniv3d &v,const double vw) {x=v.x; y=v.y; z=v.z; w=vw;}
miniv4d::miniv4d(const miniv3f &v) {x=(double)v.x; y=(double)v.y; z=(double)v.z; w=0.0;}
miniv4d::miniv4d(const miniv3f &v,const float vw) {x=(double)v.x; y=(double)v.y; z=(double)v.z; w=(double)vw;}

// string cast operator
miniv4d::operator ministring() const {return((ministring)"(" + x + "," + y + "," + z + "," + w + ")");}

// serialization
ministring miniv4d::to_string() const
   {
   ministring info("miniv4d");

   info.append("(");
   info.append_double(x);
   info.append(",");
   info.append_double(y);
   info.append(",");
   info.append_double(z);
   info.append(",");
   info.append_double(w);
   info.append(")");

   return(info);
   }

// deserialization
void miniv4d::from_string(ministring info)
   {
   if (info.startswith("miniv4d"))
      {
      info=info.tail("miniv4d(");
      info=info.head(")");

      x=info.prefix(",").value();
      info=info.tail(",");
      y=info.prefix(",").value();
      info=info.tail(",");
      z=info.prefix(",").value();
      info=info.tail(",");
      w=info.prefix(",").value();
      }
   }
