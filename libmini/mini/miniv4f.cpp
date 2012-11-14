// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "miniv4f.h"

#include "miniv4d.h"
#include "miniv3f.h"
#include "miniv3d.h"

#include "ministring.h"

// conversion constructors
miniv4f::miniv4f(const miniv4d &v) {x=(float)v.x; y=(float)v.y; z=(float)v.z; w=(float)v.w;}
miniv4f::miniv4f(const miniv3f &v) {x=v.x; y=v.y; z=v.z; w=0.0f;}
miniv4f::miniv4f(const miniv3f &v,const float vw) {x=v.x; y=v.y; z=v.z; w=vw;}
miniv4f::miniv4f(const miniv3d &v) {x=(float)v.x; y=(float)v.y; z=(float)v.z; w=0.0f;}
miniv4f::miniv4f(const miniv3d &v,const double vw) {x=(float)v.x; y=(float)v.y; z=(float)v.z; w=(float)vw;}

// string cast operator
miniv4f::operator ministring() const {return((ministring)"(" + x + "," + y + "," + z + "," + w + ")");}

// serialization
ministring miniv4f::to_string() const
   {
   ministring info("miniv4f");

   info.append("(");
   info.append_float(x);
   info.append(",");
   info.append_float(y);
   info.append(",");
   info.append_float(z);
   info.append(",");
   info.append_float(w);
   info.append(")");

   return(info);
   }

// deserialization
void miniv4f::from_string(ministring info)
   {
   if (info.startswith("miniv4f"))
      {
      info=info.tail("miniv4f(");
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
